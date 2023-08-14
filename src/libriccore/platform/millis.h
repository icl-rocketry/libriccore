#pragma once

#ifdef LIBRICCORE_UNIX
    #include "unix/millis_stub.h"
#else
<<<<<<< HEAD
    #include "esp32/millis.h"
    // #include <Arduino.h> // arduino.h has the defintion for millis
=======
    // #include "esp32/millis_stub.h"
    #include <Arduino.h> // arduino.h has the defintion for millis
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)
#endif  