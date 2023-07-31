/**
 * @file canbus.h
 * @author Kiran de Silva
 * @brief Can Bus segementation and re-assmebly driver.
 * TODO
 * - implement support for tracking error status of can driver
 * - add support for hardware based filtering (promiscous mode)
 * - add stub to emulate can driver on linux system
 * @version 0.1
 * @date 2023-02-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

#include <librnp/rnp_interface.h>
#include <librnp/rnp_packet.h>

#include "driver/gpio.h"
#include "driver/twai.h"

#include <libriccore/riccorelogging.h>
#include <libriccore/systemstatus/systemstatus.h>

#include "rnpcanidentifier.h"

struct CanBusInterfaceInfo : public RnpInterfaceInfo
{
    uint8_t maxSendBufferElements;
    uint8_t maxReceiveBufferElements;

    bool sendBufferOverflow;
    bool receiveBufferOverflow;
};

template <typename SYSTEM_FLAGS_T, RicCoreLoggingConfig::LOGGERS LOGGING_TARGET = RicCoreLoggingConfig::LOGGERS::SYS>
class CanBus : public RnpInterface
{
public:
    CanBus(SystemStatus<SYSTEM_FLAGS_T> &systemstatus, const uint8_t TxCan, const uint8_t RxCan, uint8_t id, std::string name = "Can0") : 
    RnpInterface(id, name),
    _systemstatus(systemstatus),
    can_general_config(
        {
            .mode = TWAI_MODE_NORMAL,
            .tx_io = static_cast<gpio_num_t>(TxCan),
            .rx_io = static_cast<gpio_num_t>(RxCan),
            .clkout_io = TWAI_IO_UNUSED,
            .bus_off_io = TWAI_IO_UNUSED,
            .tx_queue_len = 20,
            .rx_queue_len = 20,
            .alerts_enabled = TWAI_ALERT_NONE,
            .clkout_divider = 0,
        }),
    can_timing_config(TWAI_TIMING_CONFIG_1MBITS()),
    can_filter_config(TWAI_FILTER_CONFIG_ACCEPT_ALL())
    {
        _info.MTU = 256; // theoretical maximum is 2048 but this is very chonky
        _info.maxSendBufferElements = 10;
        _info.maxReceiveBufferElements = 10;
    };

    void setup() override
    {
        if (twai_driver_install(&can_general_config, &can_timing_config, &can_filter_config) != ESP_OK)
        {
            _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can iface failed to install!");

            return;
        }
        if (twai_start() != ESP_OK)
        {
            _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can Iface failed to start!");

            return;
        }
        RicCoreLogging::log<LOGGING_TARGET>("Can started!");
    };

    void sendPacket(RnpPacket &data) override
    {
        if ((data.header.size() + data.header.packet_len) > _info.MTU)
        {
            RicCoreLogging::log<LOGGING_TARGET>("Packet Exceeds Can MTU!");
            return;
        }
        if (_sendBuffer.size() + 1 > _info.maxSendBufferElements)
        {

            if (!_systemstatus.flagSet(SYSTEM_FLAG::ERROR_CAN))
            {
                _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can Send Buffer Overflow!");
            }
            _info.sendBufferOverflow = true;
            return;
        }

        std::vector<uint8_t> serializedPacket;
        data.serialize(serializedPacket);
        _sendBuffer.emplace(send_buffer_element_t{RnpCanIdentifier(data.header, generateCanPacketId()), serializedPacket});

        if (_info.sendBufferOverflow && _systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_CAN))
        {
            _systemstatus.deleteFlag(SYSTEM_FLAG::ERROR_CAN, "Can Send Buffer no longer overflowing!");
            _info.sendBufferOverflow = false;
        }
    };
    void update() override
    {
        processSendBuffer();
        processReceivedPackets();

        if (millis() - prevTime > cleanup_delta)
        {
            cleanupReceiveBuffer();
            prevTime = millis();
        }
    };
    const RnpInterfaceInfo *getInfo() override { return &_info; };

    /**
     * @brief Set the Hardware Acceptance Filter. Will uninstall and reinstall can driver
     * NOT IMPLEMENTED YET!
     * @param mask
     */
    void setAcceptanceFilter(uint32_t mask){};

private:
    SystemStatus<SYSTEM_FLAGS_T> &_systemstatus;

    CanBusInterfaceInfo _info;

    // CAN DRIVER CONFIG //
    const twai_general_config_t can_general_config;
    const twai_timing_config_t can_timing_config;
    const twai_filter_config_t can_filter_config;
    // CAN DRIVER CONFIG //

    uint8_t packet_counter{0};
    /**
     * @brief Generates an incrementing sequence of 5bit unsigned integers which wraps back to zero on overflow
     *
     * @return uint8_t
     */
    uint8_t generateCanPacketId()
    {
        uint8_t can_packet_id = packet_counter % 0x1F;
        ++packet_counter;
        return can_packet_id;
    };

    static constexpr uint32_t receive_buffer_expiry = 120 * 1000; // 2 minute expiry time

    struct send_buffer_element_t
    {
        RnpCanIdentifier canidentifier;
        std::vector<uint8_t> bytedata;
    };

    /**
     * @brief Send buffer Container
     *
     */
    std::queue<send_buffer_element_t> _sendBuffer;

    struct receive_buffer_element_t
    {
        std::vector<uint8_t> bytedata;
        size_t expected_size;
        uint8_t seg_id;
        uint32_t last_time_modified;
    };

    /**
     * @brief Receive Buffer Container
     *
     */
    std::unordered_map<uint32_t, receive_buffer_element_t> _receiveBuffer;

    void processReceivedPackets()
    {
        twai_message_t can_packet;
        int err = twai_receive(&can_packet, 0);
        if (err != ESP_OK)
        {
            if (err != ESP_ERR_TIMEOUT)
            {
                if (!_systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_CAN))
                {
                    _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can Receive failed with error code" + std::to_string(err));
                }
            }
            return;
        }

        if (!(can_packet.flags & TWAI_MSG_FLAG_EXTD))
        {
            RicCoreLogging::log<LOGGING_TARGET>("Bad Can Packet Type, Packet Dumped!");
            return;
        }
        // decode identifier
        const RnpCanIdentifier can_identifier(can_packet.identifier);
        const uint32_t can_packet_uid = RnpCanIdentifier::getCanPacketUID(can_packet.identifier);

        if (can_identifier.seg_id == 0) // marks the start of a new packet
        {

            // check if uid already exists in receive buffer
            if (_receiveBuffer.count(can_packet_uid))
            {

                // receive buffer already contains a matching uid implying that we will never receive
                // the rest of the previous packet with the matching key so remove the old packet.
                // to ensure that someone hasnt messed up rollover of seg_id, we check the last seg_id so that we
                // are confident this is a new packet
                if (_receiveBuffer.at(can_packet_uid).seg_id == 0xFF)
                {
                    // we are now unsure wether this received packet marks a new packet or if it is the next segment in the sequence
                    // so we dump the element in the receive buffer aswell as the new packet to avoid corrupted packets
                    _receiveBuffer.erase(can_packet_uid);
                    return;
                }
                _receiveBuffer.erase(can_packet_uid); // erase previous entry and start new packet
            }

            // construct new received packet buffer element
            // check if receiveBuffer has space to push back to
            if (_receiveBuffer.size() == _info.maxReceiveBufferElements)
            {
                _info.receiveBufferOverflow = true;
                if (!_systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_CAN))
                {
                    _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can Receive Buffer Overflow" + std::to_string(err));
                }
                return;
            }

            _receiveBuffer.emplace(can_packet_uid,
                                   receive_buffer_element_t{std::vector<uint8_t>(can_packet.data, can_packet.data + can_packet.data_length_code),
                                                            0,
                                                            0,
                                                            millis()});

            if (_info.receiveBufferOverflow && _systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_CAN))
            {
                _systemstatus.deleteFlag(SYSTEM_FLAG::ERROR_CAN, "Can Receive Buffer no longer overflowing!");
                _info.receiveBufferOverflow = false;
            }

            return;
        }
        // check if can packet uid is in the receive buffer
        if (!(_receiveBuffer.count(can_packet_uid)))
        {
            return; // ignore received packet
        }
        // get correct receive buffer element
        receive_buffer_element_t &receive_buffer_element = _receiveBuffer.at(can_packet_uid);
        // check seg_id are sequential
        if (((can_identifier.seg_id == receive_buffer_element.seg_id + 1)) || ((can_identifier.seg_id == 1) && (receive_buffer_element.seg_id == 0xFF)))
        {
            // append receive buffer element vector with can packet data
            const size_t bytedata_size = receive_buffer_element.bytedata.size();
            // resize vector
            receive_buffer_element.bytedata.resize(bytedata_size + can_packet.data_length_code);
            // copy new data
            std::memcpy(receive_buffer_element.bytedata.data() + bytedata_size, &can_packet.data, can_packet.data_length_code);
            // update last time modified
            receive_buffer_element.last_time_modified = millis();

            // check if the previous received can packet was the start of the rnp packet in which case we can deserialize the header to get expected length.
            if (receive_buffer_element.seg_id == 0 || receive_buffer_element.expected_size == 0)
            {
                if (receive_buffer_element.bytedata.size() >= RnpHeader::size()) // make sure we have enough bytes to deserialize header
                {
                    RnpHeader header(receive_buffer_element.bytedata);
                    receive_buffer_element.expected_size = header.packet_len + RnpHeader::size();

                    // reserve vector size to reduce more allocations
                    receive_buffer_element.bytedata.reserve(receive_buffer_element.expected_size);
                }
            }
            // update seg_id
            receive_buffer_element.seg_id = can_identifier.seg_id;

            // check if we have fully received a packet
            if (receive_buffer_element.bytedata.size() == receive_buffer_element.expected_size)
            {
                auto packet_ptr = std::make_unique<RnpPacketSerialized>(receive_buffer_element.bytedata);
                packet_ptr->header.src_iface = getID();        // update source interface id
                _packetBuffer->emplace(std::move(packet_ptr)); // push to network manager packet buffer
                // cleanup receive buffer
                _receiveBuffer.erase(can_packet_uid);
                return;
            }
        }
        else // can packet segment received out of order, delete recieve buffer element to prevent packet corruption. Maybe think of retransmit later
        {
            _receiveBuffer.erase(can_packet_uid);
            return;
        }
    };

    void processSendBuffer()
    {
        if (_sendBuffer.empty())
        {
            return;
        }

        const send_buffer_element_t &packet = _sendBuffer.front();
        const size_t data_size = packet.bytedata.size();
        const uint8_t curr_seg_id = packet.canidentifier.seg_id;

        uint8_t offset = curr_seg_id * 8;
        uint8_t bytes_left = data_size - offset;

        twai_message_t can_packet;
        can_packet.identifier = packet.canidentifier.getIdentifier();
        can_packet.flags = TWAI_MSG_FLAG_EXTD;
        can_packet.data_length_code = (bytes_left > 8) ? 8 : bytes_left;

        std::memcpy(&can_packet.data, packet.bytedata.data() + offset, can_packet.data_length_code);

        int err = twai_transmit(&can_packet, 0); // non blocking send
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_TIMEOUT || err == ESP_FAIL)
            {
                // can tx buffer full, dont increment seg_id and try to place on buffer next update

                RicCoreLogging::log<LOGGING_TARGET>("Can tx buffer full");
                return;
            }
            // proper error might be worth throwing here? -> future
            if (!_systemstatus.flagSetOr(SYSTEM_FLAG::ERROR_CAN))
            {
                _systemstatus.newFlag(SYSTEM_FLAG::ERROR_CAN, "Can transmit failed with error code" + std::to_string(err));
            }
            return;
        }
        // check if we just sent the last segment of the rnp packet
        if (can_packet.data_length_code + offset == data_size)
        {
            // all segments have been sent
            _sendBuffer.pop();
            return;
        }
        // increment seg_id noting the rollover to prevent repeating of 0
        // In this current implementation due to MTU restrictions, this should never happen, but is implemented
        // for sake of completeness.
        _sendBuffer.front().canidentifier.seg_id = (curr_seg_id >= 0xFF) ? 1 : curr_seg_id + 1; // increment seg_id
        return;
    };

    /**
     * @brief How long before a cleanup is called
     *
     */
    static constexpr uint32_t cleanup_delta = 60 * 1000; // every minute
    uint32_t prevTime;
    void cleanupReceiveBuffer()
    {
        for (auto &[key, value] : _receiveBuffer)
        {
            if (millis() - value.last_time_modified > receive_buffer_expiry)
            {
                _receiveBuffer.erase(key);
            }
        }
    };
};
