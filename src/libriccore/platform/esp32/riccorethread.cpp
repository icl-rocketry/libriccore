#include <libriccore/threading/riccorethread.h>
#include "riccorethread_types.h"

#include <freertos/task.h>
#include <freertos/projdefs.h>

#include <esp_pthread.h>

#include <string>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>


RicCoreThread::Thread::Thread(std::function<void(void*)> f_ptr, void* args,const size_t stack_size,const int priority,const CORE_ID coreID, std::string_view name) 
{
    if (coreID > CORE_ID::ANYCORE)
    {
        throw std::runtime_error("Illegal core id given!");
    }

    BaseType_t result;

    //construct lambda wrapper to include deleter at the return of f_ptr
    std::function<void(void *)> wrapped_f_ptr = [f_ptr](void *args){
                                                        f_ptr(args);
                                                        vTaskDelete(nullptr); //deletes the current running task
                                                    };

    if (coreID == CORE_ID::ANYCORE)
    {
        result = xTaskCreatePinnedToCore(wrapped_f_ptr, name.substr(), stack_size, args, priority, handle, tskNO_AFFINITY);
    }
    else
    {
        result = xTaskCreatePinnedToCore(wrapped_f_ptr, name.substr(), stack_size, args, priority, handle, static_cast<BaseType_t>(coreID));
    }

    if (result != pdPASS)
    {
        throw std::runtime_error("Thread failed to start with error code:" + std::to_string(static_cast<int>(result)));
    }
}
   

RicCoreThread::Thread::~Thread() {
    join();
    
    // if ((handle != nullptr) && (eTaskGetState(handle) != eTaskState::eDeleted))
    // {
    //     vTaskDelete(handle); // not an equivlanet of thread.join() so need to make one, maybe use task notifications?
    // }
}

void RicCoreThread::Thread::join()
{

    //if the task handle is not null (i.e still exists) OR the state of the task is not deleted (i.e is still running or blocked),
    // block the callee of this function until the thread of interest is non-existant
    while((handle != nullptr) || (eTaskGetState(handle) != eTaskState::eDeleted))
    {
        vTaskDelay(1);
    }
}


void RicCoreThread::delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void RicCoreThread::block()
{
    vTaskDelay(1);
}

