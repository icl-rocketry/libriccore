/**
 * @file riccoretypes.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief This header provides a templated struct which provides convinent type aliases when working with core components.
 * Users should create a seperate header file, include this header and type alias with the template paramters filled in.
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <cstdint>
#include <memory>

#include "commands/commandhandler.h"
#include "systemstatus/systemstatus.h"
#include "fsm/state.h"

/**
 * @brief Templated struct with type aliases inside to provide convient type access. Some of the template paramters might require
 * forward declaration to prevent cylic dependancies.
 * 
 * @tparam SYSTEM_T type of derived system class. This will almost always be forward declared
 * @tparam SYSTEM_FLAGS_T Enum of system flags
 * @tparam COMMAND_ID_ENUM Enum of Command IDs
 */
template<typename SYSTEM_T,typename SYSTEM_FLAGS_T,typename COMMAND_ID_ENUM,size_t N_MAX_COMMANDS=256>
struct RicCoreTypes{
    using CommandHandler_t = CommandHandler<SYSTEM_T,COMMAND_ID_ENUM,N_MAX_COMMANDS>;
    using SystemStatus_t = SystemStatus<SYSTEM_FLAGS_T>;
    using State_t = State<SYSTEM_FLAGS_T>;
    using State_ptr_t = std::unique_ptr<State_t>;
};
