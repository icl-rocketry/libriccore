#pragma once
#include <thread>
#include <mutex>

// This file is used to specify base types for general headers

namespace config {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;
}
