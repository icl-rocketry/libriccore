#pragma once
#include <chrono>

inline uint32_t millis(){
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return uint32_t(ms);
};