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
    const std::vector<char>* data; //Must be nullable
    bool* done;
};

class StoreBase;
class WrappedFile {
public:
    WrappedFile(FILE_MODE mode, StoreBase& store) : mode(mode), store(store) {}

    // Send a request to the underlying store to store this
    void append(const std::vector<char>& data, bool* done);

    void read(std::vector<char>& dest);
    void close(); // Should this be done automatically on delete?

protected:
    FILE_MODE mode;
    StoreBase& store;

private:
    virtual void _read(std::vector<char>& dest) = 0;
    virtual void _close() = 0;

    //Write/Flush to the underlying file type
    virtual void file_write(const std::vector<char>& data) = 0;
    virtual void file_flush() = 0;

friend StoreBase;
};


class StoreBase {
public:
    StoreBase(Lock& device_lock) : device_lock(device_lock), t((void (*)(void* args)) &StoreBase::flush_task, (void*) this), done(false) {}

    ~StoreBase() {
        done = true;
        has_work.up(); // Just incase the other thread is sleeping
    }

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    void append(WrappedFile& file, AppendRequest r);

    Lock& get_lock() {
        return device_lock;
    }

    std::unique_ptr<WrappedFile> open(std::string path, FILE_MODE mode);
    bool ls(std::string path, std::vector<directory_element_t> &directory_structure);
    bool mkdir(std::string path);
    bool remove(std::string path); // Removes a file or an empty directory

protected:
    Lock& device_lock;

    std::unordered_map<intptr_t, Channel<AppendRequest>> queues;

private:
    virtual std::unique_ptr<WrappedFile> _open(std::string path, FILE_MODE mode) = 0;
    virtual bool _ls(std::string path, std::vector<directory_element_t> &directory_structure) = 0;
    virtual bool _mkdir(std::string path) = 0;
    virtual bool _remove(std::string path) = 0; // Removes a file or an empty directory

    void flush_task(void*);
    Thread t;
    Semaphore has_work;
    bool done;
};