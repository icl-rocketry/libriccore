#pragma once
/**
 * @file multisink.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief This provides a multi sink logger, enabling loggers with matching log interfaces to be tied together with a common logging target.
 * This allows a user to send a log to a single target, and this message to be replicated between N defined loggers.
 * @version 0.1
 * @date 2023-02-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <initializer_list>

#include "loggerbase.h"
#include "iloggerhandler.h"

//Forward declarations
// class LoggerHandler;
// struct ILoggerHandler{
//     static LoggerHandler& getInstance();
// };

template<class ...Loggers>
class MultiSink : public LoggerBase{
    public:
        template<class ...LoggerNames_t>
        constexpr MultiSink(LoggerNames_t... logger_names){
            logger_list = std::make_tuple(ILoggerHandler::getInstance().retrieve_logger<logger_names>()...);
        }

    private:
        std::tuple<Loggers&...> logger_list;

};