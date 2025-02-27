#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace RicCoreUtil {
    inline uint32_t millisFast() {
        return xTaskGetTickCount() * portTICK_PERIOD_MS;
    };
}