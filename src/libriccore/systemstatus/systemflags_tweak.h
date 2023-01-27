#pragma once

#if __has_include("config/systemflags_config.h")
  #include "config/systemflags_config.h"
  namespace DEFAULT_CONFIG
    {
      #include "systemflags_default.h"
    };
#else
  #include "systemflags_default.h"
#endif    
