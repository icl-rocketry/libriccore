#pragma once
#include <string>
#include <functional>
#include <atomic>

#include <libriccore/platform/riccorethread_types.h>

namespace RicCoreThread
{
    /**
     * Thread manages creation and deletion
     * Once a thread object is constructed, it is launched.
     * When the destructor is called, the thread will also be deleted.
     * The Thread class automatically deals with differences in platform, so when running on the freeRTOS platform
     * on the esp32, the passed function pointer will be internally wrapped in a deleter to delete the task handle when
     * the function returns, so DONT include a deleter or you will get crashes!!!!!
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
         * @param f_ptr pointer to function to run inside thread.
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

        /**
         * @brief Join blocks the caller thread until this thread instance terminates
         *
         */
        void join();

    private:
        ThreadHandle_t handle;
        bool success; // not sure why this is here? -> need to clarify what exceptions thread construion will throw
        std::atomic<bool> deleted; //required to check if a freertos task has already been deleted.
    };

    /**
     * @brief Thread delay function
     *
     * @param ms
     */
    void delay(uint32_t ms);

    /**
     * @brief Block thread using the appropriate api
     *
     */
    void block();

};