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
#include <libriccore/threading/riccorethread.h>
#include "riccorethread_types.h"

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
    join();
}

void RicCoreThread::Thread::join()
{
    handle.join();
}


void RicCoreThread::delay(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void RicCoreThread::block()
{
    std::this_thread::yield();
}
