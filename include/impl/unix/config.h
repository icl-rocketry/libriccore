#pragma once
#include <thread>
#include <mutex>
#include <fstream>

// This file is used to specify base types for general headers
namespace config {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;
    using FileType = std::fstream;
}