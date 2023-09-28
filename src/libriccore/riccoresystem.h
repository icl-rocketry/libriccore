
#pragma once

//From librnp
#include <librnp/rnp_networkmanager.h>
#include <librnp/rnp_default_address.h>
#include <librnp/rnp_nvs_save.h>

#include "networkinterfaces/serial/streamserial.h"

#include "fsm/statemachine.h"

#include "systemstatus/systemstatus.h"

#include "commands/commandhandler.h"

#include "logging/loggerhandler.h"
#include "logging/iloggerhandler.h"

#include "util/isdetected.h"

// #include "platform/HardwareSerial.h"
#include <iostream>

template<typename DERIVED,
         typename SYSTEM_FLAGS_T,
         typename COMMAND_ID_ENUM>
class RicCoreSystem{
    //checks if derived class is hiding main update loop
    // static_assert(!RicCoreUtil::is_detected<decltype(DERIVED::coreSystemUpdate), DERIVED>::value, "Dervied System re-implements underlying update function!");
    // static_assert(!RicCoreUtil::is_detected<decltype(DERIVED::coreSystemSetup), DERIVED>::value, "Dervied System re-implements underlying setup loop!");  

    public:
        RicCoreSystem(typename CommandHandler<DERIVED,COMMAND_ID_ENUM,256>::commandMap_t commandmap,
                      const std::initializer_list<COMMAND_ID_ENUM> defaultEnabledCommands,
                      Stream &usbDebugPort):
        systemstatus(),
        loggerhandler(ILoggerHandler::getInstance()),
        networkmanager(254,NODETYPE::LEAF,true), //cant remember what happens if u intialize with address zero
        usb0(usbDebugPort,systemstatus,static_cast<uint8_t>(DEFAULT_INTERFACES::USBSERIAL),"usb0"),
        commandhandler(*(static_cast<DERIVED*>(this)),commandmap,static_cast<uint8_t>(DEFAULT_SERVICES::COMMAND),defaultEnabledCommands),
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
        
        /**
         * @brief Derived system setup method.
         * 
         */
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


        SystemStatus<SYSTEM_FLAGS_T> systemstatus;

        LoggerHandler& loggerhandler;

        RnpNetworkManager networkmanager;

        StreamSerial<SYSTEM_FLAGS_T> usb0;

        CommandHandler<DERIVED,COMMAND_ID_ENUM,256> commandhandler;

        StateMachine<SYSTEM_FLAGS_T> statemachine;


    protected:

        
        /**
         * @brief Perform default network manager setup
         * 
         */
        void networkManagerSetup(){
            //register logging callback
            networkmanager.setLogCb([](const std::string& message){RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(message);});

            // register debug interface
            networkmanager.addInterface(&usb0);

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
                RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Loading saved Network Config!");
                networkmanager.loadconfig(savedNetworkConfig);
            }else
            {
                RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Failed to load Network Config!");
            }

        };  
        

    
};