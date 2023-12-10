#pragma once

#include <stdint.h>

/**
 * @brief Type of file descriptor
 * 
 */
using store_fd = uint8_t;
 
/**
 * @brief file mode values, copied directly from the sdfat impelmentation for ease,
 * probbaly a good idea to come back to this later 
 * 
 * 
 */
enum class FILE_MODE : int {
    READ = 0x00,
    WRITE = 0x01,
    RW = 0x02,
    APPEND = 0x0008,
    CREATE = 0x0200,
    TRUNCATE = 0x0400,
    AT_END = 0x4000
};