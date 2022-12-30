#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

// This file is used to specify base types for general headers
namespace config {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;
    
    // Can replace this with a counting_semaphore in cpp20
    class SemaType {
    public:
        void up() {
            counter++;
        }
        void down() {
            while (counter == 0) {
                std::this_thread::yield(); // Do something else;
            }
            counter--;
        }
    private:
        std::atomic_int32_t counter;
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