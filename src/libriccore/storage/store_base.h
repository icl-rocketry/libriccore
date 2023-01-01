#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include <headers/thread.h>


enum class FILE_TYPE : uint8_t {
    FILE,
    DIRECTORY
};

enum class FILE_MODE : uint8_t {
    READ,
    WRITE,
    RW
};

struct directory_element_t{
    std::string name;
    uint32_t size;
    FILE_TYPE type;
};

struct AppendRequest {
    const char* data;
    bool* done;
    size_t size;
};

class StoreBase;
class WrappedFile {
public:
    WrappedFile(FILE_MODE mode, StoreBase& store) : mode(mode), store(store) {}

    // Send a request to the underlying store to store this
    void append(const char* data, size_t size, bool* done);

    virtual void read(char* dest, size_t size) = 0;
    virtual void close() = 0; // Should this be done automatically on delete?

protected:
    FILE_MODE mode;

private:
    StoreBase& store;

    //Write/Flush to the underlying file type
    virtual void file_write(const char* data, size_t size) = 0;
    virtual void file_flush() = 0;

friend StoreBase;
};


class StoreBase {
public:
    StoreBase(Lock& device_lock) : device_lock(device_lock), t((void (*)(void* args)) &StoreBase::flush_task, (void*) this) {}

    ~StoreBase() {
        done = true;
        has_work.up(); // Just incase the other thread is sleeping
    }

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    void append(WrappedFile& file, AppendRequest r);

    virtual std::unique_ptr<WrappedFile> open(std::string path, FILE_MODE mode) = 0;
    virtual bool ls(std::string path, std::vector<directory_element_t> &directory_structure) = 0;
    virtual bool mkdir(std::string path) = 0;
    virtual bool remove(std::string path) = 0; // Removes a file or an empty directory

    Lock& device_lock;

protected:
    std::unordered_map<intptr_t, Channel<AppendRequest>> queues;

private:
    void flush_task(void*);
    Thread t;
    Semaphore has_work;
    bool done;
};