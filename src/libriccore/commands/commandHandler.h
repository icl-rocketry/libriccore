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

template <typename SYSTEM_T,
          typename COMMAND_ID_ENUM,
          const std::unordered_map<COMMAND_ID_ENUM, std::function<void(SYSTEM_T &, const RnpPacketSerialized &)>> COMMAND_MAP,
          uint256_t DEFAULT_ALWAYS_ENABLED_COMMANDS,
          uint8_t SERVICE_ID>

class CommandHandler : public RnpNetworkService
{
public: // public type defintions to make life a bit easier to get types for the command handler
    using commandFunction_t = std::function<void(SYSTEM_T &, const RnpPacketSerialized &)>;
    using commandMap_t = decltype(COMMAND_MAP);

public:
    CommandHandler(SYSTEM_T &sys) : RnpNetworkService(SERVICE_ID),
                                    _sys(sys),
                                    _commandMap(COMMAND_MAP),
                                    _enabledCommands(_alwaysEnabledCommands){};

    enum class PACKET_TYPES : uint8_t
    {
        SIMPLE = 0,
        MAGCAL = 10,
        MESSAGE_RESPONSE = 100,
        TELEMETRY_RESPONSE = 101
    };

    /**
     * @brief Enable Commands integral type template specialization, handles integer types
     * 
     * @tparam T 
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param command_list 
     */
    template<class T,typename std::enable_if<std::is_integral_v<T>, int>::type = 0>
    void enable_commands(std::initializer_list<T> command_list){
        enable_commands_IMPL<T,T>(command_list);
    };

    /**
     * @brief Enable Commands Enum type template specialization, handles only COMMAND_ID_ENUM type, any other enum will cause an assert
     * 
     * @tparam T 
     * @tparam std::enable_if<std::is_enum_v<T>, int>::type 
     * @param command_list 
     */
    template<class T,typename std::enable_if<std::is_enum_v<T>, int>::type = 0>
    void enable_commands(std::initializer_list<T> command_list){
        static_assert(std::is_same_v<T,COMMAND_ID_ENUM>,"Enum passed is not the same as the command id enum");
        using T_underlying = typename std::underlying_type_t<T>;
        enable_commands_IMPL<T,T_underlying>(command_list);
    };

    /**
     * @brief Disable Commands integral type template specialization, handles integer types
     * 
     * @tparam T 
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param command_list 
     */
    template<class T,typename std::enable_if<std::is_integral_v<T>, int>::type = 0>
    void disable_commands(std::initializer_list<T> command_list){
        disable_commands_IMPL<T,T>(command_list);
    };

    /**
     * @brief Disable Commands Enum type template specialization, handles only COMMAND_ID_ENUM type, any other enum will cause an assert
     * 
     * @tparam T 
     * @tparam std::enable_if<std::is_enum_v<T>, int>::type 
     * @param command_list 
     */
    template<class T,typename std::enable_if<std::is_enum_v<T>, int>::type = 0>
    void disable_commands(std::initializer_list<T> command_list){
        static_assert(std::is_same_v<T,COMMAND_ID_ENUM>,"Enum passed is not the same as the command id enum");
        using T_underlying = typename std::underlying_type_t<T>;
        disable_commands_IMPL<T,T_underlying>(command_list);
    };

    void reset_commands()
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
     * @brief Bitset to track enabled and disabled commmands
     *
     */
    std::bitset<256> _enabledCommands;

    /**
     * @brief Constant Bitset to track commands which should persist during resets of the enabled command bitset
     *
     */
    std::bitset<256> _persistentEnabledCommands{DEFAULT_ALWAYS_ENABLED_COMMANDS};

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
