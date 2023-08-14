#include "wrappedfile.h"


#include "storebase.h"
#include "appendrequest.h"

#include <memory>


WrappedFile::WrappedFile(StoreBase& store, FILE_MODE mode) : mode(mode), store(store), file_desc(store.get_next_fd()) {}

WrappedFile::~WrappedFile()
{
    WrappedFile::close(true);
};

void WrappedFile::append(std::vector<uint8_t>& data) {
    if (mode == FILE_MODE::READ){
        throw std::runtime_error("Cannot write to readonly file!");
    } 

    store.append(std::make_unique<AppendRequest>(std::move(data),this));
}

void WrappedFile::appendCopy(const std::vector<uint8_t> &data)
{
    std::vector<uint8_t> data_copy(data);
    append(data_copy);
}

void WrappedFile::read(std::vector<uint8_t>& dest) {
    if (mode == FILE_MODE::WRITE) {
        throw std::runtime_error("Cannot read from a writeonly file!");
    }
    RicCoreThread::ScopedLock sl(store.get_lock());
    _read(dest);
}

void WrappedFile::close(bool force) {
    store.release_fd(file_desc,force);
    
    RicCoreThread::ScopedLock sl(store.get_lock());
    _close();
}
