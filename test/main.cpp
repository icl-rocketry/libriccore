#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <memory>
#include <exception>

#include <libriccore/platform/thread.h>

#include <libriccore/storage/storebase.h>
#include <libriccore/storage/wrappedfile.h>


class UnixWrappedFile : public WrappedFile {
public:
    UnixWrappedFile(std::string path, FILE_MODE mode, StoreBase& store) : WrappedFile(store,mode) {
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

protected:
    void _read(std::vector<uint8_t>& dest) override{
        file.read((char*)dest.data(), dest.size());
    }
    
    void _close() override{
        file.close();
    }

private:
    std::fstream file;

    void file_write(const std::vector<uint8_t>& data) override{
        file.write((char*)data.data(), data.size());
    }
    void file_flush() override{
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
    std::vector<uint8_t> vec(str.begin(), str.end());
    bool done;

    file->append(vec, &done);

    while (!done) {
        std::cout << "waiting" << std::endl;
    }

    std::cout << "Done" << std::endl;
    return 0;
}
