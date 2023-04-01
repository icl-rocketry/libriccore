#pragma once
#include <string>
#include <config.h>

/**
 * Thread manages creation and deletion
 * Once a thread object is constructed, it is launched.
 * When the destructor is called, the thread will also be deleted
*/
class Thread {
public:
    Thread(
        void (*f)(void* args),
        void* args,
        size_t stack_size = 0,
        int priority = 0,
        std::string name = ""
    );

    ~Thread();

private:
    config::ThreadHandleType handle;
    bool success;
};

class Lock {
public:
    Lock();
    void acquire();
    void release();
private:
    config::LockType lock;
};

class ScopedLock {
public:
    ScopedLock(Lock& l) : l(l) {
        l.acquire();
    }
    ~ScopedLock() {
        l.release();
    }
private:
    Lock& l;
};

// A thread safe queue
template <typename T>
class Channel {
public:
    void send(T item) {
        channel.send(item);
    }
    
    void receive(T& dest) {
        channel.receive(dest);
    }

    bool empty() {
        return channel.empty();
    }
private:
    config::ChannelType<T> channel;
};

// This is a binary semaphore
class Semaphore {
public:
    void up();
    void down();
    bool get();
private:
    config::SemaType sema;
};

void delay(uint32_t ms);