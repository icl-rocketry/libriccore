#pragma once

#include "loggerbase.h"

#include <iostream>
#include <string>



class CoutLogger : public LoggerBase
{
public:
    // constexpr CoutLogger(const std::string_view name = "CoutLogger") : LoggerBase(),
    //                                                                    logger_name(name){};

    constexpr CoutLogger() : LoggerBase(),
                            logger_name("SYS"){};

    void log(std::string_view msg)
    {
        std::cout << logger_name << " -> " << msg << "\n";
    };

private:
    const std::string_view logger_name;
};