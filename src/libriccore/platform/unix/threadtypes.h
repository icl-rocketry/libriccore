#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

// This file is used to specify base types for general headers
namespace ThreadTypes {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;
    
    // Can replace this with a binary_semaphore in cpp20
    class SemaType {
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

    // We don't have a thread safe queue in STL apparently
    // Cba with fine grained locking so here we go
    template <typename T>
    class ChannelType {
    public:
        void send(T item) {
            auto l = std::scoped_lock(lock);
            vec.push(item);
        }
        void receive(T& dest) {
            auto l = std::scoped_lock(lock);
            dest = std::move(vec.front());
            vec.pop();
        }
        bool empty() {
            auto l = std::scoped_lock(lock);
            return vec.empty();
        }
    
    private:
        std::mutex lock;
        std::queue<T> vec;
    };
}