#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

#include <libriccore/storage/appendrequest.h>

// This file is used to specify base types for general headers
namespace ThreadTypes {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;
    
    /**
     * @brief Provides a semaphore which checks if a thread has work, otherwise the thread yields/sleeps
     * 
     */
    class ThreadWorkSemaphore {
    public:
        void up() {
            counter = true;
        }
        void down() {
            while (!counter) {
                std::this_thread::yield(); // Do something else;
            }
            counter = false;
        }

        bool get() {
            return counter.load();
        }
    private:
        std::atomic_bool counter;
    };

    /**
     * @brief Threadsafe channel which manages unique ptrs
     * 
     * @tparam T 
     */
    template<typename T>
    class UniquePtrChannel {
    public:

        void send(std::unique_ptr<T> item) {
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
     
        bool empty() {
            auto l = std::scoped_lock(lock);
            return vec.empty();
        }
    
    private:
        std::mutex lock;
        std::queue<std::unique_ptr<T>> vec;
    };
}