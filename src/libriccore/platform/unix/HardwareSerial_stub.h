#pragma once

#include <cstdint>
#include <cstddef>


/**
 * @brief Abstract interface to emulate HardwareSerial class in arduino
 * 
 */
class HardwareSerial{
    public:
        virtual size_t availableForWrite() = 0;
        virtual void write(uint8_t* data,size_t size) = 0;
        virtual size_t available() = 0;
        virtual uint8_t read() = 0;

        virtual ~HardwareSerial(){};
};
