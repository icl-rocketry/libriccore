#pragma once

#ifdef LIBRICCORE_UNIX
    #include "unix/HardwareSerial_stub.h"
#else
    #include "esp32/HardwareSerial.h"
    // #include <Arduino.h> // arduino.h has the defintion for millis
#endif  