#pragma once
// stl
#include <memory>
#include <vector>
#include <array>
#include <string>
// esp specifc
#include <HardwareSerial.h>
// librnp
#include "rnp_interface.h"
#include "rnp_header.h"
#include "rnp_packet.h"
//logging
#include "riccorelogging.h"

#include "cobs.h"

#include "systemstatus/systemstatus.h"

struct UARTInterfaceInfo : public RnpInterfaceInfo
{
    size_t sendBufferSize;
    bool sendBufferOverflow;
    size_t receiveBufferSize;
    bool receiveBufferOverflow;
};

template <typename SYSTEM_FLAGS_T,RicCoreLoggingConfig::LOGGERS LOGGING_TARGET = RicCoreLoggingConfig::LOGGERS::SYS>
class UART : public RnpInterface
{

public:
    UART(HardwareSerial &serial, SystemStatus<SYSTEM_FLAGS_T> &systemstatus, uint8_t id = static_cast<uint8_t>(DEFAULT_INTERFACES::UARTSERIAL), std::string name = "UART0"):
     RnpInterface(id, name),
    _serial(serial),
    _systemstatus(systemstatus)
    {
        _info.MTU = 256;
        _info.sendBufferSize = 1024;
        _info.receiveBufferSize = 1024;

        _sendBuffer.reserve(200); // reserve average packet size in the send buffer to reduce heapfrag
        _receiveBuffer.reserve(200);
    };

    /**
     * @brief Setup the UART interface, as we don't own the serial interface, we leave setup to be performed externally to the UART interface.
     * @author Kiran de Silva
     */
    void setup() override{};

    /**
     * @brief Sends RNP packet over UART interface. The packet is serialzied into bytes, COBS encoded and then sent over the serial interface.
     * @author Kiran de Silva
     * @param data reference to RnpPacket
     */
    void sendPacket(RnpPacket &data) override
    {
        const size_t dataSize = data.header.size() + data.header.packet_len;
        if (dataSize > _info.MTU)
        {
            RicCoreLogging::log<LOGGING_TARGET>("Packet Exceeds Serial MTU");
            ++_info.txerror;
            return;
        }
        if (dataSize + _sendBuffer.size() > _info.sendBufferSize)
        {
            // not enough space
            _systemstatus.newFlag(SYSTEM_FLAGS_T::ERROR_SERIAL, "UART Send Buffer Overflow!");
            ++_info.txerror;
            return;
        }
        std::vector<uint8_t> serializedData;
        data.serialize(serializedData);            // serialize the packet
        COBS::encode(serializedData, _sendBuffer); // encode the serialized data with COBS
        _sendBuffer.push_back(COBS::marker);       // push marker to mark end of packet
        checkSendBuffer();
    };

    /**
     * @brief Update loop, check send buffer for new data to send and check usb receive to decode any new data.
     * 
     * @author Kiran de Silva
     * 
     */
    void update() override
    {
        checkSendBuffer();
        getPackets();
    };

    /**
     * @brief return interface info struct
     * 
     * @return const RnpInterfaceInfo* 
     */
    const RnpInterfaceInfo *getInfo() override { return &_info; };

private:
    /**
     * @brief Reference to the hardware serial interface
     * 
     */
    HardwareSerial &_serial;
    
    /**
     * @brief Reference to the system status object
     * 
     */
    SystemStatus<SYSTEM_FLAGS_T> &_systemstatus;

    /**
     * @brief Interface info struct
     * 
     */
    UARTInterfaceInfo _info;

   
  
    std::vector<uint8_t> _sendBuffer;
    std::vector<uint8_t> _receiveBuffer;

    /**
     * @brief CHecks if there is space on serial to send more data, and proceeds to consume from internal send buffer 
     * 
     */
    void checkSendBuffer()
    { // hmm this seems to be causing lag issues if a packet is split, i suspect the send buffer erase
        if (_sendBuffer.size() == 0)
        {
            return;
        }
        // _serial.write(_sendBuffer.data(),_sendBuffer.size());
        // _sendBuffer.clear();
        const size_t numBytes = _serial.availableForWrite();
        if (numBytes < _sendBuffer.size())
        {
            _serial.write(_sendBuffer.data(), numBytes);
            _sendBuffer.erase(_sendBuffer.begin(), _sendBuffer.begin() + numBytes); // remove the sent data
        }
        else if (numBytes >= _sendBuffer.size())
        {
            _serial.write(_sendBuffer.data(), _sendBuffer.size());
            _sendBuffer.clear();
            // maybe shrink to fit vector?
        }
    };

    /**
     * @brief Checks serial receive buffer for any new data, proceeds to COBS decode data. Once a full COBS encoded stream is recevied, 
     * the data is decoded back into bytes and placed in an RnpSerialziedPacket object to partially decode the header while keeping the rest
     * of the data still serialzied.
     * 
     */
    void getPackets()
    {

        while (_serial.available() > 0)
        {
            uint8_t incomming = _serial.read();
            if (_packetBuffer == nullptr)
            {
                continue; // avoid processing overhead of COBS if we cant store the packet anywhere so read out the packet
            }

            if (incomming == COBS::marker) // marks the start and end of packet
            {
                std::vector<uint8_t> _decodedData; // decoded data will always be samller than encoded data

                size_t numDecoded = COBS::decode(_receiveBuffer, _decodedData);
                _decodedData.resize(numDecoded);

                auto packet_ptr = std::make_unique<RnpPacketSerialized>(_decodedData);
                packet_ptr->header.src_iface = getID();
                _packetBuffer->push(std::move(packet_ptr));

                _receiveBuffer.clear();
                _info.receiveBufferOverflow = false;
            }
            else
            {
                if (_receiveBuffer.size() < _info.receiveBufferSize)
                {
                    _receiveBuffer.push_back(incomming);
                }
                else
                {
                    // buffer overflow, dump buffer
                    _receiveBuffer.clear();
                    _info.receiveBufferOverflow = true;
                    _systemstatus.newFlag(SYSTEM_FLAGS_T::ERROR_SERIAL, "UART Receive Buffer Overflow!");
                }
            }
        }
    }
};
