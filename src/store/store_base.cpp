#include "store_base.h"
#include <iostream>

void WrappedFile::append(const char* data, size_t size, bool* done) {
    AppendRequest req{data, done, size};
    store.append(*this,  req);
}

void StoreBase::append(WrappedFile& file, AppendRequest r) {
    intptr_t idx = (intptr_t) (&file);
    queues.at(idx).send(r);
    has_work.up(); //Must have sequential consistency (guarantee that the up happens after the channel send)
}

void StoreBase::flush_task(void* args) {
    AppendRequest req;
    
    while (true) {
        has_work.down();
        if (done) break;
        
        for (auto& [idx, queue] : queues) {
            WrappedFile* file = (WrappedFile*) idx;
            bool pending_flush = false;
            
            while (!queue.empty()) {
                queue.receive(req);
                
                {
                    ScopedLock l(device_lock);
                    file->file_write(req.data, req.size);
                }
                if (req.done != nullptr) {
                    *req.done = true;
                }
                pending_flush = true;
            }

            if (pending_flush) {
                file->file_flush();
            }
        }
        std::cout << "looping" << std::endl;
    }
}
