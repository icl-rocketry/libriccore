#pragma once
#include <thread>
#include <mutex>
#include <queue>

// This file is used to specify base types for general headers
namespace config {
    using ThreadHandleType = std::thread;
    using LockType = std::mutex;

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
    
    private:
        std::mutex lock;
        std::queue<T> vec;
    };
}