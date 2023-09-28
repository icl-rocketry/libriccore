#pragma once
/**
 * @file riccorethread_types.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Contains specialized types for unix platform for riccorethread
 * @version 0.1
 * @date 2023-08-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "freertos_lock.h"

// This file is used to specify base types for general headers
namespace RicCoreThread
{
    using ThreadHandle_t = TaskHandle_t;

    using Lock_t = FreeRTOS_Lock;

};