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
#include "systemflags_tweak.h"




class SystemStatus : public BitwiseFlagManager<SYSTEM_FLAGS>
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
    void newFlag(SYSTEM_FLAGS flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<LOGGER::SYS>(getStatus(), static_cast<uint32_t>(flag), info);
    };

    void newFlag(SYSTEM_FLAGS flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<LOGGER::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

    void deleteFlag(SYSTEM_FLAGS flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<LOGGER::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag removed");
    };

    void deleteFlag(SYSTEM_FLAGS flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        RicCoreLogging::log<LOGGER::SYS>(getStatus(), static_cast<uint32_t>(flag), "flag removed");
    };

};
