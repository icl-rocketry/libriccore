#pragma once
/**
 * @brief Provides a millis stub function if we do not have millis()
 * 
 */

#ifndef ARDUINO
#include <chrono>
inline uint32_t millis(){
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return uint32_t(ms);
};
#endif