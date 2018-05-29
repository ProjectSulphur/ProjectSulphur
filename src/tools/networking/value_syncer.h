#pragma once
#include "tools/networking/syncable_network_value.h"
#include "tools/networking/network_value_owner.h"
#include "tools/networking/enet_fwd.h"
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>
#include <foundation/containers/list.h>

namespace sulphur
{
  namespace networking
  {
    const unsigned int kValueStackSize = 32; //!<The size of a value stack, used to increase the pool

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
      * @param[in] networking_system (sulphur::networking::NetworkingSystem) The system
      * @param[in] packet_handler (sulphur::networking::PacketHandler) The packet handler
      */
      ValueSyncer(NetworkingSystem* networking_system, PacketHandler* packet_handler);
      /**
      * @brief Destructor
      */
      ~ValueSyncer();
      /**
      * @brief Create a syncable value
      * @param[in] type (sulphur::networking::NetworkValueType) The type of the value
      * @param[in] owner (sulphur::networking::NetworkValueOwner) The owner of the value
      * @return (sulphur::networking::SyncValueID) The id associated with the value
      */
      SyncValueID CreateValue(NetworkValueType type, NetworkValueOwner owner);
      /**
      * @brief Remove a value from the synclist
      * @param[in] id (sulphur::networking::SyncValueID) The id of the value
      */
      void RemoveValue(SyncValueID id);
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
      * @param[in] id (sulphur::networking::SyncValueID) The id we want to validate
      * @remarks This function should be used occasionally
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      * @remarks If something is invalid, it wont report back to the host. This is mainly because,
      sometimes in 1 frame, a value could possibly not be create yet.
      However if it keeps occuring something is wrong.
      */
      void ValidateValue(SyncValueID id);
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
      * @brief Set the value of a syncable value
      * @param[in] id (sulphur::networking::SyncValueID) The id of the syncvalue we want to set the value
      * @param[in] value (const NetworkValue&) The value
      */
      void SetValue(SyncValueID id, const NetworkValue& value);
      /**
      * @brief Get the value of a syncable value
      * @param[in] id (sulphur::networking::SyncValueID) The id of the syncvalue we want to get the value
      * @param[out] value (NetworkValue*) A pointer to a networkvalue
      * @return (bool) If getting the value succeeded
      */
      bool GetValue(SyncValueID id, NetworkValue* value);
    private:
      NetworkingSystem* networking_system_;//!<The networking system
      PacketHandler* packet_handler_;//!<The packet handler
      foundation::Vector<SyncableNetworkValue> value_stack_; //!<The values we need to sync
      foundation::Deque<SyncValueID> id_stack_;//!<The list of available ids
      foundation::Vector<SyncValueIDWithOwner> all_values_;//!<The list with ids that we need to sync
      /**
      * @brief Grow the stack of values and ids safely
      * @remarks Internal use only
      */
      void GrowStack();
      /**
      * @brief Request and unique ID, will grow the pool if needed
      * @remarks Internal use only
      * @return (sulphur::networking::SyncValueID) The id
      */
      SyncValueID RequestID();
      /**
      * @brief A check if the value is in use by the system, should prevent crashes on invalidations
      * @param[in] id (sulphur::networking::SyncValueID) The id
      * @remarks Internal use only
      * @return (sulphur::networking::SyncValueID) If the value is in use by the system
      */
      bool ValidateID(SyncValueID id);
      /**
      * @brief Create a default NetworkValue
      * @param[in] type (const sulphur::networking::NetworkValueType) The type
      * @remarks Internal use only
      * @return (const sulphur::networking::NetworkValue) the NetworkValue
      */
      NetworkValue DefaultNetworkValue(const NetworkValueType type);
    };
  }
}