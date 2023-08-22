#pragma once

#include <stdint.h>

/**
 * @brief Type of file descriptor
 * 
 */
using store_fd = uint32_t;
 
/**
 * @brief 
 * 
 * 
 */
enum class FILE_MODE : uint8_t {
    READ = 0x00,
    WRITE = 0x01,
    RW = 0x02,
    AT_END = 0x04,
    APPEND = 0x08,
    CREATE = 0x10,
    TRUNCATE = 0x20    
};