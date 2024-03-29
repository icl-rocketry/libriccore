/**
 * @file rnpmessagelogger.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Sends all log messages over rnp to defined destination address using the log message ID
 * @version 0.1
 * @date 2023-06-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "loggerbase.h"

#include <iostream>
#include <string>
#include <libriccore/platform/millis.h>

#include <librnp/rnp_packet.h>
#include <librnp/rnp_networkmanager.h>



class RnpMessageLogger : public LoggerBase
{
public:
    RnpMessageLogger(const std::string_view name, const uint8_t destination = 1, const uint8_t destination_service = 0) : 
                                              LoggerBase(),
                                              logger_name(name),
                                              destinationAddress(destination),
                                              destinationService(destination_service),
                                              _netman(nullptr)
                                              {};
                                              

    void initialize(RnpNetworkManager& netman)
    {
        _netman = &netman;
        initialized = true;
    };

    void log(std::string_view msg)
    {   

        if (!enabled){return;};

        if (!initialized){return;};

        if (_netman == nullptr){return;}; // this shouldnt ever really happen

        
        //well this is awful
        const std::string msg_str = std::string(msg);

        MessagePacket_Base<0,packetType> message(msg_str);
        
        //Note this needsto be changed so it makes more sense
        message.header.source_service = 0;
        message.header.source = _netman->getAddress();
        message.header.destination = destinationAddress;
        message.header.destination_service=destinationService;
        message.header.uid = 0;

        _netman->sendPacket(message);
        
       
    };

    void log(uint32_t status,uint32_t flag,std::string_view message)
    {
        std::string log_str = std::to_string(flag) + "," + std::string(message) + "," + std::to_string(status);
        log(log_str);
    };

    void changeNetworkTarget(uint8_t destination,uint8_t destination_service)
    {
        destinationAddress=destination;
        destinationService=destination_service;
    }

    ~RnpMessageLogger(){};

private:
    const std::string logger_name;
    uint8_t destinationAddress;
    uint8_t destinationService;

    static constexpr int packetType = 100; // defined in the rnp_registered nodes excel

    /**
     * @brief Pointer to network manager instance
     * 
     */
    RnpNetworkManager* _netman;
};