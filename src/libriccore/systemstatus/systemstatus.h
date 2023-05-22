/**
 * @file systemstatus.h
 * @author kiran de silva
 * @brief Keeps track of the status of the whole system
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include "bitwiseflagmanager.h"

#include <string>

#include <libriccore/riccorelogging.h>




template<typename SYSTEM_FLAGS_T,RicCoreLoggingConfig::LOGGERS LOGGING_TARGET = RicCoreLoggingConfig::LOGGERS::SYS>
class SystemStatus : public BitwiseFlagManager<SYSTEM_FLAGS_T>
{
    // check passed system_flags_t for default flags present
    
public:
    SystemStatus() : BitwiseFlagManager<SYSTEM_FLAGS_T>::BitwiseFlagManager(0)
                     {};


    /**
     * @brief raise new flag in system status
     * 
     * @param flag 
     * @param info 
     */
    void newFlag(SYSTEM_FLAGS_T flag, std::string_view info)
    {
        BitwiseFlagManager<SYSTEM_FLAGS_T>::newFlag(flag);
        RicCoreLogging::log<LOGGING_TARGET>(this->getStatus(), static_cast<uint32_t>(flag), info);
    };

    void newFlag(SYSTEM_FLAGS_T flag) override
    {
        BitwiseFlagManager<SYSTEM_FLAGS_T>::newFlag(flag);
        RicCoreLogging::log<LOGGING_TARGET>(this->getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

    void deleteFlag(SYSTEM_FLAGS_T flag) override
    {
        BitwiseFlagManager<SYSTEM_FLAGS_T>::deleteFlag(flag);
        RicCoreLogging::log<LOGGING_TARGET>(this->getStatus(), static_cast<uint32_t>(flag), "flag removed");
    };

    void deleteFlag(SYSTEM_FLAGS_T flag, std::string_view info)
    {
        BitwiseFlagManager<SYSTEM_FLAGS_T>::deleteFlag(flag);
        RicCoreLogging::log<LOGGING_TARGET>(this->getStatus(), static_cast<uint32_t>(flag), info);
    };

};
