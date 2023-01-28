#pragma once
#include <tuple>
#include "loggers/coutlogger.h"

namespace RicCoreConfig
{
    enum class LOGGERS
    {
        SYS // default system logging
    };

    const inline std::tuple logger_list =
        {
            CoutLogger("SYS_LOG")};
};