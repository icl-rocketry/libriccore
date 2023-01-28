#pragma once

#include <unordered_map>

#include "rnp_packet.h"
#include "rnp_networkmanager.h"

#include "commandhandlertypes.h"

class stateMachine; //forward declaration

namespace RicCoreConfig{

enum class COMMAND_ID:uint8_t{
    Nocommand = 0
};

const CommandHandlerTypes::commandMap_t command_map = 
{
};

};





