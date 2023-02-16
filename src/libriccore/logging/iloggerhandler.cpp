#include "iloggerhandler.h"
#include "loggerhandler.h"


LoggerHandler& ILoggerHandler::getInstance(){
    return LoggerHandler::getInstance();
}