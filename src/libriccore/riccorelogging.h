#pragma once
#include "logging/loggerhandler_config.h"
#include "logging/loggerhandler.h"

struct RicCoreLogging{

    /**
     * @brief call log on the given logger with the correct arguments
     * 
     * @tparam NAME 
     * @tparam Ts 
     * @param args 
     */
    template<LOGGER NAME,class... Ts> // maybe add an assert here to check LOGGER contains NAME for nicer debug message
    static void log(Ts&&... args){
        // std::cout<<std::to_string(static_cast<int>(NAME))<<" called!"<<std::endl;
        // test_log(std::forward<Ts>(args)...);
        LoggerHandler::getInstance().retrieve_logger<NAME>().log(std::forward<Ts>(args)...);
        };
        

};