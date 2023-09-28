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

#include <tuple>

#include "libriccore/util/istuple.h"

#include "loggerhandler_config_tweak.h"



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

        /**
         * @brief Returns logger objects inside the logger handler as its actual type. This allows
         *  function paramter deduction later.
         * 
         * @tparam NAME 
         * @return constexpr auto 
         */
        template<RicCoreLoggingConfig::LOGGERS NAME>
        constexpr auto& retrieve_logger(){
            return std::get<static_cast<int>(NAME)>(logger_list_tuple);
        }; 

        /**
         * @brief Call update handle on all registered loggers, This allows periodic tasks to be called in the 
         * underlying logger such as time dependant log flushing or automatic heartbeat logging. In the apply functor, we use 
         * auto& rather than LoggerBase* which means we retain flexibility in the type that the Logger class can be. This means
         * although the intention is to use normal dynamic polymorphism with loggers, static CRTP like inheritance is permitted,
         * the Logger class doesn't even need to explicity inherit from LoggerBase, all the logger class needs to do is provide 
         * a method named update.
         * 
         * @author K./
         * 
         */
        void update()
        {
            std::apply(
                [](auto &&...loggers) 
                {
                    (...,loggers.update());
                },
                logger_list_tuple);
        };

    private:
        // Interface class friendships
        friend struct ILoggerHandler;
        friend struct RicCoreLogging;
        
        /**
         * @brief Tuple containing configured loggers for the logger handler. Note this must be an inlined tuple.
         * 
         */
        decltype(RicCoreLoggingConfig::logger_list)& logger_list_tuple = RicCoreLoggingConfig::logger_list;
        //check config is of type tuple
        static_assert(RicCoreUtil::is_tuple<decltype(RicCoreLoggingConfig::logger_list)>::value, "Logger list is not a tuple!");

        /**
         * @brief Get the Instance Reference
         * 
         * @return LoggerHandler& 
         */
        static LoggerHandler& getInstance(){ 
            static LoggerHandler lg;
            return lg;
            };
        

        LoggerHandler(){};
        ~LoggerHandler(){};
        

};

