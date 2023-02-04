#include <riccoresystem.h>
#include <riccorelogging.h>

enum class MOCK_SYSTEM_FLAGS:uint32_t{
    f1,
    f2,
    f3
};


class MockSystem : public RicCoreSystem<MockSystem,MOCK_SYSTEM_FLAGS>{
    public:
        void systemUpdate(){
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("derived system update called!");
        }
};





int main(){
    return 0;
}