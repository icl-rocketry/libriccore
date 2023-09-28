#pragma once
/**
 * @file frerertos_lock.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief  Lock specialized for freertos, interface must match Unix_Lock
 * @version 0.1
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class FreeRTOS_Lock
{
    public:
        FreeRTOS_Lock():
        mutexBuffer(),
        mutex(xSemaphoreCreateMutexStatic(&mutexBuffer))
        {};

        void acquire()
        {
            xSemaphoreTake(mutex,portMAX_DELAY); //block until we can take semaphore
        };

        void release()
        {
            xSemaphoreGive(mutex); //return semaphore
        };
        
    private:
        StaticSemaphore_t mutexBuffer;
        SemaphoreHandle_t mutex;
        

};