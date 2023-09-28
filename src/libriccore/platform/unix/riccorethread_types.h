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
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>


#include "unix_lock.h"

// This file is used to specify base types for general headers
namespace RicCoreThread
{
    using ThreadHandle_t = std::thread;

    using Lock_t = Unix_Lock;

};