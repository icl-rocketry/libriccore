#pragma once
/**
 * @file coutlogger.h
 * @author your name (you@domain.com)
 * @brief Basic logger which prints to either serial out or cout depending if arduino is defined
 * @version 0.1
 * @date 2023-01-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "loggerbase.h"

#include <iostream>
#include <string>
#include <libriccore/platform/millis.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif



class CoutLogger : public LoggerBase
{
public:
    CoutLogger(const std::string_view name) : LoggerBase(),
                                              logger_name(name){
                                              };

    void log(std::string_view msg)
    {   
        if (!enabled){return;};
        
        #ifdef ARDUINO
        Serial.println((std::string(logger_name) + ":[" + std::to_string(millis()).c_str() + "] -> "  + std::string(msg)).c_str());
        #else
        std::cout << logger_name << ":[" + std::to_string(millis()) + "] -> " << msg << "\n";
        #endif
    };

    void log(uint32_t status,uint32_t flag,std::string_view message)
    {
        std::string log_str = std::to_string(flag) + "," + std::string(message) + "," + std::to_string(status);
        log(log_str);
    };

    
    ~CoutLogger(){};

private:
    const std::string_view logger_name;
};