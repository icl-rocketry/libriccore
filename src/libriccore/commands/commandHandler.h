// process commands stored in command buffer and execute

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <cstdarg>
#include <initializer_list>

#include "commandhandlertypes.h"
#include "commandhandler_config_tweak.h"


#include "rnp_packet.h"
#include "rnp_networkmanager.h"
#include "rnp_networkservice.h"
#include <default_packets/simplecommandpacket.h>


class stateMachine; // forward declaration
// typedef void (*handlerFunction)(const RnpPacketSerialized& packet);


class CommandHandler : public RnpNetworkService
{
public:
    CommandHandler(stateMachine *sm) : RnpNetworkService(serviceID),
                                       _sm(sm),
                                       _commandMap(RicCoreConfig::command_map),
                                       _enabledCommands(_alwaysEnabledCommands){};

    // this aint great, it forces only a sinlge command handler in the whole system
    // need to think of a better way to do this
    static constexpr uint8_t serviceID = static_cast<uint8_t>(DEFAULT_SERVICES::COMMAND); // serivce ID for network manager

    enum class PACKET_TYPES : uint8_t
    {
        SIMPLE = 0,
        MAGCAL = 10,
        MESSAGE_RESPONSE = 100,
        TELEMETRY_RESPONSE = 101
    };

    /**
     * @brief Enables a list of command ids to be executed
     *
     * @tparam T
     * @param command_list
     */
    template <class T>
    void enable_commands(std::initializer_list<T> command_list)
    {
        for (auto command_id : command_list)
        {
            _enabledCommands.set(command_id);
        }
    }

    /**
     * @brief Disables passed command id as long as it is not set in always enabled commands
     *
     * @tparam T
     * @param command_list
     */
    template <class T>
    void disable_commands(std::initializer_list<T> command_list)
    {
        for (auto command_id : command_list)
        {
            _enabledCommands.reset(command_id);
        }
        // ensure that _alwaysEnabledCommands arent disabled
        _enabledCommands |= _alwaysEnabledCommands;
    }

    void reset_commands()
    {
        _enabledCommands = _alwaysEnabledCommands;
    }

private:
    stateMachine *_sm; // pointer to state machine

    const CommandHandlerTypes::commandMap_t _commandMap;

    std::bitset<256> _enabledCommands;

    static constexpr std::bitset<256> _alwaysEnabledCommands{0b00000001};

    /**
     * @brief Process the recevied command packet
     *
     * @param packetptr
     */
    void handleCommand(packetptr_t packetptr)
    {

        command_t cmd = CommandPacket::getCommand(*packetptr);
        if (_enabledCommands.test(cmd))
        {
            _commandMap.at(static_cast<RicCoreConfig::COMMAND_ID>(cmd))(*_sm, *packetptr);
        }
    };

    /**
     * @brief Simply a wrapper for handleCommand function for readability
     *
     * @param packetptr
     */
    void networkCallback(packetptr_t packetptr) override
    {
        handleCommand(std::move(packetptr));
    };
};
