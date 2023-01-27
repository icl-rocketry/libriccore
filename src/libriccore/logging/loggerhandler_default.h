#pragma once
#include <tuple>
#include "loggers/coutlogger.h"


enum class LOGGER
{
    SYS // default system logging
};

const inline std::tuple logger_list = 
{
    CoutLogger("SYS_LOG")
};
