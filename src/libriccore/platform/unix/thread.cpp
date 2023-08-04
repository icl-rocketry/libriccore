#include <libriccore/platform/thread.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>

Thread::Thread(std::function<void(void*)> f_ptr, void* args,const size_t stack_size,const int priority,const CORE_ID coreID, std::string_view name) {
    
    if (coreID > CORE_ID::ANYCORE){
        throw std::runtime_error("Illegal Core specified!");
    }

    std::thread thread(f_ptr, args);
    handle = std::move(thread);
    success = true;
}

Thread::~Thread() {
    handle.join();
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


