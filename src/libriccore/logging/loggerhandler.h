/**
 * @file loggerhandler.h
 * @author Kiran de Silva
 * @brief 
 * @version 0.1
 * @date 2023-01-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "loggerhandler_config.h"

/**
 * @brief Logger Handler Singleton. This provides the core logging framework targetting two main requirements for 
 * embedded logging. Firstly ensuring we have an easily accessible logging interface to produce logs (found in RicCoreLogging),
 * reducing code noise typically assosicated with a DI style logging framework, producing logs should be as painless as possible. 
 * Secondly providing a DI interface to logger handler management functions, e.g the logger initialize functions etc. This adds back 
 * the encalsupation which is normally lost when creating singletons ensuring that access to these functions is well managed and traceable.
 *  The general structre of this singleton follows the meyer singleton with the key difference of making the getInstance method private.
 * Access to this function is managed thru friend interface classes ensuring that only the RicCoreSystem class has access to the getInstance 
 * method. The reference returned can then be DI'ed to whatever management objects or methods may exist in the System class.
 * 
 */

class LoggerHandler{

    public:
        //Copy and Move constructors are deleted publically to make debugging less of a pain
        LoggerHandler(LoggerHandler const&) = delete;
        void operator=(LoggerHandler const&)  = delete;

    
    private:
        // Interface class friendships
        friend class ILoggerHandler;
        friend class RicCoreLogging;
        
        
        const decltype(logger_list) config = logger_list;

        /**
         * @brief Get the Instance Reference
         * 
         * @return LoggerHandler& 
         */
        static LoggerHandler& getInstance(){ 
            static LoggerHandler lg;
            return lg;
            };


        /**
         * @brief Returns logger objects inside the logger handler as its actual type. This allows
         *  function paramter deduction later.
         * 
         * @tparam NAME 
         * @return constexpr auto 
         */
        template<LOGGER NAME>
        constexpr auto retrieve_logger(){
            return std::get<static_cast<int>(NAME)>(config);
        }; 

        LoggerHandler(){};
        ~LoggerHandler(){};
        
        



};

/**
 * @brief Interface to allow RicCoreSystem classes retrieve the instance of the logger handler singleton
 *  This enforces a DI interface for anything that wants to do more than just use the loggers i.e stopping all logging etc...
 * 
 */
struct ILoggerHandler{
    private:
        template<typename PLACEHOLDER> //placeholder required as RicCoreSystem is a template
        friend class RicCoreSystem;

        static LoggerHandler& getInstance(){
            return LoggerHandler::getInstance();
        }
};