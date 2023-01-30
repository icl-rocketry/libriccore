#pragma once
#include "logging/loggerhandler_config_tweak.h"
#include "logging/loggerhandler.h"



struct RicCoreLogging{
   
    /**
     * @brief call log on the given logger with the correct arguments
     * 
     * @tparam NAME 
     * @tparam Ts 
     * @param args 
     */
    template<RicCoreLoggingConfig::LOGGERS NAME,class... Ts> // maybe add an assert here to check LOGGER contains NAME for nicer debug message
    static void log(Ts&&... args){
        LoggerHandler::getInstance().retrieve_logger<NAME>().log(std::forward<Ts>(args)...);
        };
        
};