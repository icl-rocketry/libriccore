#pragma once
#include <functional>
#include <unordered_map>

#include "rnp_packet.h"

class System;

namespace RicCoreConfig{
    enum class COMMAND_ID:uint8_t; //enum forward declaration to resolve command map type
};

namespace CommandHandlerTypes{
    using commandFunction_t = std::function<void(System &, const RnpPacketSerialized &)>;
    using commandMap_t = std::unordered_map<RicCoreConfig::COMMAND_ID,commandFunction_t>;

};