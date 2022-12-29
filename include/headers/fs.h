#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <config.h>


enum class FILE_TYPE : uint8_t {
    FILE,
    DIRECTORY
};

enum class FILE_MODE : uint8_t {
    READ,
    WRITE,
    RW
};

struct directory_element_t{
    std::string name;
    uint32_t size;
    FILE_TYPE type;
};

namespace FS {
    config::FileType open(std::string path, FILE_MODE mode);
    bool ls(std::string path, std::vector<directory_element_t> &directory_structure);
    bool ls(std::vector<directory_element_t> &directory_structure); // ls root
    bool mkdir(std::string path);
    bool remove(std::string path); // Removes a file or an empty directory
}

class File {
public:
    File(std::string path, FILE_MODE mode);

    void flush();
    void read(char* dest, size_t size);
    void write(const char* src, size_t size);
    void seekg(size_t offset); // For reads
    void seekp(size_t offset); // For writes
    void close(); // Should this be done automatically on delete?

private:
    size_t cursor;
    config::FileType file;
    FILE_MODE mode;
};