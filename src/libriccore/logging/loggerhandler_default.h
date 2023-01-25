#pragma once
#include <tuple>
#include "loggers/coutlogger.h"

enum class LOGGER
{
    SYS
};

const inline auto logger_list = std::make_tuple(CoutLogger("SYS_LOG"));
