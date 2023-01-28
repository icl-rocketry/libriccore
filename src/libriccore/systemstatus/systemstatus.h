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

#include "riccorelogging.h"
#include "systemflags_config_tweak.h"




class SystemStatus : public BitwiseFlagManager<RicCoreConfig::SYSTEM_FLAGS>
{
public:
    SystemStatus() : BitwiseFlagManager::BitwiseFlagManager(0)
                     {};


    /**
     * @brief raise new flag in system status
     * 
     * @param flag 
     * @param info 
     */
    void newFlag(RicCoreConfig::SYSTEM_FLAGS flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<RicCoreConfig::LOGGERS::SYS>(getStatus(), static_cast<uint32_t>(flag), info);
    };

    void newFlag(RicCoreConfig::SYSTEM_FLAGS flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<RicCoreConfig::LOGGERS::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

    void deleteFlag(RicCoreConfig::SYSTEM_FLAGS flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<RicCoreConfig::LOGGERS::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag removed");
    };

    void deleteFlag(RicCoreConfig::SYSTEM_FLAGS flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<RicCoreConfig::LOGGERS::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag removed");
    };

};
