#pragma once
/**
 * @file uniqueptrchannel.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Contains unique pointer channel template for unix platform
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <thread>
#include <queue>
#include <atomic>
#include <memory>

#include <libriccore/platform/riccorethread_types.h>
#include "scopedlock.h"

namespace RicCoreThread
{
    template <typename T>
    class UniquePtrChannel
    {
    public:
        void send(std::unique_ptr<T> item)
        {
            auto l = ScopedLock(lock);
            vec.push(std::move(item));
        }

        std::unique_ptr<T> pop()
        {
            auto l = ScopedLock(lock);
            std::unique_ptr<T> ret = std::move(vec.front());
            vec.pop();
            return ret;
        }

        bool empty()
        {
            auto l = ScopedLock(lock);
            return vec.empty();
        }

        size_t size()
        {
            auto l = ScopedLock(lock);
            return vec.size();
        }

    private:
        Lock_t lock;
        std::queue<std::unique_ptr<T>> vec;
    };
};
