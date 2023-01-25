// #include <iostream>
// #include <fstream>
// #include <filesystem>
// #include <utility>
// #include <memory>
// #include <headers/thread.h>
// #include <libriccore/store/store_base.h>
// #include <exception>

// int fib(int n, int a, int b) {
//     if (n == 0) {
//         return a;
//     }
//     return fib(n - 1, a + b, a);
// }

// int fib(int n) {
//     return fib(n, 0, 1);
// }

// void thread2(void* args) {
//     Lock* l = (Lock*) args;
//     int x = fib(32);
//     std::cout << x << std::endl;
//     l->release(); 
// }

// class UnixWrappedFile : public WrappedFile {
// public:
//     UnixWrappedFile(std::string path, FILE_MODE mode, StoreBase& store) : WrappedFile(mode, store) {
//         auto f_mode = std::fstream::in;
//         switch (mode) {
//         case FILE_MODE::READ:
//             break;
//         case FILE_MODE::WRITE:
//             f_mode = std::fstream::out;
//             break;
//         case FILE_MODE::RW:
//             f_mode |= std::fstream::out;
//             break;
//         };
//         file = std::fstream(path, f_mode);
//     }

//     void read(char* dest, size_t size) {
//         if (mode == FILE_MODE::WRITE) {
//             throw std::runtime_error("Cannot read from a writeonly file");
//         }
//         file.read(dest, size);
//     }
    
//     void close() {
//         file.close();
//     }

// private:
//     std::fstream file;

//     virtual void file_write(const char* data, size_t size) {
//         file.write(data, size);
//     }
//     virtual void file_flush() {
//         file.flush();
//     }
// };

// class UnixStore : public StoreBase {
// public:
//     UnixStore(Lock& device_lock) : StoreBase(device_lock) {}

//     std::unique_ptr<WrappedFile> open(std::string path, FILE_MODE mode) {
//         auto file = std::make_unique<UnixWrappedFile>(path, mode, *this);
//         intptr_t idx = (intptr_t) (file.get());
//         queues.emplace(std::piecewise_construct,
//                        std::forward_as_tuple(idx),
//                        std::forward_as_tuple());
//         return file;
//     }

//     bool ls(std::string path, std::vector<directory_element_t> &directory_structure) {
//         if (!std::filesystem::exists(path)) return false;

//         for (const auto& entry : std::filesystem::directory_iterator(path)) {
//             FILE_TYPE f_type = FILE_TYPE::FILE;
//             if (entry.is_directory()) {
//                 f_type = FILE_TYPE::DIRECTORY;
//             }

//             directory_structure.push_back(
//                 {
//                     entry.path().filename(),
//                     static_cast<uint32_t>(entry.file_size()),
//                     f_type
//                 }
//             );
//         }
//         return true;
//     }
//     bool mkdir(std::string path) {
//         return std::filesystem::create_directory(path);
//     }

//     // Removes a file or an empty directory
//     bool remove(std::string path) {
//         return std::filesystem::remove(path);
//     }
// };


// int main() {
//     Lock l;
//     UnixStore s(l);
    
//     if (s.mkdir("./tmp/")) {
//         std::cout << "Created tmp dir" << std::endl;
//     }

//     std::vector<directory_element_t> files;
//     s.ls("./tmp/", files);
    
//     std::cout << "Found files:" << std::endl;
//     for (auto& file : files) {
//         std::cout << file.name << std::endl;
//     }

//     std::cout << std::endl;

//     auto file = s.open("./tmp/asdf.txt", FILE_MODE::WRITE);

//     std::string str = "hello world\n";
//     bool done;

//     file->append(str.c_str(), str.size(), &done);

//     while (!done) {
//         std::cout << "waiting" << std::endl;
//     }

//     std::cout << "Done" << std::endl;
//     return 0;
// }
#include <iostream>

#include "riccorelogging.h"



int main() {
    RicCoreLogging::log<LOGGER::SYS>("test log");
   
}