#pragma once
#include "tools/networking/enet_fwd.h"
#include "tools/networking/message.h"
#include "tools/networking/message_type.h"
#include "tools/networking/network_value.h"
#include "tools/networking/rpc_data.h"
#include <foundation/containers/deque.h>

namespace sulphur
{
  namespace networking
  {
    class NetworkingSystem;
    /**
    * @class sulphur::networking::PacketHandler
    * @brief Handles the packets
    * @author Joren Bolhuis
    */
    class PacketHandler
    {
    public:
      /**
      * @brief Constructor
      * @param[in] system (sulphur::networking::NetworkingSystem*) The main networking system
      */
      PacketHandler(NetworkingSystem* system);
      /**
      * @brief Destructor
      */
      ~PacketHandler();
      /**
      * @brief Send name function, used by the client
      * @param[in] message (const sulphur::networking::Message&) The message we want to send
      */
      void ClientSendName(const Message& message);
      /**
      * @brief Send name function, used by the host
      * @param[in] delivery (sulphur::networking::PacketDelivery) The type of delivery
      * @param[in] message (const sulphur::networking::Message&) The message we want to send
      */
      void HostSendName(PacketDelivery delivery, const Message& message);
      /**
      * @brief Sync name function, used by the host
      * @param[in] name (const char*) The name
      * @param[in] id (uint*_t) The id of the player we need to rename
      */
      void HostSyncName(const char* name, uint8_t id);
      /**
      * @brief Send id function, usable by host
      * @param[in] Message (const sulphur::networking::Message&) The message we want to send
      */
      void HostSendID(const Message& message);
      /**
      * @brief Send heartbeat function, usable by client and host
      * @param[in] peer (ENetPeer*) The peer we want to send our heartbeat to
      */
      void ClientHostSendHeartbeat(ENetPeer* peer);
      /**
      * @brief Add player sync function, only usable by a host
      * @param[in] peer (ENetPeer*) The peer we want to sync with
      */
      void HostAddPlayerSync(ENetPeer* peer);
      /**
      * @brief Add player function, only usable by a host
      * @param[in] id (uint8_t) The id of the player we want to add
      * @param[in] name (const char*) The name of the player we want to add
      * @param[in] peer (ENetPeer*) The peer of the player we want to add
      */
      void HostAddPlayer(uint8_t id, const char* name, ENetPeer* peer);
      /**
      * @brief Remove player from the list function, only usable by a host
      * @remarks Doesn't actually close the connection
      * @param[in] id (uint8_t) The id of the player we want to remove
      */
      void HostRemovePlayer(uint8_t id);
      /**
      * @brief Sync a value with the others
      * @param[in] id (uint16_t) The id of the value we are syncing
      * @param[in] value (const sulphur::networking::NetworkValue) The value we need to sync
      * @param[in] peer_to_exclude (ENetPeer*) The peer we could exclude from broadcasting our value
      * this occurs when a client sends a value to a host, the host needs to update everyone except the owner
      */
      void HostClientSyncValue(uint16_t id, const NetworkValue value, 
        ENetPeer* peer_to_exclude = nullptr);
      /**
      * @brief Send a validation message
      * @param[in] id (uint16_t) The id of the value we are validating
      * @param[in] type (const sulphur::networking::NetworkValueType) The type we need to check
      */
      void HostValidateValue(uint16_t id, NetworkValueType type);
      /**
      * @brief Send RPC to another machine
      * @param[in] id (uint16_t) The id of the rpc we are sending
      * @param[in] player_id (uint8_t) The id of the player (sender)
      * @param[in] rpc_mode (sulphur::networking::RPCMode) The mode we are gonna send
      * @param[in] arguments (const sulphur::foundation::Vector<NetworkValue>&) The arguments attached
      * @param[in] peer (ENetPeer*) The peer we want to exclude in RPCMode::KAll and RPCMode::kOthers
      */
      Message ClientHostSendRPC(uint16_t id, uint8_t player_id, RPCMode rpc_mode, 
        const foundation::Vector<NetworkValue>& arguments, ENetPeer* peer);
      /**
      * @brief Send RPC to another machine with a pre-created message
      * @param[in] msg (sulphur::networking::Message) The pre-created message
      */
      void ClientHostSendRPC(Message msg);
      /**
      * @brief Send a validation message
      * @param[in] id (uint16_t) The id of the rpc we are validating
      * @param[in] argument_types (const sulphur::foundation::Vector<sulphur::networking::NetworkValueType>&) The argument types of the rpc
      */
      void HostValidateRPC(uint16_t id, 
        const foundation::Vector<NetworkValueType>& argument_types);
      /**
      * @brief Process all incomming packets
      * @param[in] peer (ENetPeer*) The peer associated with the message
      * @param[in] packet_data (uint8_t*) The data that we recieved
      * @param[in] size (const size_t) The size of the data
      */
      void ProcessPacket(ENetPeer* peer, uint8_t* packet_data, 
        const size_t size);
      /**
      * @brief Dispatch the queued messages
      */
      void DispatchMessages();
    private:
      /**
      * @brief Process the incomming name message on the client side
      * @remarks Internal use only
      * @param[in] packet_data (uint8_t*) Pointer to the data we can start reading from
      */
      void ClientProcessName(uint8_t* packet_data);
      /**
      * @brief Process the incomming name message on the host side
      * @remarks Internal use only
      * @param[in] packet_data (uint8_t*) Pointer to the data we can start reading from
      * @param[in] peer (ENetPeer*) The peer associated
      */
      void HostProcessName(uint8_t* packet_data, ENetPeer* peer);
      /**
      * @brief Process the incomming id message on the client
      * @remarks Internal use only
      * @param[in] packet_data (uint8_t*) Pointer to the data we can start reading from
      */
      void ClientProcessID(uint8_t* packet_data);
      /**
      * @brief Process the incomming add player message on the client
      * @remarks Internal use only
      * @param[in] packet_data (uint8_t*) Pointer to the data we can start reading from
      */
      void ClientProcessAddPlayer(uint8_t* packet_data);
      /**
      * @brief Process the incomming heartbeat message on the client and host side
      * @remarks Internal use only
      * @param[in] peer (ENetPeer*) The peer associated
      */
      void ClientHostProcessHeartbeat(ENetPeer* peer);
      /**
      * @brief Process the incomming remove player message on the client side
      * @remarks Internal use only
      * @param[in] packet_data (uint8_t*) Pointer to the data we can start reading from
      */
      void ClientProcessRemovePlayer(uint8_t* packet_data);
      /**
      * @brief Construct byte array with AddPlayer data
      * @remarks Internal use only
      * @param[in] name (const char*) the name of the player
      * @param[in] id (uint8_t) the id of the player
      * @param[out] data (uint8_t*) Data we write to
      * @param[out] size (size_t*) Size of the data we must set
      */
      void ConstructAddPlayerData(const char* name, uint8_t id, uint8_t* data, 
        size_t* size);
      /**
      * @brief Send a packet
      * @remarks Internal use only
      * @param[in] packet (const Packet&) The packet we send
      * @return (bool) Whether sending has been successful
      */
      bool SendPacket(const Packet& packet);
      /**
      * @brief Send a packet
      * @remarks Internal use only
      * @param[in] message (const Message&) The message we want to queue
      * @param[in] type (const MessageDataType) The identifier of the message
      */
      void QueueMessage(const Message& message, const MessageDataType type);
      /**
      * @brief Send a packet, but to everyone
      * @remarks Internal use only
      * @param[in] message (const Message&) The message we want to queue
      * @param[in] type (const MessageDataType) The identifier of the message
      */
      void QueueBroadcastMessage(const Message& message, 
        const MessageDataType type);
      /**
      * @brief Send a packet to everyone except 1 client
      * @remarks Internal use only
      * @param[in] message (const Message&) The message we want to queue
      * @param[in] type (const MessageDataType) The identifier of the message
      * @param[in] peer (const ENetPeer*) The peer to exclude
      */
      void QueueBroadcastMessageExceptOne(const Message& message,
        const MessageDataType type, const ENetPeer* peer);
      /**
      * @brief Process a message
      * @remarks Internal use only
      * @param[in] data (uint8_t*) Pointer to the data we can start reading from
      * @param[in] type (const MessageDataType) The identifier of the message
      * @param[in] peer (ENetPeer*) The peer associated with the message
      * @return (size_t) the size of the processed message, 
      so we know the offset for the next message
      */
      size_t ProcessMessage(uint8_t* data, const MessageDataType type, 
        ENetPeer* peer);
      /**
      * @brief Create a packet
      * @remarks Internal use only
      * @param[in] packet (const Packet&) the data for the packet
      * @return (ENetPacket*) The created packet
      */
      ENetPacket* CreatePacket(const Packet& packet);
      NetworkingSystem* system_; //!<Pointer to the main networking system
      foundation::Deque<const QueuedMessage> message_queue_; //!<The queued messages
    };
  }
}