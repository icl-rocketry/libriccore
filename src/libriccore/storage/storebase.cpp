#include "storebase.h"
#include <iostream>
#include <memory>
#include <exception>
#include <cctype>
#include <string>

#include <libriccore/riccorelogging.h>

#include <libriccore/threading/riccorethread.h>
#include <libriccore/threading/uniqueptrchannel.h>
#include <libriccore/threading/scopedlock.h>

#include "appendrequest.h"

StoreBase::StoreBase(RicCoreThread::Lock_t &device_lock) : device_lock(device_lock),
                                            _storeState(STATE::NOMINAL),
                                          flush_thread(
                                            [this](void *arg){this->StoreBase::flush_task(arg);},
                                            reinterpret_cast<void *>(this),
                                            10000,
                                            1,
                                            RicCoreThread::Thread::CORE_ID::CORE0,
                                            "flushtask"),
                                          file_desc(0),
                                          done(false) {}

StoreBase::~StoreBase()
{
    done = true;
    has_work = true; // Just incase the other thread is sleeping
    // need to 'wait' for thread to die here with a join like method
}

std::unique_ptr<WrappedFile> StoreBase::open(std::string_view path, FILE_MODE mode, size_t maxQueueSize) {
    if (_storeState != STATE::NOMINAL)
    {
        return nullptr;
    }
    //need to ensure sequential locking i.e thread lock first, then dev lock otherwise we have a deadlock with the flush task
    //so we generate the next file descriptior first, then go get the file from the underlying storage
    store_fd fileDesc = get_next_fd();
    RicCoreThread::ScopedLock sl(device_lock);
    return _open(path, fileDesc, mode, maxQueueSize);
}

bool StoreBase::ls(std::string_view path, std::vector<directory_element_t> &directory_structure) {
    if (_storeState != STATE::NOMINAL)
    {
        return false;
    }
    RicCoreThread::ScopedLock sl(device_lock);
    return _ls(path, directory_structure);
}

bool StoreBase::mkdir(std::string_view path) {
    if (_storeState != STATE::NOMINAL)
    {
        return false;
    }
    RicCoreThread::ScopedLock sl(device_lock);
    return _mkdir(path);
}

bool StoreBase::remove(std::string_view path) {
    if (_storeState != STATE::NOMINAL)
    {
        return false;
    }
    RicCoreThread::ScopedLock sl(device_lock);
    return _remove(path); // QUESTION: Should this close the file automatically?
}

bool StoreBase::append(std::unique_ptr<AppendRequest> request_ptr) { 
    //std::move to transfer ownershp of the append request to the queue
    if (_storeState != STATE::NOMINAL)
    {
        return false;
    }
    queues.at(request_ptr->file->file_desc).send(std::move(request_ptr));
    has_work = true; 
    return true;
    //Must have sequential consistency (guarantee that the up happens after the channel send)
    
}

void StoreBase::flush_task(void* args) { 
    std::unique_ptr<AppendRequest> req;
    WrappedFile* file;
    
    while (true) {
        //yield thread while there is no work
        while(!has_work)
        {
            RicCoreThread::block();
        }

        if (_storeState != STATE::NOMINAL){
            // remove has_work flag otherwise we will eat all cpu nom nom
            has_work = false;
            RicCoreThread::block();
            continue; // skip writing queues if store is not nominal
        }

        thread_lock.acquire(); // locks the queues container

        //iterate thru every queue in the queues container for each file, and process any pending writes

        for (auto& [file_desc, queue] : queues){

            bool pending_flush = false;
            bool error = false;

            file = nullptr; // Make sure we don't accidentally write to the wrong file
            //need to verify that the file still exists using the fd
            
            while (!queue.empty()) { // maybe instead of empty get the current count and process that 
                
                //take 'ownership' of the first append request and remove from the queue
                req = std::move(queue.pop());   
                //update the file pointer to the request file
                file = req->file;

                {
                    RicCoreThread::ScopedLock l(device_lock);
                    //call underlying write to file
                    try{
                        file->file_write(req->data);
                    } catch (WrappedFile::WriteException& e)
                    {
                        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Write error");
                        error = true;
                        _storeState = STATE::ERROR_WRITE;
                        break;
                    }
                }

                pending_flush = true;
            }

            if (pending_flush && !error) 
            { // file can't be nullptr here implicitly
            //process flush at the end to try and take advatnage of multi-block writes
            //only really works if the underlying storage system supports this
                RicCoreThread::ScopedLock l(device_lock); // get device lock to call flush
                try{
                    file->file_flush(); // on sd cards this will take the most time
                } catch(WrappedFile::FlushException& e)
                {
                    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Flush error");
                    error = true;
                    _storeState = STATE::ERROR_FLUSH;

                }
            }

            if (error)
            {
                RicCoreThread::ScopedLock l(device_lock);
                //first close file, then attempt to clear the queue
                //close file
                try{
                    file->_close();
                }
                catch(WrappedFile::CloseException& e)
                {
                    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("File close error");
                   _storeState = STATE::ERROR_CLOSE;
                }

                //clear all pending write requests. Note we cant erase the queue and release the fd here
                //as the main thread might be calling append, and it may be holding a reference to the
                //current queue, so the best we can do is clear it.
                
                queue.clear();
            }

        }
        //update semaphore that work is done
        has_work = false;
        //release store thread lock
        thread_lock.release();
        RicCoreThread::block();
        if (done) break;
    }
}

store_fd StoreBase::get_next_fd(size_t maxQueueSize) {
    //check if we can use a returned filedesc, otherwise generate a new file desc
    store_fd desc;

    if (returned_fileDesc.size())
    {
        desc = returned_fileDesc.front();
        returned_fileDesc.pop();
    }else
    {
        desc = ++file_desc;
    }

    RicCoreThread::ScopedLock sl(thread_lock);
    //consturct append channel in the queues container
    queues.emplace(std::piecewise_construct,
                    std::forward_as_tuple(desc),
                    std::forward_as_tuple(maxQueueSize));
    return desc;
}

void StoreBase::release_fd(store_fd file_desc,bool force) {
    // Make sure all pending writes have been written
    if (!queues.count(file_desc)){
        //return if file_desc no longer exists in the queues
        return;
    }
    thread_lock.acquire();
    
    // we want to be able to force clear the queue if the state is not nominal otherwise
    // we will have a deadlock as the flush task does not process anyhting on the queue when
    //the state is not nominal
    if (!force || _storeState == STATE::NOMINAL){
        while (!queues.at(file_desc).empty()) 
        {
            thread_lock.release();
            RicCoreThread::delay(1); // Give flush_task some time to acquire the lock
            thread_lock.acquire();
        }
    }

    queues.erase(file_desc);
    thread_lock.release();
    returned_fileDesc.push(file_desc);
}


std::string StoreBase::generateUniquePath(std::string_view filepath,std::string_view prefix)
{
    std::string uniquePath = std::string(filepath) + "/" + std::string(prefix);
    std::vector<directory_element_t> fileNames;
    //ensure directory exists
    mkdir(filepath);

    if (!ls(filepath,fileNames)){
        //idk why this would happen but some error has occured so we are reutnring an empty string
        //maybe throw an excpetion here?

        throw std::runtime_error("something when wrong when trying to ls, no clue what!");
    }

    // go away u a big poo
// rude
// <3
    size_t max_index = 0;


    for (directory_element_t elem : fileNames) {
        std::string_view filename = elem.name;

        if (filename.find(prefix) == std::string::npos)
        {
            //if filename does not contain the expected prefix, continue in for looop
            continue;
        }

        size_t filename_index = 0;

        try{
            filename_index = getFilepathIndex(filename);
        }
        catch (std::runtime_error &e)
        {
            //some behaviour if the index is too big - currently just throw
            throw e;

        }

        if (filename_index > max_index) {
            max_index = filename_index;
        }

    }
    
    return uniquePath + std::to_string(max_index + 1) ;

    //ls file path
        //iterate thru ls directory
        //get highest index
        //return 1 + 
}

size_t StoreBase::getFilepathIndex(std::string_view initialString)
{
    std::string indexString;
    for (auto c = initialString.rbegin(); c != initialString.rend(); ++c)
    {
        if (!std::isdigit(*c))
        {
            break;
        }
        indexString.insert(0,std::string{*c});
    }
    //throws out of range when stol is too large
    return indexString.length() ? stol(indexString) : 0;
};