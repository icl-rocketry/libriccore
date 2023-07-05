#pragma once

#include <stdint.h>

/**
 * @brief Type of file descriptor
 * 
 */
using store_fd = uint32_t;
 
/**
 * @brief 
 * TODO -> impelment POSIX file flags
 * 
 */
enum class FILE_MODE : uint8_t {
    READ,
    WRITE,
    RW
};