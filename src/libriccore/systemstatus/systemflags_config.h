#pragma once

#if __has_include("config/systemflags.h")
  #include "config/systemflags.h"
  namespace DEFAULT_CONFIG
    {
      #include "systemflags_default.h"
    };
#else
  #include "systemflags_default.h"
#endif    
