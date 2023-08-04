#include <libriccore/platform/thread.h>

#include <freertos/task.h>
#include <freertos/projdefs.h>

#include <esp_pthread.h>

#include <string>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>


Thread::Thread(std::function<void(void*)> f_ptr, void* args,const size_t stack_size,const int priority,const CORE_ID coreID, std::string_view name) 
{
    if (coreID > CORE_ID::ANYCORE)
    {
        throw std::runtime_error("Illegal core id given!");
    }

    BaseType_t result;

    if (coreID == CORE_ID::ANYCORE)
    {
        result = xTaskCreatePinnedToCore(f_ptr, name.substr(), stack_size, args, priority, handle, tskNO_AFFINITY);
    }
    else
    {
        result = xTaskCreatePinnedToCore(f_ptr, name.substr(), stack_size, args, priority, handle, static_cast<BaseType_t>(coreID));
    }

    if (result != pdPASS)
    {
        throw std::runtime_error("Thread failed to start with error code:" + std::to_string(static_cast<int>(result)));
    }
}
   

Thread::~Thread() {
    // handle.join();
    
    if (handle != nullptr)
    {

    }
}

Lock::Lock() {}

void Lock::acquire() {
    lock.lock();
}

void Lock::release() {
    lock.unlock();
}


void delay(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

