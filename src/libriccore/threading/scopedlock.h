#pragma once

#include <libriccore/platform/riccorethread_types.h>

namespace RicCoreThread
{
    class ScopedLock
    {
    public:
        ScopedLock(Lock_t &l) : l(l)
        {
            l.acquire();
        }
        ~ScopedLock()
        {
            l.release();
        }

    private:
        Lock_t &l;
    };
};