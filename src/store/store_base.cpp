#include "store_base.h"
#include <iostream>

WrappedFile::WrappedFile(FILE_MODE mode, StoreBase& store) : mode(mode), store(store), file_desc(store.get_next_fd()) {}

void WrappedFile::append(const std::vector<char>& data, bool* done) {
    AppendRequest req{&data, this, done};
    store.append(*this,  req);
}

void WrappedFile::read(std::vector<char>& dest) {
    if (mode == FILE_MODE::WRITE) {
        throw std::runtime_error("Cannot read from a writeonly file");
    }
    ScopedLock sl(store.get_lock());
    _read(dest);
}

void WrappedFile::close() {
    store.release_fd(file_desc);
    
    ScopedLock sl(store.get_lock());
    _close();
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

void StoreBase::append(WrappedFile& file, AppendRequest r) {
    queues.at(file.file_desc).send(r);
    has_work.up(); //Must have sequential consistency (guarantee that the up happens after the channel send)
}

void StoreBase::flush_task(void* args) {
    AppendRequest req;
    WrappedFile* file;
    
    while (true) {
        has_work.down();
        thread_lock.acquire();
        for (auto& [file_desc, queue] : queues) {
            bool pending_flush = false;
            file = nullptr; // Make sure we don't accidentally write to the wrong file
            
            while (!queue.empty()) {
                queue.receive(req);
                file = req.file;
                {
                    ScopedLock l(device_lock);
                    file->file_write(*req.data);
                }
                if (req.done != nullptr) {
                    *req.done = true;
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

fd StoreBase::get_next_fd() {
    fd desc = file_desc++;

    ScopedLock sl(thread_lock);
    queues.emplace(std::piecewise_construct,
                    std::forward_as_tuple(desc),
                    std::forward_as_tuple());
    return desc;
}

void StoreBase::release_fd(fd file_desc) {
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