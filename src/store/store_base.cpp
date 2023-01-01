#include "store_base.h"
#include <iostream>

void WrappedFile::append(const std::vector<char>& data, bool* done) {
    AppendRequest req{&data, done};
    store.append(*this,  req);
}

void StoreBase::append(WrappedFile& file, AppendRequest r) {
    intptr_t idx = reinterpret_cast<intptr_t>(&file);
    queues.at(idx).send(r);
    has_work.up(); //Must have sequential consistency (guarantee that the up happens after the channel send)
}

void StoreBase::flush_task(void* args) {
    AppendRequest req;
    
    while (true) {
        has_work.down();
        for (auto& [idx, queue] : queues) {
            WrappedFile* file = (WrappedFile*) idx;
            bool pending_flush = false;
            
            while (!queue.empty()) {
                queue.receive(req);
                
                {
                    ScopedLock l(device_lock);
                    file->file_write(*req.data);
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
        if (done) break;
    }
}
