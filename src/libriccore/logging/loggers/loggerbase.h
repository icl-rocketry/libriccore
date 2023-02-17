#pragma once

/**
 * @brief Abstract interface for a logger in the loggerhandler. Note a logger maybe be constructed in a non initalized state 
 * as the logger may depend on system objects which havent been constructed such as the storage manager.
 * 
 */
class LoggerBase{
    public:

        LoggerBase():
        initalized(false),
        enabled(true)
        {};
        
        ~LoggerBase(){};

        /**
         * @brief Perform any intialization required by the derived logger
         * 
         */
        virtual void initialize(){};

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
        
    
    protected:
        bool initalized;
        bool enabled;

};