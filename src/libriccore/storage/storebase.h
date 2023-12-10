#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <queue>

#include "storetypes.h"
#include "wrappedfile.h"
#include "appendrequest.h"

#include <libriccore/threading/riccorethread.h>
#include <libriccore/threading/uniqueptrchannel.h>



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
    StoreBase(RicCoreThread::Lock_t &device_lock);

    ~StoreBase();

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    /**
     * @brief Submit a append reques, throws an std::out_of_range if the file_desc is not in the queues map
     * Not threadsafe if called in a different thread to get_next_fd or release_fd. Threadsafe when called in a different thread
     * to flush_task. Can be made fully threadsafe if a second lock is added to prevent modification of the corresponding fd in the map
     * while append is being called, however on the esp32 this will enver happen as only flush_task is run on a second core.
     * 
     * @param request_ptr 
     */
    bool append(std::unique_ptr<AppendRequest> request_ptr);

    /**
     * @brief Get the underlying device lock
     * 
     * @return RicCoreThread::Lock_t& 
     */
    RicCoreThread::Lock_t& get_lock() {
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
        return has_work.load();
    }

    /**
     * @brief Open a new file, returns a wrapped file, returns nullptr on error
     * 
     * @param path 
     * @param mode 
     * @return std::unique_ptr<WrappedFile> 
     */
    std::unique_ptr<WrappedFile> open(std::string_view path, FILE_MODE mode = FILE_MODE::RW,size_t maxQueueSize = 10);

    /**
     * @brief List the current directory, returning a vector of directory_elements
     * 
     * @param path 
     * @param directory_structure Currently a vector of directory_elemets
     * @return true success
     * @return false error
     */
    bool ls(std::string_view path, std::vector<directory_element_t> &directory_structure);

    /**
     * @brief Make new directory
     * 
     * @param path 
     * @return true 
     * @return false 
     */
    bool mkdir(std::string_view path);

    /**
     * @brief Finds the lowest numbered instance of the given file prefix and generates a unique
     * filepath by incrementing the integer at the end of the path_prefix. 
     * 
     * @param path_prefix 
     * @return std::string 
     */
    std::string generateUniquePath(std::string_view filepath,std::string_view prefix="");
    
    /**
     * @brief Removes file or directory specified by path
     * 
     * @param path 
     * @return true 
     * @return false 
     */
    bool remove(std::string_view path);

    /**
     * @brief Allocate a new file descriptor and generate an append request channel
     * 
     * @param maxQueueSize maximum number of append requests to be queued for given file descriptor default is 5
     * @return store_fd 
     */
    store_fd get_next_fd(size_t maxQueueSize = 10);

    /**
     * @brief Release the file descriptor, this function blocks until all writes are complete
     * Maybe providea force option which kills the queue and discards any pending writes
     * 
     * @param file_desc 
     * @param force force erase queue, warning potential data loss
     */
    void release_fd(store_fd file_desc,bool force);


    enum class STATE:uint8_t{
            NOMINAL,
            ERROR_SETUP,
            ERROR_CLOSE,
            ERROR_WRITE,
            ERROR_FLUSH
        };

    STATE getState(){return _storeState;};

protected:
    /**
     * @brief This is a reference to another lock in case several devices share a bus
     * Use this when performing operations on the device e.g. file write
     * 
     */
    RicCoreThread::Lock_t& device_lock;

    /**
     * @brief Use this when updating shared internal state e.g. the queues map
     * 
     */
    RicCoreThread::Lock_t thread_lock; 

    
    std::atomic<STATE> _storeState;


    /**
     * @brief Returns the suffix integer of the given filepath. Throws out of range if the number is too large
     * 
     * @param intialString 
     * @return size_t 
     */
    size_t getFilepathIndex(std::string_view intialString);

    

private:
    virtual std::unique_ptr<WrappedFile> _open(std::string_view path,store_fd fileDesc, FILE_MODE mode,size_t maxQueueSize) = 0;
    virtual bool _ls(std::string_view path, std::vector<directory_element_t> &directory_structure) = 0;
    virtual bool _mkdir(std::string_view path) = 0;
    virtual bool _remove(std::string_view path) = 0; // Removes a file or an empty directory

    /**
     * @brief map of queues to file descriptors
     * 
     */
    std::unordered_map<store_fd, RicCoreThread::UniquePtrChannel<AppendRequest>> queues; 

    void flush_task(void* args);
    RicCoreThread::Thread flush_thread;
    std::atomic<bool> has_work;

    store_fd file_desc;
    std::queue<store_fd> returned_fileDesc;
    
    std::atomic<bool> done;


    
};