#pragma once
#include <string>
#include <functional>

#ifdef LIBRICCORE_UNIX
#include "unix/riccorethread_types.h"
#else
// #include "esp32/riccorethread_types.h"
#endif

#include "unix/riccorethread_types.h"

namespace RicCoreThread
{
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
        enum class CORE_ID : int
        {
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
            std::function<void(void *)> f_ptr,
            void *args,
            const size_t stack_size = 0,
            const int priority = 0,
            const CORE_ID coreID = CORE_ID::ANYCORE,
            std::string_view name = "");

        ~Thread();

    private:
        ThreadHandle_t handle;
        bool success;
    };

    class Lock
    {
    public:
        Lock();
        void acquire();
        void release();

    private:
        Lock_t lock;
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

    /**
     * @brief Thread delay function
     * 
     * @param ms 
     */
    void delay(uint32_t ms);

    /**
     * @brief Provides a semaphore which checks if a thread has work, otherwise the thread yields/sleeps
     *
     */
    class ThreadWorkSemaphore
    {
    public:
        /**
         * @brief Signal semaphore up
         * 
         */
        void up();
        /**
         * @brief Idle thread until semaphore is up, thread yielding is platform specific.
         * 
         */
        void waitForWork();
        /**
         * @brief Signal semaphore down
         * 
         */
        void down();

        /**
         * @brief Get current state of semaphore
         * 
         * @return true 
         * @return false 
         */
        bool get();

    private:
        std::atomic_bool counter;
    };

};