#include <libriccore/platform/thread.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>

Thread::Thread(std::function<void(void*)> f, void* args, size_t stack_size, int priority, std::string name) {
    std::thread thread(f, args);
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

void Semaphore::up() {
    sema.up();
}

void Semaphore::down() {
    sema.down();
}

bool Semaphore::get() {
    return sema.get();
}
