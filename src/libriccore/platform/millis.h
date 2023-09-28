#pragma once

#ifdef LIBRICCORE_UNIX
    #include "unix/millis_stub.h"
#else
    #include "esp32/millis.h"
    // #include <Arduino.h> // arduino.h has the defintion for millis
#endif  