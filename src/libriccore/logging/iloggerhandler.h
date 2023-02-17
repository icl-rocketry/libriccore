#pragma once

#include "loggerhandler.h"

/**
 * @brief Interface to allow RicCoreSystem classes retrieve the instance of the logger handler singleton
 *  This enforces a DI interface for anything that wants to do more than just use the loggers i.e stopping all logging etc...
 * 
 */
struct ILoggerHandler{
    private:
        template<typename ...PLACEHOLDER> //placeholder required as RicCoreSystem is a template
        friend class RicCoreSystem;

        template<typename ...PLACEHOLDER>
        /**
         * @brief the multi sink logger requires access to the logger handler to retrieve the loggers assigned to it
         * 
         */
        friend class MultiSink;

        static LoggerHandler& getInstance(){return LoggerHandler::getInstance();};
};

