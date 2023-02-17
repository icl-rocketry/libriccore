
#include <iostream>

#include "riccorelogging.h"



int main() {
    // auto logfunc = RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS,std::string_view>;
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS,RicCoreLoggingConfig::LOGGERS::SYS>("test log");
    // logfunc("hello");
    return 0;
   
}