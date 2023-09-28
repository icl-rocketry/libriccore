#pragma once

#include "HardwareSerial_stub.h"

#include <cstdint>
#include <cstddef>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <termios.h>

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
            ptsDeviceName = ptsname(fileDescriptor);

            // struct termios tty;
            // if(tcgetattr(fileDescriptor, &tty) != 0) {
            //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Error from tcgetattr");
            // }
            // //set baud
            // cfsetispeed(&tty, baud);
            // cfsetospeed(&tty, baud);   

            // if (tcsetattr(fileDescriptor, TCSANOW, &tty) != 0) {
            //     RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Error from tcsetattr");
            // }
            // ioctl(fileDescriptor,TIOCMBIS,TIOCM_RTS);
            // ioctl(fileDescriptor,TIOCMBIS,TIOCM_DTR);

            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Virtual serial port opened on " + ptsDeviceName);
        };

        size_t availableForWrite() override 
        {
            struct stat filestat; //ew c 
            stat(ptsDeviceName.c_str(),&filestat);
            size_t ptsFileSize = filestat.st_size;
            return fileSizeLimit-ptsFileSize;

        };

        void write(uint8_t *data, size_t size) override 
        {
            ::write(fileDescriptor,reinterpret_cast<void*>(data),size);
        };
        size_t available() override 
        {
            int numBytes;
            ioctl(fileDescriptor,FIONREAD,&numBytes);
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("VSP - " + std::to_string(numBytes));
            return (numBytes < 0) ? 0 : numBytes;
        };
        uint8_t read() override 
        {
            uint8_t byte;
            ::read(fileDescriptor,reinterpret_cast<void*>(&byte),1);
            return byte;
        };

        ~VirtualSerialPort()
        {
            close(fileDescriptor);
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Virtual serial port closed on " + ptsDeviceName);

        };

    private:
        int fileDescriptor;
        std::string ptsDeviceName;
        static constexpr int baud = 115200;

        /**
         * @brief Hard limit of 1MB for file size for now
         * 
         */
        static constexpr size_t fileSizeLimit = 1e6; 
};