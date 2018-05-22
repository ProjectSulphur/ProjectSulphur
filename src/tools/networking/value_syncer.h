#pragma once
#include "tools/networking/network_value.h"
#include "tools/networking/value_owner.h"
#include "tools/networking/enet_fwd.h"
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>
#include <foundation/containers/list.h>

namespace sulphur
{
  namespace networking
  {
    const unsigned int kValueStackSize = 32;//!<The size of a value stack, used to increase the pool

    class PacketHandler;
    class NetworkingSystem;
    /**
    * @class sulphur::networking::ValueSyncer
    * @brief Syncs values over network
    * @author Joren Bolhuis
    */
    class ValueSyncer
    {
    public:
      /**
      * @brief Constructor
      */
      ValueSyncer(NetworkingSystem* networking_system, PacketHandler* packet_handler);
      /**
      * @brief Destructor
      */
      ~ValueSyncer();
      /**
      * @brief Create a syncable value
      * @param[in] type (sulphur::networking::NetworkValueType) The type of the value
      * @param[in] location (void*) The location of the value in the engine
      * @param[in] owner (sulphur::networking::NetworkValueOwner) The owner of the value
      * @return (uint16_t) The id associated with the value
      */
      uint16_t CreateValue(NetworkValueType type, void* location, NetworkValueOwner owner);
      /**
      * @brief Remove a value from the synclist
      * @param[in] id (uint16_t) The id of the value
      */
      void RemoveValue(uint16_t id);
      /**
      * @brief Sync the proper values
      */
      void Sync();
      /**
      * @brief Sync a value in the system
      * @param[in] data (uint8_t*) A pointer to the recieved data
      * @param[in] from (ENetPeer*) The sender of this value
      */
      size_t ClientHostProcessValueSync(uint8_t* data, ENetPeer* from);
      /**
      * @brief Validate a value in the system
      * @param[in] data (uint8_t*) A pointer to the recieved data
      */
      void ClientProcessValueValidation(uint8_t* data);
      /**
      * @brief Validates a value on other clients
      * @param[in] id (uint8_t) The id we want to validate
      * @remarks This function should be used occasionally
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      * @remarks If something is invalid, it wont report back to the host. This is mainly because,
      sometimes in 1 frame, a value could possibly not be create yet.
      However if it keeps occuring something is wrong.
      */
      void ValidateValue(uint16_t id);
      /**
      * @brief Validates all values on other clients. Just as ValidateValue does
      * @remarks This function should be used occasionally
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      * @remarks If something is invalid, it wont report back to the host. This is mainly because,
      sometimes in 1 frame, a value could possibly not be create yet.
      However if it keeps occuring something is wrong.
      */
      void ValidateAllValues();
    private:
      NetworkingSystem * networking_system_;//!<The networking system
      PacketHandler* packet_handler_;//!<The packet handler
      foundation::Vector<SyncableNetworkValue> value_stack_; //!<The values we need to sync
      foundation::Deque<uint16_t> id_stack_;//!<The list of available ids
      foundation::Vector<uint16_t> our_values_;//!<The list with ids that we need to sync
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
      * @brief A check if the value is in use by the system, should prevent crashes on invalidations
      * @param[in] id (uint16_t id) The id
      * @remarks Internal use only
      * @return (bool) If the value is in use by the system
      */
      bool ValidateID(uint16_t id);
      /**
      * @brief Check if the value needs syncing
      * @param[in] nv (const sulphur::networking::SyncableNetworkValue&) The value
      * @remarks Internal use only
      * @return (bool) If we need to sync this value
      */
      bool NeedsUpdate(const SyncableNetworkValue& nv);
      /**
      * @brief Create a NetworkValue from location and type
      * @param[in] location (void*) The location
      * @param[in] type (const sulphur::networking::NetworkValueType) The type
      * @remarks Internal use only
      * @return (const sulphur::networking::NetworkValue) the NetworkValue
      */
      NetworkValue NetworkValueFromLocation(void* location,
        const NetworkValueType type);
    };
  }
}