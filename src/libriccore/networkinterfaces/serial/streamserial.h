/**
 * @file streamserial.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Network interface to support any serial device which implements the stream interface 
 * @version 0.1
 * @date 2023-08-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
// stl
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <iostream>
// librnp
#include <librnp/rnp_interface.h>
#include <librnp/rnp_header.h>
#include <librnp/rnp_packet.h>
//logging
#include <libriccore/riccorelogging.h>

#include <libriccore/systemstatus/systemstatus.h>

#include "cobs.h"



struct StreamSerialInterfaceInfo : public RnpInterfaceInfo
{
    size_t sendBufferSize;
    bool sendBufferOverflow;
    size_t receiveBufferSize;
    bool receiveBufferOverflow;
};

template <typename SYSTEM_FLAGS_T,RicCoreLoggingConfig::LOGGERS LOGGING_TARGET = RicCoreLoggingConfig::LOGGERS::SYS>
class StreamSerial : public RnpInterface
{

public:
    StreamSerial(Stream &stream, SystemStatus<SYSTEM_FLAGS_T> &systemstatus, uint8_t id = static_cast<uint8_t>(DEFAULT_INTERFACES::USBSERIAL), std::string name = "StreamSerial0"):
     RnpInterface(id, name),
    _stream(stream),
    _systemstatus(systemstatus)
    {
        _info.MTU = 256;
        _info.sendBufferSize = 1024;
        _info.receiveBufferSize = 1024;

        _sendBuffer.reserve(200); // reserve average packet size in the send buffer to reduce heapfrag
        _receiveBuffer.reserve(200);
    };

    /**
     * @brief Setup the StreamSerial interface, as we don't own the serial interface, we leave setup to be performed externally to the StreamSerial interface.
     * @author Kiran de Silva
     */
    void setup() override{};

    /**
     * @brief Sends RNP packet over StreamSerial interface. The packet is serialzied into bytes, COBS encoded and then sent over the serial interface.
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
            _systemstatus.newFlag(SYSTEM_FLAGS_T::ERROR_SERIAL, "StreamSerial Send Buffer Overflow!");
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
     * @brief Reference to the stream interface
     * 
     */
    Stream &_stream;
    
    /**
     * @brief Reference to the system status object
     * 
     */
    SystemStatus<SYSTEM_FLAGS_T> &_systemstatus;

    /**
     * @brief Interface info struct
     * 
     */
    StreamSerialInterfaceInfo _info;

   
  
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
        // const size_t numBytes = _serial.availableForWrite();

        const size_t to_send = _sendBuffer.size();
        //this returns the actual number of bytes written
        const size_t numBytes = _stream.write(_sendBuffer.data(),to_send);
        const size_t leftover = to_send-numBytes;

        if(leftover)
        {
            _sendBuffer.erase(_sendBuffer.begin(), _sendBuffer.begin() + numBytes); // remove the sent data
        }
        else
        {
            _sendBuffer.clear();
        }

        // if (numBytes < _sendBuffer.size())
        // {
        //     // _serial.write(_sendBuffer.data(), numBytes);
        //     _sendBuffer.erase(_sendBuffer.begin(), _sendBuffer.begin() + numBytes); // remove the sent data
        // }
        // else if (numBytes >= _sendBuffer.size())
        // {
        //     _serial.write(_sendBuffer.data(), _sendBuffer.size());
        //     _sendBuffer.clear();
        //     // maybe shrink to fit vector?
        // }
    };

    /**
     * @brief Checks serial receive buffer for any new data, proceeds to COBS decode data. Once a full COBS encoded stream is recevied, 
     * the data is decoded back into bytes and placed in an RnpSerialziedPacket object to partially decode the header while keeping the rest
     * of the data still serialzied.
     * 
     */
    void getPackets()
    {

        while (_stream.available() > 0)
        {
            uint8_t incomming = _stream.read();
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
                    _systemstatus.newFlag(SYSTEM_FLAGS_T::ERROR_SERIAL, "StreamSerial Receive Buffer Overflow!");
                }
            }
        }
    }
};
