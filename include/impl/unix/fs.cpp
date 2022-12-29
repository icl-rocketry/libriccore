#include <headers/fs.h>

#include <fstream>
#include <filesystem>
#include <exception>

File::File(std::string path, FILE_MODE mode) : cursor(0), file(FS::open(path, mode)), mode(mode) {}

void File::flush() {
    file.flush();
}

void File::write(const char* src, size_t size) {
    if (mode == FILE_MODE::READ) {
        throw std::runtime_error("Cannot write to a readonly file");
    }
    file.write(src, size);
}

void File::read(char* dest, size_t size) {
    if (mode == FILE_MODE::WRITE) {
        throw std::runtime_error("Cannot read from a writeonly file");
    }
    file.read(dest, size);
}

void File::seekg(size_t offset) {
    if (mode == FILE_MODE::WRITE) {
        throw std::runtime_error("Cannot seekg on a writeonly file");
    }
    file.seekg(offset);
}

void File::seekp(size_t offset) {
    if (mode == FILE_MODE::READ) {
        throw std::runtime_error("Cannot seekp on a readonly file");
    }
    file.seekp(offset);
}

void File::close() {
    file.close();
}

namespace FS {
    config::FileType open(std::string path, FILE_MODE mode) {
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
        std::fstream file(path, f_mode);
        return file;
    }

    bool ls(std::string path, std::vector<directory_element_t> &directory_structure) {
        if (!std::filesystem::exists(path)) return false;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            FILE_TYPE f_type = FILE_TYPE::FILE;
            if (entry.is_directory()) {
                f_type = FILE_TYPE::DIRECTORY;
            }

            directory_structure.push_back(
                {
                    entry.path().filename(),
                    entry.file_size(),
                    f_type
                }
            );
        }
        return true;
    }

    bool ls(std::vector<directory_element_t> &directory_structure) {
        return ls(".", directory_structure);
    }

    bool mkdir(std::string path) {
        return std::filesystem::create_directory(path);
    }

    bool remove(std::string path) {
        return std::filesystem::remove(path);
    }
}
