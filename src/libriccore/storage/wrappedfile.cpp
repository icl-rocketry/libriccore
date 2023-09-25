#include "wrappedfile.h"


#include "storebase.h"
#include "appendrequest.h"

#include <memory>

WrappedFile::WrappedFile(StoreBase &store, store_fd fileDesc, FILE_MODE mode,size_t maxQueueSize) : 
mode(mode), 
store(store), 
file_desc(fileDesc),
_closed(false)
{}

WrappedFile::~WrappedFile()
{
    _closed=true;
    store.release_fd(file_desc, true);
};

void WrappedFile::append(std::vector<uint8_t>& data) {
    if (static_cast<uint16_t>(mode) & static_cast<uint16_t>(FILE_MODE::READ)){
        throw std::runtime_error("Cannot write to readonly file!");
    } 
    if (_closed)
    {
        throw WrappedFile::CloseException();
    }

    store.append(std::make_unique<AppendRequest>(std::move(data),this));
}

void WrappedFile::appendCopy(const std::vector<uint8_t> &data)
{
    std::vector<uint8_t> data_copy(data);
    append(data_copy);
}

void WrappedFile::read(std::vector<uint8_t>& dest) {
    if (static_cast<uint16_t>(mode) & static_cast<uint16_t>(FILE_MODE::WRITE)) {
        throw std::runtime_error("Cannot read from a writeonly file!");
    }
    if (_closed)
    {
        throw WrappedFile::CloseException();
    }
    RicCoreThread::ScopedLock sl(store.get_lock());
    _read(dest);
}

void WrappedFile::close(bool force) {
    store.release_fd(file_desc,force);
    _closed = true;
    
    RicCoreThread::ScopedLock sl(store.get_lock());
    _close();

}
