#pragma once

#include "rnp_interface.h"

#include <memory>
#include <vector>
#include <array>
#include <string>

#include <HardwareSerial.h>

struct UARTInterfaceInfo:public RnpInterfaceInfo{
    size_t sendBufferSize;
    bool sendBufferOverflow;
    size_t receiveBufferSize;
    bool receiveBufferOverflow;
};

class UART : public RnpInterface
{

public:
    UART(HardwareSerial &serial, SystemStatus &systemstatus, LogController &logcontroller, uint8_t id = static_cast<uint8_t>(DEFAULT_INTERFACES::UARTSERIAL), std::string name = "Serial");
    void setup() override;
    void sendPacket(RnpPacket &data) override;
    void update() override;
    const RnpInterfaceInfo *getInfo() override { return &_info; };

private:
    HardwareSerial &_serial;     // pointer to stream interface
    SystemStatus &_systemstatus; // pointer to system status object
    LogController &_logcontroller;

    UARTInterfaceInfo _info;

    std::vector<uint8_t> _sendBuffer;
    std::vector<uint8_t> _receiveBuffer;
    void checkSendBuffer();
    void getPackets();
};
