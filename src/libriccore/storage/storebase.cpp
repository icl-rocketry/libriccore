#include "storebase.h"
#include <iostream>
#include "appendrequest.h"

#include <memory>

StoreBase::StoreBase(Lock &device_lock) : device_lock(device_lock),
                                          t([this](void *arg)
                                            { this->StoreBase::flush_task(arg); },
                                            reinterpret_cast<void *>(this)),
                                          file_desc(0),
                                          done(false) {}

StoreBase::~StoreBase()
{
    done = true;
    has_work.up(); // Just incase the other thread is sleeping
}

std::unique_ptr<WrappedFile> StoreBase::open(std::string path, FILE_MODE mode) {
    ScopedLock sl(device_lock);
    return _open(path, mode);
}

bool StoreBase::ls(std::string path, std::vector<directory_element_t> &directory_structure) {
    ScopedLock sl(device_lock);
    return _ls(path, directory_structure);
}

bool StoreBase::mkdir(std::string path) {
    ScopedLock sl(device_lock);
    return _mkdir(path);
}

bool StoreBase::remove(std::string path) {
    ScopedLock sl(device_lock);
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
        has_work.down(); // this yields the thread
        thread_lock.acquire(); // locks the queues container
        for (auto& [file_desc, queue] : queues) {
            bool pending_flush = false;
            file = nullptr; // Make sure we don't accidentally write to the wrong file
            //need to verify that the file still exists using the fd
            
            while (!queue.empty()) {

                req = std::move(queue.pop());

                file = req->file;

                {
                    ScopedLock l(device_lock);
                    file->file_write(req->data);
                }

                if (req->done != nullptr) {
                    *(req->done) = true;
                }
                pending_flush = true;
            }

            if (pending_flush) { // file can't be nullptr here implicitly
                file->file_flush();
            }
        }
        thread_lock.release();
        if (done) break;
    }
}

store_fd StoreBase::get_next_fd() {
    store_fd desc = file_desc++;

    ScopedLock sl(thread_lock);
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
        delay(20); // Give flush_task some time to acquire the lock
        thread_lock.acquire();
    }

    queues.erase(file_desc);
    thread_lock.release();
}