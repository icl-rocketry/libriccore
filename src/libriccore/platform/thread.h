#pragma once
#include <string>
#include <functional>

#ifdef LIBRICCORE_UNIX
    #include "unix/threadtypes.h"
#else
    #include "esp32/threadtypes.h"
#endif

/**
 * Thread manages creation and deletion
 * Once a thread object is constructed, it is launched.
 * When the destructor is called, the thread will also be deleted
 */
class Thread
{
public:
    Thread(
        std::function<void(void*)>,
        void *args,
        size_t stack_size = 0,
        int priority = 0,
        std::string name = "");

    ~Thread();


private:
    ThreadTypes::ThreadHandleType handle;
    bool success;
};

class Lock
{
public:
    Lock();
    void acquire();
    void release();

private:
    ThreadTypes::LockType lock;
};

class ScopedLock
{
public:
    ScopedLock(Lock &l) : l(l)
    {
        l.acquire();
    }
    ~ScopedLock()
    {
        l.release();
    }

private:
    Lock &l;
};

// A thread safe queue
template <typename T>
class Channel
{
public:
    void send(T item)
    {
        channel.send(item);
    }

    void receive(T &dest)
    {
        channel.receive(dest);
    }

    bool empty()
    {
        return channel.empty();
    }

private:
    ThreadTypes::ChannelType<T> channel;
};

// This is a binary semaphore
class Semaphore
{
public:
    void up();
    void down();
    bool get();

private:
    ThreadTypes::SemaType sema;
};

void delay(uint32_t ms);