#pragma once


#if __has_include("config/loggerhandler_config.h")
  #include "config/loggerhandler_config.h"
  namespace DEFAULT_CONFIG
    {
      #include "loggerhandler_default.h"
    };
#else
  #include "loggerhandler_default.h"
#endif    

