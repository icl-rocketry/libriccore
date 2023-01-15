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

#include <string>


#include "logController.h"

#include "bitwiseflagmanager.h"


template<typename SYSTEM_FLAG_ENUM>
class SystemStatus : public BitwiseFlagManager<SYSTEM_FLAG_ENUM>
{
public:
    SystemStatus(LogController &logcontroller) : BitwiseFlagManager::BitwiseFlagManager(0),
                                                 _logcontroller(logcontroller)
                                                 {};


    /**
     * @brief raise new flag in system status
     * 
     * @param flag 
     * @param info 
     */
    void newFlag(SYSTEM_FLAG_ENUM flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        _logcontroller.log(getStatus(), static_cast<uint32_t>(flag), info);
    };

    void newFlag(SYSTEM_FLAG_ENUM flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        _logcontroller.log(getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

    void deleteFlag(SYSTEM_FLAG_ENUM flag) override
    {
        BitwiseFlagManager::newFlag(flag);
        _logcontroller.log(getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

    void deleteFlag(SYSTEM_FLAG_ENUM flag, std::string_view info)
    {
        BitwiseFlagManager::newFlag(flag);
        _logcontroller.log(getStatus(), static_cast<uint32_t>(flag), "flag raised");
    };

private:
    LogController &_logcontroller;
};
