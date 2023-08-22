#include <libriccore/threading/riccorethread.h>
#include "riccorethread_types.h"

#include <string>
#include <memory>
#include <functional>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/projdefs.h>

// #include <esp_pthread.h>




RicCoreThread::Thread::Thread(std::function<void(void*)> f_ptr, void* args,const size_t stack_size,const int priority,const CORE_ID coreID, std::string_view name) 
{
    if (coreID > CORE_ID::ANYCORE)
    {
        throw std::runtime_error("Illegal core id given!");
    }

    BaseType_t result;

    struct TaskArgs
    {
        std::function<void(void*)> taskCode;
        void* args;
    };

    TaskArgs* wrapped_f_args = new TaskArgs{f_ptr,args};
    //construct lambda wrapper to include deleter at the return of f_ptr
    auto wrapped_f_ptr = [](void *args){
                                            if (args != nullptr)
                                            {
                                                //copy task args to scoped variable
                                                TaskArgs taskArgs = *reinterpret_cast<TaskArgs*>(args); 
                                                //delete args as we have copied to scoped local variable
                                                delete reinterpret_cast<TaskArgs*>(args);
                                                taskArgs.taskCode(taskArgs.args);
                                            }
                                            vTaskDelete(nullptr); //deletes the current running task
                                            };
    
    if (coreID == CORE_ID::ANYCORE)
    {
        result = xTaskCreatePinnedToCore(wrapped_f_ptr, std::string(name).c_str(), stack_size, wrapped_f_args, priority, &handle, tskNO_AFFINITY);
    }
    else
    {
        result = xTaskCreatePinnedToCore(wrapped_f_ptr, std::string(name).c_str(), stack_size, wrapped_f_args, priority, &handle, static_cast<BaseType_t>(coreID));
    }

    if (result != pdPASS)
    {
        delete wrapped_f_args;
        throw std::runtime_error("Thread failed to start with error code:" + std::to_string(static_cast<int>(result)));
    }
}
   

RicCoreThread::Thread::~Thread() {
    join();
    
    //ensure task handle is deleted
    if ((handle != nullptr) && (eTaskGetState(handle) != eTaskState::eDeleted))
    {
        vTaskDelete(handle);
    }

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

