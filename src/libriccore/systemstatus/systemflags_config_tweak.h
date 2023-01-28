#pragma once

#if __has_include("config/systemflags_config.h")
  #include "config/systemflags_config.h"
  namespace DEFAULT_CONFIG
    {
      #include "systemflags_config_default.h"
    };
#else
  #include "systemflags_config_default.h"
#endif    
