#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "storetypes.h"
#include "wrappedfile.h"
#include "appendrequest.h"

#include <libriccore/platform/thread.h>



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
    StoreBase(Lock &device_lock);

    ~StoreBase();

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    void append(std::unique_ptr<AppendRequest> request_ptr);

    Lock& get_lock() {
        return device_lock;
    }

    std::unique_ptr<WrappedFile> open(std::string path, FILE_MODE mode = FILE_MODE::RW);
    bool ls(std::string path, std::vector<directory_element_t> &directory_structure);
    bool mkdir(std::string path);
    bool remove(std::string path); // Removes a file or an empty directory

    store_fd get_next_fd();
    void release_fd(store_fd file_desc);

protected:
    // This is a reference to another lock in case several devices share a bus
    Lock& device_lock; // Use this when performing operations on the device e.g. file write

    Lock thread_lock; // Use this when updating shared internal state e.g. the queues map

private:
    virtual std::unique_ptr<WrappedFile> _open(std::string path, FILE_MODE mode) = 0;
    virtual bool _ls(std::string path, std::vector<directory_element_t> &directory_structure) = 0;
    virtual bool _mkdir(std::string path) = 0;
    virtual bool _remove(std::string path) = 0; // Removes a file or an empty directory

    std::unordered_map<store_fd, ThreadTypes::UniquePtrChannel<AppendRequest>> queues;

    void flush_task(void* args);
    Thread t;
    ThreadTypes::ThreadWorkSemaphore has_work;
    store_fd file_desc;
    std::atomic<bool> done;
};