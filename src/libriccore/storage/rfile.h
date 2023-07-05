#pragma once

#include <vector>
#include <memory>
#include <initializer_list>

#include "storebase.h"
#include "storetype.h"
#include "wrappedfile.h"

// Replicated File, 1 file replicated on n stores
class RFile {
public:
    RFile(std::string path, std::initializer_list<StoreBase&> stores, FILE_MODE mode = FILE_MODE::RW) {
        for (auto s : stores) {
            auto file = s.open(path, mode);
            files.push_back(std::move(file));
        }
    }

    void append_async(const std::vector<char>& data) {
        for (auto& f : files) {
            f->append(data, nullptr);
        }
    }

    void append(const std::vector<char>& data) {
        std::vector<bool*> dones(files.size());

        for (int i = 0; i < files.size(); i++) {
            files[i]->append(data, dones.at(i));
        }

        bool all_done = false;
        while (!all_done) {
            all_done = true;
            for (auto done : dones) {
                all_done &= *done;
            }
        }
    }

    void read(std::vector<char>& dest) {
        files.at(0)->read(dest); //Always read the first file because it's got priority
    }

private:
    std::vector<std::unique_ptr<WrappedFile>> files;
};