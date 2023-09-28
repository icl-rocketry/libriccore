#pragma once
#include "logging/loggerhandler.h"
#include "logging/loggerhandler_config_tweak.h"


struct RicCoreLogging{
   
    /**
     * @brief call log on the given logger with the correct arguments. Back to back variadic parameter packs 
     * are used to firstly support writing to mulitple loggers in the same function call, but also allowing any 
     * log funciton signature to be called as long as it exists in the logger implementation. Perfect forwarding is 
     * used to ensure that passed arguments are passed correctly i.e reference passed by reference. When calling multiple 
     * loggers, ensure that the log function signature exists for both loggers.
     * Note, if paassing the log function to a std::function or as a function pointer, you need to wrap the call to the logger
     * inside a lambda e.g:
     * [](std::string_view msg){RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(msg);};
     * as direct auto aliasing of this function is not currently possible.
     * 
     * @tparam logger_names name of loggers to log to 
     * @tparam Ts types of the function arguments 
     * @param args 
     */
    template<RicCoreLoggingConfig::LOGGERS... logger_names,class... Ts> // maybe add an assert here to check LOGGER contains NAME for nicer debug message
    static void log(Ts&&... args){
        ((LoggerHandler::getInstance().retrieve_logger<logger_names>().log(std::forward<Ts>(args)...)), ...);
        };





};