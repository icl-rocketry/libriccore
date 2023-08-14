/**
 * @file riccorethread.cpp
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Contains implementation specific to unix platform for riccorethread
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <libriccore/platform/riccorethread.h>

#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>

RicCoreThread::Thread::Thread(std::function<void(void *)> f_ptr, void *args, const size_t stack_size, const int priority, const CORE_ID coreID, std::string_view name)
{

    if (coreID > CORE_ID::ANYCORE)
    {
        throw std::runtime_error("Illegal Core specified!");
    }

    std::thread thread(f_ptr, args);
    handle = std::move(thread);
    success = true;
}

RicCoreThread::Thread::~Thread()
{
    handle.join();
}

RicCoreThread::Lock::Lock() {}

void RicCoreThread::Lock::acquire()
{
    lock.lock();
}

void RicCoreThread::Lock::release()
{
    lock.unlock();
}

void RicCoreThread::delay(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void RicCoreThread::ThreadWorkSemaphore::up()
{
    counter = true;
}

void RicCoreThread::ThreadWorkSemaphore::waitForWork()
{
    while (!counter)
    {
        std::this_thread::yield(); // Do something else;
    }
}

void RicCoreThread::ThreadWorkSemaphore::down()
{

    counter = false;
}

bool RicCoreThread::ThreadWorkSemaphore::get()
{
    return counter.load();
}