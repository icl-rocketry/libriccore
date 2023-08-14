#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "storetypes.h"
#include "wrappedfile.h"
#include "appendrequest.h"

#include <libriccore/platform/riccorethread.h>



enum class FILE_TYPE : uint8_t {
    FILE,
    DIRECTORY
};

struct directory_element_t{
    std::string name;
    uint32_t size;
    FILE_TYPE type;
};


class StoreBase {
public:
    StoreBase(RicCoreThread::Lock &device_lock);

    ~StoreBase();

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    /**
     * @brief Submit a append reques, throws an std::out_of_range if the file_desc is not in the queues map
     * 
     * @param request_ptr 
     */
    void append(std::unique_ptr<AppendRequest> request_ptr);

    /**
     * @brief Get the underlying device lock
     * 
     * @return RicCoreThread::Lock& 
     */
    RicCoreThread::Lock& get_lock() {
        return device_lock;
    }

    /**
     * @brief Returns the state of the has_work semaphore indicating whether there are curretnyl queued append requests.
     * 
     * @return true 
     * @return false 
     */
    bool pendingWrites()
    {
        return has_work.get();
    }

    /**
     * @brief Open a new file, returns a wrapped file
     * 
     * @param path 
     * @param mode 
     * @return std::unique_ptr<WrappedFile> 
     */
    std::unique_ptr<WrappedFile> open(std::string path, FILE_MODE mode = FILE_MODE::RW);

    /**
     * @brief List the current directory, returning a vector of directory_elements
     * 
     * @param path 
     * @param directory_structure Currently a vector of directory_elemets
     * @return true 
     * @return false 
     */
    bool ls(std::string path, std::vector<directory_element_t> &directory_structure);

    /**
     * @brief Make new directory
     * 
     * @param path 
     * @return true 
     * @return false 
     */
    bool mkdir(std::string path);

    /**
     * @brief Removes file or directory specified by path
     * 
     * @param path 
     * @return true 
     * @return false 
     */
    bool remove(std::string path);

    /**
     * @brief Allocate a new file descriptor and generate an append request channel
     * 
     * @return store_fd 
     */
    store_fd get_next_fd();

    /**
     * @brief Release the file descriptor, this function blocks until all writes are complete
     * Maybe providea force option which kills the queue and discards any pending writes
     * 
     * @param file_desc 
     * @param force force erase queue, warning potential data loss
     */
    void release_fd(store_fd file_desc,bool force);

protected:
    /**
     * @brief This is a reference to another lock in case several devices share a bus
     * Use this when performing operations on the device e.g. file write
     * 
     */
    RicCoreThread::Lock& device_lock;

    /**
     * @brief Use this when updating shared internal state e.g. the queues map
     * 
     */
    RicCoreThread::Lock thread_lock; 

private:
    virtual std::unique_ptr<WrappedFile> _open(std::string path, FILE_MODE mode) = 0;
    virtual bool _ls(std::string path, std::vector<directory_element_t> &directory_structure) = 0;
    virtual bool _mkdir(std::string path) = 0;
    virtual bool _remove(std::string path) = 0; // Removes a file or an empty directory

    /**
     * @brief map of queues to flie descriptors
     * 
     */
    std::unordered_map<store_fd, RicCoreThread::UniquePtrChannel<AppendRequest>> queues; 

    void flush_task(void* args);
    RicCoreThread::Thread t;
    RicCoreThread::ThreadWorkSemaphore has_work;
    store_fd file_desc;
    std::atomic<bool> done;
};