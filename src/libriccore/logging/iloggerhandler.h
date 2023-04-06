#pragma once

#include "loggerhandler.h"

/**
 * @brief Interface to allow RicCoreSystem classes retrieve the instance of the logger handler singleton
 *  This enforces a DI interface for anything that wants to do more than just use the loggers i.e stopping all logging etc...
 * 
 */
struct ILoggerHandler{
    private:
        template<typename P1,typename P2,typename P3> //placeholder required as RicCoreSystem is a template
        friend class RicCoreSystem;



        static LoggerHandler& getInstance(){return LoggerHandler::getInstance();};
};

