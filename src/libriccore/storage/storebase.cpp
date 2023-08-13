#include "storebase.h"
#include <iostream>
#include "appendrequest.h"

#include <memory>



StoreBase::StoreBase(RicCoreThread::Lock &device_lock) : device_lock(device_lock),
                                          t([this](void *arg)
                                            { this->StoreBase::flush_task(arg); },
                                            reinterpret_cast<void *>(this)),
                                          file_desc(0),
                                          done(false) {}

StoreBase::~StoreBase()
{
    done = true;
    has_work.up(); // Just incase the other thread is sleeping
    // need to 'wait' for thread to die here with a join like method
}

std::unique_ptr<WrappedFile> StoreBase::open(std::string path, FILE_MODE mode) {
    RicCoreThread::ScopedLock sl(device_lock);
    return _open(path, mode);
}

bool StoreBase::ls(std::string path, std::vector<directory_element_t> &directory_structure) {
    RicCoreThread::ScopedLock sl(device_lock);
    return _ls(path, directory_structure);
}

bool StoreBase::mkdir(std::string path) {
    RicCoreThread::ScopedLock sl(device_lock);
    return _mkdir(path);
}

bool StoreBase::remove(std::string path) {
    RicCoreThread::ScopedLock sl(device_lock);
    return _remove(path); // QUESTION: Should this close the file automatically?
}

void StoreBase::append(std::unique_ptr<AppendRequest> request_ptr) { 
    //std::move to transfer ownershp of the append request to the queue
    queues.at(request_ptr->file->file_desc).send(std::move(request_ptr));
    has_work.up(); //Must have sequential consistency (guarantee that the up happens after the channel send)
    
}

void StoreBase::flush_task(void* args) {
    std::unique_ptr<AppendRequest> req;
    WrappedFile* file;
    
    while (true) {
        //yield thread while there is no work
        has_work.waitForWork();
        thread_lock.acquire(); // locks the queues container

        //iterate thru every queue in the queues container for each file, and process any pending writes
        for (auto& [file_desc, queue] : queues) {
            bool pending_flush = false;
            file = nullptr; // Make sure we don't accidentally write to the wrong file
            //need to verify that the file still exists using the fd
            
            while (!queue.empty()) {
                
                //take 'ownership' of the first append request and remove from the queue
                req = std::move(queue.pop());   
                //update the file pointer to the request file
                file = req->file;

                {
                    RicCoreThread::ScopedLock l(device_lock);
                    //call underlying write to file
                    file->file_write(req->data);
                }

                pending_flush = true;
            }

            if (pending_flush) { // file can't be nullptr here implicitly
            //process flush at the end to try and take advatnage of multi-block writes
            //only really works if the underlying storage system supports this
                file->file_flush();
            }
        }
        //update semaphore that work is done
        has_work.down();
        //release store thread lock
        thread_lock.release();
        if (done) break;
    }
}

store_fd StoreBase::get_next_fd() {
    //generate new file descriptor, increment desc as system lifetime is unlikely going to exceed the number of files allocated
    store_fd desc = file_desc++;

    RicCoreThread::ScopedLock sl(thread_lock);
    //consturct append channel in the queues container
    queues.emplace(std::piecewise_construct,
                    std::forward_as_tuple(desc),
                    std::forward_as_tuple());
    return desc;
}

void StoreBase::release_fd(store_fd file_desc) {
    // Make sure all pending writes have been written
    thread_lock.acquire();
    while (!queues.at(file_desc).empty()) {
        thread_lock.release();
        RicCoreThread::delay(20); // Give flush_task some time to acquire the lock
        thread_lock.acquire();
    }

    queues.erase(file_desc);
    thread_lock.release();
}