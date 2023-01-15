
#pragma once


#include "fsm/statemachine.h"

#include "systemstatus/systemstatus.h"
#include "systemstatus/defaultsystemflags.h"

#include "util/isdetected.h"




template<typename DERIVED,
         typename SYSTEM_FLAGS_ENUM=DEFAULT_SYSTEM_FLAGS>
class RicCoreSystem{
    //checks if derived class is hiding main update loop
    static_assert(!is_detected<decltype(DERIVED::coreSystemUpdate, DERIVED)>::value, "Dervied System re-implements underlying update loop!"); 

    public:
        RicCoreSystem();

        /**
         * @brief main update loop called by freertos
         * 
         */
        void coreSystemUpdate();


    protected:

        SystemStatus<SYSTEM_FLAGS_ENUM> systemstatus;

        void 


    
};