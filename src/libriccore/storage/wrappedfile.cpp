#include "wrappedfile.h"


#include "storebase.h"


WrappedFile::WrappedFile(StoreBase& store, FILE_MODE mode) : mode(mode), store(store), file_desc(store.get_next_fd()) {}

WrappedFile::~WrappedFile(){};

void WrappedFile::append(const std::vector<char>& data, bool* done) {
    if (mode == FILE_MODE::READ){
        throw std::runtime_error("Cannot write to readonly file!");
    }
    AppendRequest req{&data, this, done};
    store.append(*this,  req);
}

void WrappedFile::read(std::vector<char>& dest) {
    if (mode == FILE_MODE::WRITE) {
        throw std::runtime_error("Cannot read from a writeonly file!");
    }
    ScopedLock sl(store.get_lock());
    _read(dest);
}

void WrappedFile::close() {
    store.release_fd(file_desc);
    
    ScopedLock sl(store.get_lock());
    _close();
}
