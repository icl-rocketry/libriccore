#pragma once
#include <tuple>
#include "loggers/coutlogger.h"

enum class LOGGER
{
    SYS
};

constexpr auto logger_list = std::make_tuple(CoutLogger());
