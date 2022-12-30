#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <thread.h>


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

class StoreBase;

struct AppendRequest;

class WrappedFile {
public:
    WrappedFile(FILE_MODE mode, StoreBase& store) : cursor(0), store(store), mode(mode) {}

    // Send a request to the underlying store to store this
    void append(const char* data, size_t size, bool* done) {
        AppendRequest req{data, done, size};
        store.append(*this,  req);
    }

    virtual void read(char* dest, size_t size);
    virtual void close(); // Should this be done automatically on delete?

private:
    size_t cursor;
    StoreBase& store;
    FILE_MODE mode;

    //Write/Flush to the underlying file type
    virtual void file_write(const char* data, size_t size);
    virtual void file_flush();

friend StoreBase;
};


struct AppendRequest {
    const char* data;
    bool* done;
    size_t size;
};

class StoreBase {
public:
    StoreBase(std::string name, Lock& device_lock) : 
    t((void (*)(void* args)) &StoreBase::flush_task, nullptr), device_lock(device_lock), queues({}) {}

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls("/", directory_structure);
    }

    void append(WrappedFile& file, AppendRequest r) {
        queues.at(file).send(r);
        remaining_work.up(); //Must have sequential consistency (guarantee that the up happens after the channel send)
    }

    virtual WrappedFile open(std::string path, FILE_MODE mode);
    virtual bool ls(std::string path, std::vector<directory_element_t> &directory_structure);
    virtual bool mkdir(std::string path);
    virtual bool remove(std::string path); // Removes a file or an empty directory

    Lock& device_lock;

private:
    void flush_task(void* args) {
        AppendRequest req;
        while (true) {
            for (auto& [file, queue] : queues) {
                bool pending_flush = false;
                
                while (!queue.empty()) {
                    remaining_work.down();
                    queue.receive(req);
                    
                    {
                        ScopedLock l(device_lock);
                        file.file_write(req.data, req.size);
                    }
                    if (req.done != nullptr) {
                        *req.done = true;
                    }
                    pending_flush = true;
                }

                if (pending_flush) {
                    file.file_flush();
                }
            }
        }
    }

    Thread t;
    Semaphore remaining_work;
    std::unordered_map<WrappedFile&, Channel<AppendRequest>> queues;
};
