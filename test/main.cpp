#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <memory>
#include <headers/thread.h>
#include "store/store_base.h"
#include <exception>

class UnixWrappedFile : public WrappedFile {
public:
    UnixWrappedFile(std::string path, FILE_MODE mode, StoreBase& store) : WrappedFile(mode, store) {
        auto f_mode = std::fstream::in;
        switch (mode) {
        case FILE_MODE::READ:
            break;
        case FILE_MODE::WRITE:
            f_mode = std::fstream::out;
            break;
        case FILE_MODE::RW:
            f_mode |= std::fstream::out;
            break;
        };
        file = std::fstream(path, f_mode);
    }

    void read(std::vector<char>& dest) {
        if (mode == FILE_MODE::WRITE) {
            throw std::runtime_error("Cannot read from a writeonly file");
        }
        ScopedLock sl(store.get_lock());
        file.read(dest.data(), dest.size());
    }
    
    void close() {
        ScopedLock sl(store.get_lock());
        file.close();
    }

private:
    std::fstream file;

    virtual void file_write(const std::vector<char>& data) {
        file.write(data.data(), data.size());
    }
    virtual void file_flush() {
        file.flush();
    }
};

class UnixStore : public StoreBase {
public:
    UnixStore(Lock& device_lock) : StoreBase(device_lock) {}

protected:
    std::unique_ptr<WrappedFile> _open(std::string path, FILE_MODE mode) {
        return std::make_unique<UnixWrappedFile>(path, mode, *this);
    }

    bool _ls(std::string path, std::vector<directory_element_t> &directory_structure) {
        if (!std::filesystem::exists(path)) return false;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            FILE_TYPE f_type = FILE_TYPE::FILE;
            if (entry.is_directory()) {
                f_type = FILE_TYPE::DIRECTORY;
            }

            directory_structure.push_back(
                {
                    entry.path().filename(),
                    static_cast<uint32_t>(entry.file_size()),
                    f_type
                }
            );
        }
        return true;
    }
    bool _mkdir(std::string path) {
        return std::filesystem::create_directory(path);
    }

    // Removes a file or an empty directory
    bool _remove(std::string path) {
        return std::filesystem::remove(path);
    }
};


int main() {
    Lock l;
    UnixStore s(l);
    
    if (s.mkdir("./tmp/")) {
        std::cout << "Created tmp dir" << std::endl;
    }

    std::vector<directory_element_t> files;
    s.ls("./tmp/", files);
    
    std::cout << "Found files:" << std::endl;
    for (auto& file : files) {
        std::cout << file.name << std::endl;
    }

    std::cout << std::endl;

    auto file = s.open("./tmp/asdf.txt", FILE_MODE::WRITE);

    std::string str = "hello world\n";
    std::vector<char> vec(str.begin(), str.end());
    bool done;

    file->append(vec, &done);

    while (!done) {
        std::cout << "waiting" << std::endl;
    }

    std::cout << "Done" << std::endl;
    return 0;
}
