#include <headers/thread.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <memory>

Thread::Thread(void (*f)(void* args), void* args, size_t stack_size, int priority, std::string name) {
    std::thread thread(f, args);
    handle = std::move(thread);
    success = true;
}

Thread::~Thread() {
    if (handle.joinable()) {
        join();
    }
}

void Thread::join() {
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
