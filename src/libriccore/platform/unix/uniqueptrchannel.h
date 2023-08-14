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
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>


template <typename T>
class Unix_UniquePtrChannel
{
public:
    void send(std::unique_ptr<T> item)
    {
        auto l = std::scoped_lock(lock);
        vec.push(std::move(item));
    }

    std::unique_ptr<T> pop()
    {
        auto l = std::scoped_lock(lock);
        std::unique_ptr<T> ret = std::move(vec.front());
        vec.pop();
        return ret;
    }

    bool empty()
    {
        auto l = std::scoped_lock(lock);
        return vec.empty();
    }

    size_t size()
    {
        auto l = std::scoped_lock(lock);
        return vec.size();
    }

private:
    std::mutex lock;
    std::queue<std::unique_ptr<T>> vec;
};
