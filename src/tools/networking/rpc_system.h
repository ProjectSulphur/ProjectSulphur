#pragma once
#include "tools/networking/rpc_data.h"
#include "tools/networking/enet_fwd.h"
#include "tools/networking/message.h"
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>

namespace sulphur
{
  namespace networking
  {
    const unsigned int kRPCStackSize = 32;//!<The size of a rpc stack, used to increase the pool

    class PacketHandler;
    class NetworkingSystem;
    /**
    * @class sulphur::networking::RPCSystem
    * @brief Call RPCs
    * @author Joren Bolhuis
    */
    class RPCSystem
    {
    public:
      /**
      * @brief Constructor
      * @param[in] networking_system (sulphur::networking::NetworkingSystem) The system
      * @param[in] packet_handler (sulphur::networking::PacketHandler) The packet handler
      */
      RPCSystem(NetworkingSystem* networking_system, PacketHandler* packet_handler);
      /**
      * @brief Destructor
      */
      ~RPCSystem();
      /**
      * @brief Registers an rpc in the system
      * @param[in] function (eastl::function<void(sulphur::networking::RPCHandle, sulphur::networking::NetworkPlayerData, 
      sulphur::foundation::Vector<sulphur::networking::NetworkValue>)>) The function pointer
      * @param[in] arguments (sulphur::foundation::Vector<NetworkValueType>) The arguments
      * @return (sulphur::networking::RPCHandle) The handle associated with the handle
      */
      RPCHandle RegisterRPC(eastl::function<void(RPCHandle, NetworkPlayerData,
        const foundation::Vector<NetworkValue>&)> function, 
        const foundation::Vector<NetworkValueType>& arguments);
      /**
      * @brief Removes an rpc from the system
      * @param[in] handle (const sulphur::foundation::RPCHandle) The handle
      */
      void UnregisterRPC(const RPCHandle handle);
      /**
      * @brief Invokes an RPC on another machine(s)
      * @param[in] handle (const sulphur::foundation::RPCHandle) The handle
      * @param[in] mode (const sulphur::foundation::RPCMode) The mode
      * @param[in] arguments (sulphur::foundation::Vector<sulphur::networking::NetworkValue>) The arguments
      */
      void InvokeRPC(const RPCHandle handle, const RPCMode mode,
        const foundation::Vector<NetworkValue>& arguments);
      /**
      * @brief Process an rpc message
      * @param[in] data (uint8_t*) A pointer to the recieved data
      * @param[in] from (ENetPeer*) The sender of this value
      * @return (size_t) The size of the data array that we consumed
      */
      size_t ClientHostProcessRPC(uint8_t* data, ENetPeer* from);
      /**
      * @brief Validate an RPC
      * @param[in] data (uint8_t*) A pointer to the recieved data
      * @return (size_t) The size of the data array that we consumed
      */
      size_t ClientProcessRPCValidation(uint8_t* data);
      /**
      * @brief Validates an RPC on other clients
      * @param[in] id (uint16_t) The id we want to validate
      * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      */
      void ValidateRPC(uint16_t id);
      /**
      * @brief Validates all RPCs on other clients. Just as ValidateRPC does
      * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      */
      void ValidateAllRPCs();
      /**
      * @brief CleanUp the leftover calls and reset the id stack
      */
      void CleanUp();
      /**
      * @brief The ids that are not in use need to be sorted. This is mostly for the client after exiting a game,
      then connecting to a new one. The lifetime rpcs still exist, but the gameplay rpcs are gone, resulting in a weird id stack
      */
      void SortIDs();
      /**
      * @brief Clears the RPC buffer, usefull to call after a point where everybody is equal again (example: back to the lobby)
      */
      void ClearRPCBuffer();
      /**
      * @brief Send the buffer to a peer
      * @param[in] peer (ENetPeer*) The peer we want to send it to
      */
      void SendRPCBuffer(ENetPeer* peer);
    private:
      NetworkingSystem* networking_system_;//!<The networking system
      PacketHandler* packet_handler_;//!<The packet handler
      foundation::Vector<RPCData> rpc_stack_; //!<The registered RPCs
      foundation::Deque<uint16_t> id_stack_;//!<The list of available ids
      foundation::Vector<Message> rpc_buffer_;//!<The buffered RPC calls
      /**
      * @brief Grow the stack of values and ids safely
      * @remarks Internal use only
      */
      void GrowStack();
      /**
      * @brief Request and unique ID, will grow the pool if needed
      * @remarks Internal use only
      * @return (uint16_t) The id
      */
      uint16_t RequestID();
      /**
      * @brief A check if the rpc is in use by the system, should prevent crashes on invalidations
      * @param[in] id (uint16_t) The id
      * @remarks Internal use only
      * @return (bool) If the value is in use by the system
      */
      bool ValidateID(uint16_t id);
      /**
      * @brief A check if the arguments are valid with the function
      * @param[in] handle (const RPCHandle) The handle
      * @param[in] arguments (const sulphur::foundation::Vector<NetworkValue>&) The arguments
      * @remarks Internal use only
      * @return (bool) If the arguments are valid
      */
      bool ValidateArgumentsAgainstHandle(const RPCHandle handle, 
        const foundation::Vector<NetworkValue>& arguments);
    };
  }
}