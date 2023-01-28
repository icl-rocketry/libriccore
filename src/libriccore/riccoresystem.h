
#pragma once

//From librnp
#include "rnp_networkmanager.h"

#include "fsm/statemachine.h"

#include "systemstatus/systemstatus.h"
// #include "systemflags_config_tweak.h" // config headers select correct tweak header

#include "logging/loggerhandler.h"

#include "util/isdetected.h"




template<typename DERIVED>
class RicCoreSystem{
    //checks if derived class is hiding main update loop
    static_assert(!is_detected<decltype(DERIVED::coreSystemUpdateLoop, DERIVED)>::value, "Dervied System re-implements underlying update loop!"); 

    public:
        RicCoreSystem():
        loggerhandler(ILoggerHandler::getInstance()),
        networkmanager()
        {};

        /**
         * @brief core system update loop, calls the derived update loop aswell. Don't hide this function in the derived class!
         * 
         */
        void coreSystemUpdateLoop(){
            networkmanager.update();
            static_cast<DERIVED*>(this)->systemUpdateLoop();
            statemachine.update();
        };

        /**
         * @brief Dervied system update loop, provides a function which is called each loop by the core system. Re-implement in child class
         * to implement desired update behaviour.
         * 
         */
        void systemUpdateLoop(){};


    protected:

        SystemStatus systemstatus;

        LoggerHandler& loggerhandler;

        RnpNetworkManager networkmanager;

        StateMachine statemachine;

        

    
};