/**
 * @file systemflags_default.h
 * @author kiran de silva
 * @brief Default system flags which should always exist in a user defined system flag enum.
 * NOTE the values used in this enum were taken from Ricardo-OS, however these values can be changed if 
 * you have a new flags defintion. Just make sure if is documented and nay parsing on the backend you do
 * is aslo updated.
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <cstdint>
namespace RicCoreSystemFlags_Default{
    enum class SYSTEM_FLAGS:uint32_t{
        DEBUG = (1 << 7),
        ERROR_SPI = (1 << 8),
        ERROR_I2C = (1 << 9),
        ERROR_SERIAL = (1 << 10),
        ERROR_STORAGE = (1 << 19),
    };
};