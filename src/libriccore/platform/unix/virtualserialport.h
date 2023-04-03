#pragma once

#include "HardwareSerial_stub.h"

#include <cstdint>
#include <cstddef>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>
#include <string>

#include <libriccore/riccorelogging.h>


class VirtualSerialPort : public HardwareSerial
{
    public:
        VirtualSerialPort():
        HardwareSerial()
        {
            fileDescriptor = posix_openpt(O_RDWR | O_NOCTTY);
            if (grantpt(fileDescriptor))
            {
                throw std::runtime_error("failed to get permission");
            }
            if (unlockpt(fileDescriptor))
            {
                throw std::runtime_error("failed to unlock pt");
            }
            deviceName = ptsname(fileDescriptor);

            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Virtual serial port opened on " + deviceName);
        };

        size_t availableForWrite() override {return 1;};
        void write(uint8_t *data, size_t size) override {};
        bool available() override {return true;};
        uint8_t read() override {return 2;};

        ~VirtualSerialPort()
        {
            close(fileDescriptor);
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Virtual serial port closed on " + deviceName);

        };

    private:
        int fileDescriptor;
        std::string deviceName;
};