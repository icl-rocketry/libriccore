#pragma once

#include <stdint.h>


/**
 * @brief Abstract interface for a logger in the loggerhandler. 
 * NB a logger maybe be constructed in a non initalized state 
 * as the logger may depend on system objects which havent been constructed such as the storage manager.
 * 
 */
class LoggerBase{
    public:


        LoggerBase():
        initialized(false),
        enabled(true)
        {};
        
        ~LoggerBase(){};

        /**
         * @brief Enable the logger
         * 
         */
        virtual void enable(){enabled = true;};

        /**
         * @brief Disable the logger
         * 
         */
        virtual void disable(){enabled = false;};

        /**
         * @brief Update handle for derived loggers which require some sort of perodic call such as 
         * timeout based flushing of logs
         * 
         */
        virtual void update(){}; 
        
    
    public:
        bool initialized;
        bool enabled;
        

};