#pragma once

#if __has_include("config/loggerhandler.h")
  #include "config/loggerhandler.h"
  namespace DEFAULT_CONFIG
    {
      #include "loggerhandler_default.h"
    };
#else
  #include "loggerhandler_default.h"
#endif    
