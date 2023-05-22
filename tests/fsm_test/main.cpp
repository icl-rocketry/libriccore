#include <unistd.h>

#include <libriccore/riccoresystem.h>
#include <libriccore/riccorelogging.h>

#include <libriccore/platform/unix/virtualserialport.h>

#include <libriccore/commands/commandhandler.h>

#include <libriccore/fsm/state.h>

enum class MOCK_SYSTEM_FLAGS:uint32_t{
    STATE_1 = (1<<0),
    STATE_2 = (1<<1),
    STATE_3 = (1<<2),
    STATE_4 = (1<<3),
    F1 = (1<<4),
    F2 = (1<<5),
    F3 = (1<<6),
    F4 = (1<<7),
    ERROR_SERIAL = (1<<8)
};


enum class MOCK_COMMAND_IDS:uint32_t{
    C1,
    C2,
    C3,
    C4
};

class MockSystem;

void command1(MockSystem& system, const RnpPacketSerialized& packet)
{
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("command1 called!");
}

void command2(MockSystem& system, const RnpPacketSerialized& packet)
{
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("command2 called!");
}

void command3(MockSystem& system, const RnpPacketSerialized& packet)
{
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("command3 called!");
}

void command4(MockSystem& system, const RnpPacketSerialized& packet)
{
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("command4 called!");
}

decltype(RicCoreSystem<MockSystem,MOCK_SYSTEM_FLAGS,MOCK_COMMAND_IDS>::commandhandler)::commandMap_t MOCK_COMMAND_MAP{
                                               {MOCK_COMMAND_IDS::C1,command1},
                                               {MOCK_COMMAND_IDS::C2,command2},
                                               {MOCK_COMMAND_IDS::C3,command3},
                                               {MOCK_COMMAND_IDS::C4,command4}};

VirtualSerialPort vsp1;




class MockSystem : public RicCoreSystem<MockSystem,MOCK_SYSTEM_FLAGS,MOCK_COMMAND_IDS>{

    // using RicCoreSystem_t = RicCoreSystem<MockSystem,MOCK_SYSTEM_FLAGS,MOCK_COMMAND_IDS>;
    public:

        MockSystem():
        RicCoreSystem(MOCK_COMMAND_MAP,{MOCK_COMMAND_IDS::C1,MOCK_COMMAND_IDS::C2,MOCK_COMMAND_IDS::C3},vsp1)
        {};

        void systemSetup();
        

        void systemUpdate(){
            // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("derived system update called!");
        }
};


MockSystem mocksystem = MockSystem();

class State1 : public State<MOCK_SYSTEM_FLAGS>
{
    public:
        State1();
        std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> update();
        ~State1();
};


class State4 : public State<MOCK_SYSTEM_FLAGS>
{
    public:
        State4():State(MOCK_SYSTEM_FLAGS::STATE_4,mocksystem.systemstatus){};
        std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> update(){return std::make_unique<State1>();};
        ~State4(){};
};

class State3 : public State<MOCK_SYSTEM_FLAGS>
{
    public:
        State3():State(MOCK_SYSTEM_FLAGS::STATE_3,mocksystem.systemstatus){};
        std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> update(){return std::make_unique<State4>();};
        ~State3(){};
};

class State2 : public State<MOCK_SYSTEM_FLAGS>
{
    public:
        State2():State(MOCK_SYSTEM_FLAGS::STATE_2,mocksystem.systemstatus){};
        std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> update(){return std::make_unique<State3>();};
        ~State2(){};
};


State1::State1():State(MOCK_SYSTEM_FLAGS::STATE_1,mocksystem.systemstatus){};
// std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> State1::update(){return std::make_unique<State2>();};
std::unique_ptr<State<MOCK_SYSTEM_FLAGS>> State1::update(){return nullptr;};
State1::~State1(){};



void MockSystem::systemSetup()
{
    //initialize statemchaine with initial state
    statemachine.initalize(std::make_unique<State1>());
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("system setup complete!");

}


int main(){
    mocksystem.coreSystemSetup();
    while (true){
        mocksystem.coreSystemUpdate();
        // sleep(1);
        // RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("tick");
        // std::cout<<std::endl;
        std::cout<<std::flush;
    }

}