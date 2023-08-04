#pragma once
#include <string>
#include <functional>


#ifdef LIBRICCORE_UNIX
    #include "unix/threadtypes.h"
#else
    // #include "esp32/threadtypes.h"
#endif

#include "unix/threadtypes.h"


/**
 * Thread manages creation and deletion
 * Once a thread object is constructed, it is launched.
 * When the destructor is called, the thread will also be deleted
 */
class Thread
{
public:
    /**
     * @brief Enum for core affinity
     * 
     */
    enum class CORE_ID:int{
        CORE0 = 0,
        CORE1 = 1,
        ANYCORE = 2
    };
public:
    /**
     * @brief Construct a new Thread object
     * 
     * @param f_ptr pointer to thread function 
     * @param args void pointer to function arguments
     * @param stack_size stack size [ignored for unix]
     * @param priority priority of running task [ignored for unix]
     * @param coreID id for core to run on, 2 for no core affinity [ignored for unix]
     * @param name name of thread
     */
    Thread(
        std::function<void(void*)> f_ptr,
        void *args,
        const size_t stack_size = 0,
        const int priority = 0,
        const CORE_ID coreID = CORE_ID::ANYCORE,
        std::string_view name = "");


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


void delay(uint32_t ms);