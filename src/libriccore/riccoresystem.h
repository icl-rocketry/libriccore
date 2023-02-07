
#pragma once

//From librnp
#include "rnp_networkmanager.h"
#include "rnp_default_address.h"
#include "rnp_nvs_save.h"

#include "fsm/statemachine.h"

#include "systemstatus/systemstatus.h"


#include "logging/loggerhandler.h"

#include "util/isdetected.h"




template<typename DERIVED,
         typename SYSTEM_FLAGS_T>
class RicCoreSystem{
    //checks if derived class is hiding main update loop
    static_assert(!is_detected<decltype(DERIVED::coreSystemUpdate, DERIVED)>::value, "Dervied System re-implements underlying update function!");
    static_assert(!is_detected<decltype(DERIVED::coreSystemSetup, DERIVED)>::value, "Dervied System re-implements underlying setup loop!");  

    public:
        RicCoreSystem():
        systemstatus(),
        loggerhandler(ILoggerHandler::getInstance()),
        networkmanager(static_cast<uint8_t>(DEFAULT_ADDRESS::NOADDRESS),NODETYPE::LEAF,true), //cant remember what happens if u intialize with address zero
        statemachine()
        {};

        /**
         * @brief Core System Setup, performs core system setup, default network manager intialization and finally 
         * calls the derived system setup functions. Intended to perform final setup for board communication busses,
         * assign network interfaces, load config into system etc... 
         * 
         */
        void coreSystemSetup(){
            networkManagerSetup();
            static_cast<DERIVED*>(this)->systemSetup();
        };

        void systemSetup(){};

        /**
         * @brief core system update loop, calls the derived update loop aswell. Don't hide this function in the derived class!
         * 
         */
        void coreSystemUpdate(){
            networkmanager.update();
            static_cast<DERIVED*>(this)->systemUpdate();
            statemachine.update();
        };

        /**
         * @brief Dervied system update loop, provides a function which is called each loop by the core system. Re-implement in child class
         * to implement desired update behaviour.
         * 
         */
        void systemUpdate(){};

        

    protected:

        SystemStatus<SYSTEM_FLAGS_T> systemstatus;

        LoggerHandler& loggerhandler;

        RnpNetworkManager networkmanager;

        StateMachine<SYSTEM_FLAGS_T> statemachine;
        
        /**
         * @brief Perform default network manager setup
         * 
         */
        void networkManagerSetup(){
            //register logging callback
            networkmanager.setLogCb([](const std::string& message){RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(message);});

            // register debug interface
            networkmanager.addInterface(&usbserial);

            //generate default network routes in the routing table
            networkmanager.generateDefaultRoutes();            

            //setup default config of network manager
            networkmanager.enableAutoRouteGen(false);
            networkmanager.setNoRouteAction(NOROUTE_ACTION::DUMP,{});
            
            // command handler callback
            networkmanager.registerService(static_cast<uint8_t>(DEFAULT_SERVICES::COMMAND),commandhandler.getCallback()); 
        
            
            //configure save function from network manager
            networkmanager.setSaveConfigImpl(RnpNvsSave::SaveToNVS);

            //try to load previous net config from nvs
            RnpNetworkManagerConfig savedNetworkConfig;
            if (!RnpNvsSave::ReadFromNVS(savedNetworkConfig))
            {
                RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Loading Saved Network Config!");
                networkmanager.loadconfig(savedNetworkConfig);
            }

        };  
        

    
};