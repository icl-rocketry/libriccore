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
        /**
         * @brief Construct a new Unique Ptr Channel object.
         *
         * @param maxSize Maximum size of queue, -1 for no max size.
         */
        UniquePtrChannel(size_t maxSize = -1) : _maxSize(maxSize){};

        /**
         * @brief Add new element to queue, throws runtime_error if maxSize exceeded
         *
         * @param item
         */
        void send(std::unique_ptr<T> item)
        {
            auto l = ScopedLock(lock);
            if (vec.size() == _maxSize)
            {
                throw std::runtime_error("Queue Maxsize exceeded!");
            }
            vec.push(std::move(item));
        }

        /**
         * @brief Removes the element at the front of the queue and returns it. Returns nullptr if no element is in queue
         *
         * @return std::unique_ptr<T>
         */
        std::unique_ptr<T> pop()
        {
            auto l = ScopedLock(lock);
            if (vec.empty())
            {
                return nullptr;
            }
            std::unique_ptr<T> ret = std::move(vec.front());
            vec.pop();
            return ret;
        }

        bool empty()
        {
            auto l = ScopedLock(lock);
            return vec.empty();
        }

        void clear()
        {
            auto l = ScopedLock(lock);
            vec = {};
        }

        size_t size()
        {
            auto l = ScopedLock(lock);
            return vec.size();
        }

    private:
        Lock_t lock;
        std::queue<std::unique_ptr<T>> vec;
        size_t _maxSize;
    };
};
