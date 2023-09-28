#pragma once
/**
 * @file unix_lock.h
 * @author Akshat
 * @brief Simple lock implementation
 * @version 0.1
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mutex>

class Unix_Lock
{
public:
    Unix_Lock(){};
    void acquire()
    {
        lock.lock();
    };
    void release()
    {
        lock.unlock();
    };

private:
    std::mutex lock;
};