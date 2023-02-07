// process commands stored in command buffer and execute

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <cstdarg>
#include <initializer_list>


#include "rnp_packet.h"
#include "rnp_networkmanager.h"
#include "rnp_networkservice.h"
#include <default_packets/simplecommandpacket.h>

#include "util/bitsethelpers.h"

template <typename SYSTEM_T,
          typename COMMAND_ID_ENUM,
          uint8_t SERVICE_ID,
          size_t N_MAX_COMMANDS = 256>

class CommandHandler : public RnpNetworkService
{
    static_assert(std::is_enum_v<COMMAND_ID_ENUM>, "COMMAND_ID_ENUM template paramter not an enum!");
public: // public type defintions to make life a bit easier to get types for the command handler
    using commandFunction_t = std::function<void(SYSTEM_T &, const RnpPacketSerialized &)>;
    using commandMap_t = std::unordered_map<COMMAND_ID_ENUM, commandFunction_t>>;
private: 
    using bitset_t = std::bitset<N_MAX_COMMANDS>;

public:

    CommandHandler(SYSTEM_T &sys,commandMap_t commandMap) : 
                        RnpNetworkService(SERVICE_ID),
                        _sys(sys),
                        _commandMap(commandMap),
                        _defaultPersistentEnabledCommands(0),
                        _persistentEnabledCommands(_defaultPersistentEnabledCommands),
                        _enabledCommands(_persistentEnabledCommands){};

    template <class T>
    CommandHandler(SYSTEM_T &sys,commandMap_t commandMap, std::initializer_list<T> defaultPersistCommands) : 
                                                        RnpNetworkService(SERVICE_ID),
                                                        _sys(sys),
                                                        _commandMap(commandMap),
                                                        _defaultPersistentEnabledCommands(BitsetHelpers::generateBitset<N_MAX_COMMANDS>(defaultPersistCommands)),
                                                        _persistentEnabledCommands(_defaultPersistentEnabledCommands),
                                                        _enabledCommands(_persistentEnabledCommands){};
    {};

    enum class PACKET_TYPES : uint8_t
    {
        SIMPLE = 0,
        MAGCAL = 10,
        MESSAGE_RESPONSE = 100,
        TELEMETRY_RESPONSE = 101
    };

    template<class T>
    void enableCommands(std::initializer_list<T> command_ids)
    {
        static_assert(std::is_integral_v<T> || (std::is_enum_v<T> && std::is_same_v<T,COMMAND_ID_ENUM>),"Enum Type not the same as COMMAND_ID_ENUM template type!");
        BitsetHelpers::setBits(_enabledCommands,command_ids);
    }

    template<class T>
    void disableCommands(std::initializer_list<T> command_ids)
    {
        static_assert(std::is_integral_v<T> || (std::is_enum_v<T> && std::is_same_v<T,COMMAND_ID_ENUM>),"Enum Type not the same as COMMAND_ID_ENUM template type!");
        BitsetHelpers::resetBits(_enabledCommands,command_ids);
        //ensure that persistent enabled commands do not get reset
        _enabledCommands |= _persistentEnabledCommands;
    }

    void resetCommands()
    {
        _enabledCommands = _alwaysEnabledCommands;
    }


    

private:
    /**
     * @brief Reference to the system class to access objects within the system
     *
     */
    SYSTEM_T &_sys;

    /**
     * @brief Command map which maps the given command id to the releveant function callback
     *
     */
    const commandMap_t _commandMap;

    /**
     * @brief Constant Bitset to store the default persistent enabled commands
     * 
     */
    const bitset_t _defaultPersistentEnabledCommands;

    /**
     * @brief Bitset to track commands which should persist during resets of the enabled command bitset
     *
     */
    bitset_t _persistentEnabledCommands;

    /**
     * @brief Bitset to track enabled and disabled commmands
     *
     */
    bitset_t _enabledCommands;

    /**
     * @brief Process the recevied command packet
     *
     * @param packetptr
     */
    void handleCommand(packetptr_t packetptr)
    {
        
        //Note Command_t is set in the simplecommmandpacket header (librnp) and is considerd to be constant
        //throughout ricardo avionics
        command_t cmd = CommandPacket::getCommand(*packetptr); 
        if (_enabledCommands.test(cmd))
        {
            _commandMap.at(static_cast<COMMAND_ID_ENUM>(cmd))(_sys, *packetptr);
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

    /**
     * @brief Enables a list of command ids to be executed
     *
     * @tparam T1 T2
     * @param command_list
     */
    template <class T1,class T2>
    void enable_commands_IMPL(std::initializer_list<T1> command_list)
    {
        for (auto command_id : command_list)
        {
            _enabledCommands.set(static_cast<T2>command_id);
        }
    }

    /**
     * @brief Disables passed command id as long as it is not set in always enabled commands
     *
     * @tparam T1 T2
     * @param command_list
     */
    template <class T1, class T2>
    void disable_commands_IMPL(std::initializer_list<T1> command_list)
    {
        for (auto command_id : command_list)
        {
            _enabledCommands.reset(static_cast<T2>(command_id));
        }
        // ensure that _alwaysEnabledCommands arent disabled
        _enabledCommands |= _alwaysEnabledCommands;
    }


};
