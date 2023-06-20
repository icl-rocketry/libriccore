// process commands stored in command buffer and execute

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <cstdarg>
#include <initializer_list>


#include <librnp/rnp_networkservice.h>
#include <librnp/rnp_networkmanager.h>
#include <librnp/rnp_packet.h>


#include <librnp/default_packets/simplecommandpacket.h>

#include "libriccore/util/bitsethelpers.h"


template <typename SYSTEM_T,
          typename COMMAND_ID_ENUM,
          size_t N_MAX_COMMANDS = 256>
class CommandHandler : public RnpNetworkService
{
    static_assert(std::is_enum_v<COMMAND_ID_ENUM>, "COMMAND_ID_ENUM template paramter not an enum!");

//type aliases
public: // public type defintions to make life a bit easier to get types for the command handler
    using commandFunction_t = std::function<void(SYSTEM_T &, const RnpPacketSerialized &)>;
    using commandMap_t = std::unordered_map<COMMAND_ID_ENUM, commandFunction_t>;
private: 
    using bitset_t = std::bitset<N_MAX_COMMANDS>;

public:

    /**
     * @brief Command handler constructor with default intialization for the default persistent enabled commands
     * 
     * @author Kiran de Silva
     * 
     * @param sys referece to the derived system object to allow commands to access system objects
     * @param commandMap unordered map from the command id to the function call back for a given command
     * @param ServiceID network service ID
     */
    CommandHandler(SYSTEM_T &sys,commandMap_t commandMap,const uint8_t ServiceID) : 
                        RnpNetworkService(ServiceID),
                        _sys(sys),
                        _commandMap(commandMap),
                        _defaultPersistentEnabledCommands(0),
                        _persistentEnabledCommands(_defaultPersistentEnabledCommands),
                        _enabledCommands(_persistentEnabledCommands){};

    /**
     * @brief  Command handler constructor with support for intializing the default persisnent enabled command bitset.
     * 
     * @author Kiran de Silva
     * 
     * @tparam T type of element of intializer list
     * @param sys reference to the dervied system object
     * @param commandMap unordered map from the command id to the function call back for a given command
     * @param ServiceID network service ID
     * @param defaultPersistCommands intializer list of command ids to always be enabled during system lifetime
     */
    template <class T>
    CommandHandler(SYSTEM_T &sys,commandMap_t commandMap,const uint8_t ServiceID,const std::initializer_list<T> defaultPersistCommands) : 
                                                        RnpNetworkService(ServiceID),
                                                        _sys(sys),
                                                        _commandMap(commandMap),
                                                        _defaultPersistentEnabledCommands(RicCoreUtil::BitsetHelpers::generateBitset<N_MAX_COMMANDS>(defaultPersistCommands)),
                                                        _persistentEnabledCommands(_defaultPersistentEnabledCommands),
                                                        _enabledCommands(_persistentEnabledCommands)
                                                        {};

    /**
     * @brief Commonly used packet types for the command handling service.
     * 
     */
    enum class PACKET_TYPES : uint8_t
    {
        SIMPLE = 0,
        MAGCAL = 10,
        MESSAGE_RESPONSE = 100,
        TELEMETRY_RESPONSE = 101
    };

    /**
     * @brief Enable specified commands
     * 
     * @author Kiran de Silva
     * 
     * @tparam T Type of the list element, can be integral or enum type, however enum type must match COMMAND_ID_ENUM type
     * @param command_ids initializer list of command ids to be enabled
     * @param persist enable command and persist throught command bit field resets
     */
    template<class T>
    void enableCommands(const std::initializer_list<T> command_ids,bool persist = false)
    {
        static_assert(std::is_integral_v<T> || (std::is_enum_v<T> && std::is_same_v<T,COMMAND_ID_ENUM>),"Enum Type not the same as COMMAND_ID_ENUM template type!");
        RicCoreUtil::BitsetHelpers::setBits(_enabledCommands,command_ids);
        if (persist){ 
            // if we require the command to always be enabled, also set the bit in the persistent Enabled Commands bitset
            RicCoreUtil::BitsetHelpers::setBits(_persistentEnabledCommands,command_ids);
        }
    }

    /**
     * @brief Disable specified commands  
     * 
     * @author Kiran de Silva
     * 
     * @tparam T Type of the list element, can be integral or enum type, however enum type must match COMMAND_ID_ENUM type
     * @param command_ids initializer list of command ids to be enabled
     * @param persist set true to disable a command which has been enabled persistently
     */
    template<class T>
    void disableCommands(const std::initializer_list<T> command_ids, bool persist = false)
    {
        static_assert(std::is_integral_v<T> || (std::is_enum_v<T> && std::is_same_v<T,COMMAND_ID_ENUM>),"Enum Type not the same as COMMAND_ID_ENUM template type!");
        RicCoreUtil::BitsetHelpers::resetBits(_enabledCommands,command_ids);
        
        if (persist){ 
            // if we want to reset a persistently enabled command, we must also reset the command id in the persistent enabled command bitset
            RicCoreUtil::BitsetHelpers::resetBits(_persistentEnabledCommands,command_ids);
        }

        //ensure that persistent enabled commands do not get reset
        _enabledCommands |= _persistentEnabledCommands;
        
    }

    /**
     * @brief Reset commands to the persistent enabled commands
     * 
     * @author Kiran de Silva
     * 
     */
    void resetCommands()
    {
        _enabledCommands = _persistentEnabledCommands;
    }

    /**
     * @brief Reset persistent enabled commands to the default persistent enabled commands
     * 
     * @author Kiran de Silva
     * 
     */
    void resetPersistentCommands()
    {
        _persistentEnabledCommands = _defaultPersistentEnabledCommands;
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


};
