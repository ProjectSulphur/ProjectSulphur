#pragma once
/**
* @def PS_NETWORKING_API
* @brief exporting macro for dll
*/
#ifdef PS_NETWORKING_API_EXPORT
#define PS_NETWORKING_API __declspec(dllexport) 
#else  
#define PS_NETWORKING_API __declspec(dllimport)
#endif  

/**
* @def PS_NETWORKING_C_API
* @brief exporting macro for dll using extern C
*/
#ifdef PS_NETWORKING_API_EXPORT
#define PS_NETWORKING_C_API extern "C" __declspec(dllexport) 
#else  
#define PS_NETWORKING_C_API extern "C" __declspec(dllimport)
#endif  
#include "tools/networking/connection_state.h"
#include "tools/networking/network_player.h"
#include "tools/networking/network_value.h"
#include "tools/networking/network_value_owner.h"
#include "tools/networking/disconnection_info.h"
#include "tools/networking/rpc_data.h"
#include "tools/networking/syncable_network_value.h"
#include <foundation/containers/vector.h>

#ifdef __cplusplus
#include <cinttypes>
#else
#include <inttypes.h>
#endif

namespace sulphur
{
  namespace networking
  {
    static class NetworkingSystem* network_manager_ = nullptr; //!< Network system, for internal use only
    /**
    * @brief constructs the engine networking system
    * @remarks This functions needs to be called to construct a network instance, 
    because it lives inside a dll
    */
    PS_NETWORKING_API void SNetConstruct(size_t size);
    /**
    * @brief destructs the engine networking system
    * @remarks This functions needs to be called to construct a network instance, 
    because it lives inside a dll
    */
    PS_NETWORKING_API void SNetDestruct();
    /**
    * @brief Initialises the networking system
    * @return (bool) Whether the initialising has succeeded
    */
    PS_NETWORKING_API bool SNetInitSystem();
    /**
    * @brief Destroys the networking system
    */
    PS_NETWORKING_API void SNetDestroySystem();
    /**
    * @brief Create a host version for your networking setup
    * @param[in] port (unsigned int) The port opened by the host
    * @param[in] max_client (unsigned int) Maximum amount of clients the host allows
    * @param[in] player_name (const sulphur::foundation::String&) The name of the host, 
    used in the playerlist
    * @return (bool) Whether the initialising has succeeded
    */
    PS_NETWORKING_API bool SNetInitHost(unsigned int port, 
      unsigned int max_clients, const foundation::String& player_name);
    /**
    * @brief Create a client version for your networking setup
    * @param[in] name (const sulphur::foundation::String&) The name of the client, 
    used in the playerlist
    * @return (bool) Whether the initialising has succeeded
    */
    PS_NETWORKING_API bool SNetInitClient(const foundation::String& name);
    /**
    * @brief Destruction of the host or client initialisation
    * @return (bool) Whether the destruction has succeeded
    */
    PS_NETWORKING_API bool SNetDestroyHostOrClient();
    /**
    * @brief Updating the network system, should be called every frame
    * @return (bool) Whether the update has succeeded
    */
    PS_NETWORKING_API bool SNetUpdate(float deltatime);
    /**
    * @brief Late Updating the network system, mainly for sending the packets we created, 
    should be called every frame
    * @return (bool) Whether the update has succeeded
    */
    PS_NETWORKING_API bool SNetLateUpdate();
    /**
    * @brief Connecting to a host
    * @param[in] ip (const sulphur::foundation::String&) The ip we want to connect to
    * @param[in] port (unsigned int) The port we want to connect to
    * @remarks Function should only be called by a client
    * @remarks Client is not immediately connected but is put in a connecting state, 
    if connection succeeds, the state will but put into connected
    * @return (bool) Whether the connection call has succeeded
    */
    PS_NETWORKING_API bool SNetConnect(const foundation::String& ip, 
      unsigned int port);
    /**
    * @brief Disconnect from host
    * @return (bool) Whether the connection call has succeeded
    */
    PS_NETWORKING_API bool SNetDisconnect();
    /**
    * @brief Change the name of the client, 
    can be used anytime since it will be synced by hosts and clients
    * @param[in] name (const sulphur::foundation::String&) The name we want to use
    */
    PS_NETWORKING_API void SNetSetName(const foundation::String& name);
    /**
    * @brief Get the name of the client
    * @return (foundation::String) name
    */
    PS_NETWORKING_API foundation::String SNetGetName();
    /**
    * @brief Get the id of the client
    * @return (unsigned int) id
    */
    PS_NETWORKING_API unsigned int SNetGetID();
    /**
    * @brief Check whether we are connected
    * @return (bool) are we connected?
    */
    PS_NETWORKING_API bool SNetIsConnected();
    /**
    * @brief Check whether we are a host
    * @return (bool) are we a host?
    */
    PS_NETWORKING_API bool SNetIsHost();
    /**
    * @brief Check whether we are a client
    * @return (bool) are we a client?
    */
    PS_NETWORKING_API bool SNetIsClient();
    /**
    * @brief Check the amount of players connected
    * @remarks this includes yourself, as you are a player too, 
    no matter if you are a host or a client
    * @return (unsigned int) amount of players
    */
    PS_NETWORKING_API unsigned int SNetGetNumClients();
    /**
    * @brief Recieve a networkplayer list
    * @remark Type is NetworkPlayerData as the main struct should not be exposed
    * @return (foundation::Vector<NetworkPlayerData>) List of NetworkPlayerData
    */
    PS_NETWORKING_API foundation::Vector<NetworkPlayerData> 
      SNetGetNetworkPlayers();
    /**
    * @brief Retrieves the connection setup (host, client or not initialised)
    * @return (sulphur::networking::ConnectionSetup) the connection setup
    */
    PS_NETWORKING_API ConnectionSetup SNetConnectionSetup();
    /**
    * @brief Retrieves the connection status 
    (disconnected, connecting, connected or disconnecting)
    * @remarks If we are a host, it will return connected
    * @return (sulphur::networking::ConnectionStatus) the connection status
    */
    PS_NETWORKING_API ConnectionStatus SNetConnectionStatus();
    /**
    * @brief Kick a client, only works if we are a host. We cannot kick ourselves
    * @param[in] id (uint8_t) The client that will be kicked.
    * @return (bool) Whether the kick succeeded.
    */
    PS_NETWORKING_API bool SNetKickClient(uint8_t id);
    /**
    * @brief Set the time after we should forcequit our connection
    * @param[in] timeout (unsigned int) The timeout in miliseconds
    */
    PS_NETWORKING_API void SNetSetConnectionTimeout(unsigned int timeout);
    /**
    * @brief Get the value for the connection timeout
    * @return (bool) The connection timeout
    */
    PS_NETWORKING_API unsigned int SNetGetConnectionTimeout();
    /**
    * @brief Set the tickrate of the networking system
    * @remarks An value that is too low will result in an inacurate simulation
    * @remarks An value that is too high will create more overhead than possibly 
    could be noticed
    * @remarks Finding a good balance for your game is important
    * @remarks Defaults at 60hz
    * @param[in] tickrate (unsigned int) The tickrate value, in hertz.
    */
    PS_NETWORKING_API void SNetSetTickRate(unsigned int tickrate);
    /**
    * @brief Set the tickrate of the networking system
    * @return (unsigned int) The tickrate value, in hertz.
    */
    PS_NETWORKING_API unsigned int SNetGetTickRate();
    /**
    * @brief Create a value for syncing
    * @param[in] location (void*) The location
    * @param[in] type (const sulphur::networking::NetworkValueType) The type
    * @param[in] owner (sulphur::networking::NetworkValueOwner) 
    The owner of the value, defaults to host
    * @remarks Make sure that this function is called on all clients!
    * @remarks Make sure that only 1 client can have a value of NetworkValueOwner::kMe 
    and the other clients should have NetworkValueOwner::kOther. 
    If this is not properly setup, incorrect syncing will occur!
    * @remarks If using NetworkValueOwner::kHost, make sure you are already a host, 
    otherwise setup will fail!
    * @remarks When a client is the owner, it will take approximatly twice 
    as much time before it reaches its destination (client -> host -> client)
    * @return (uint16_t) The id of the value
    */
    PS_NETWORKING_API uint16_t SNetCreateSyncValue(const NetworkValueType type,
      NetworkValueOwner owner = NetworkValueOwner::kHost);
    /**
    * @brief Remove a sync value
    * @param[in] id (uint16_t) The id of the value
    */
    PS_NETWORKING_API void SNetRemoveSyncValue(uint16_t id);
    /**
    * @brief Validates a value on other clients
    * @param[in] id (uint8_t) The id we want to validate
    * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
    * @remarks Only the host is able to use this call,
    this is because the host should have the correct version of the game, anyone else is wrong.
    * @remarks If something is invalid, it wont report back to the host. This is mainly because,
      sometimes in 1 frame, a value could possibly not be create yet.
      However if it keeps occuring something is wrong.
    */
    PS_NETWORKING_API void SNetValidateValue(uint16_t id);
    /**
    * @brief Validates all values on other clients. Just as ValidateValue does
    * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
    * @remarks Only the host is able to use this call,
    this is because the host should have the correct version of the game, anyone else is wrong.
    * @remarks If something is invalid, it wont report back to the host. This is mainly because,
      sometimes in 1 frame, a value could possibly not be create yet.
      However if it keeps occuring something is wrong.
    */
    PS_NETWORKING_API void SNetValidateAllValues();
    /**
    * @brief Set the OnConnectedToServer callback. This function is called when a client is connected to a host
    * @param[in] function (eastl::function<void()>) The function
    */
    PS_NETWORKING_API void SNetSetOnConnectedToServerCallback(
      eastl::function<void()> function);
    /**
    * @brief Set the OnDisconnectedFromServer callback. This function is called when a client is disconnected from a host
    * @param[in] function (eastl::function<void()>) The function
    */
    PS_NETWORKING_API void SNetSetOnDisconnectedFromServerCallback(
      eastl::function<void(DisconnectionInfo)> function);
    /**
    * @brief Set the OnPlayerConnected callback. This function is called when any player is connected.
    * @param[in] function (eastl::function<void(NetworkPlayerData)>) The function
    */
    PS_NETWORKING_API void SNetSetOnPlayerConnected(
      eastl::function<void(NetworkPlayerData)> function);
    /**
    * @brief Set the OnPlayerDisconnected callback. This function is called when any player is disconnected.
    * @param[in] function (eastl::function<void(NetworkPlayerData)>) The function
    */
    PS_NETWORKING_API void SNetSetOnPlayerDisconnected(
      eastl::function<void(NetworkPlayerData)> function);
    /**
    * @brief Set the OnFailedToConnect callback. This function is called when a connection could not be established.
    * @param[in] function (eastl::function<void()>) The function
    */
    PS_NETWORKING_API void SNetSetOnFailedToConnect(eastl::function<void()> function);
    /**
    * @brief Retrieve the ping of the host of an user, clients can only lookup the host
    * @param[in] id (uint8_t) The id of the client we want to lookup. Optional for clients
    */
    PS_NETWORKING_API unsigned int SNetGetPing(uint8_t id = 0);
    /**
    * @brief Registers an rpc in the system
    * @param[in] function (eastl::function<void(sulphur::networking::RPCHandle, sulphur::networking::NetworkPlayerData, sulphur::foundation::Vector<sulphur::networking::NetworkValue>)>) The function pointer
    * @param[in] arguments (const sulphur::foundation::Vector<NetworkValueType>&) The arguments
    * @return (sulphur::networking::RPCHandle) The handle associated with the handle
    */
    PS_NETWORKING_API RPCHandle SNetRegisterRPC(eastl::function<void(RPCHandle, NetworkPlayerData,
      const foundation::Vector<NetworkValue>&)> function, 
      const foundation::Vector<NetworkValueType>& arguments);
    /**
    * @brief Removes an rpc from the system
    * @param[in] handle (const sulphur::foundation::RPCHandle) The handle
    */
    PS_NETWORKING_API void SNetUnregisterRPC(const RPCHandle handle);
    /**
    * @brief Invokes an RPC on another machine(s)
    * @param[in] handle (const sulphur::foundation::RPCHandle) The handle
    * @param[in] mode (const sulphur::foundation::RPCMode) The mode
    * @param[in] arguments (const sulphur::foundation::Vector<sulphur::networking::NetworkValue>&) The arguments
    */
    PS_NETWORKING_API void SNetInvokeRPC(const RPCHandle handle, const RPCMode mode, 
      const foundation::Vector<NetworkValue>& arguments);
    /**
    * @brief Validates an RPC on other clients
    * @param[in] id (uint8_t) The id we want to validate
    * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
    * @remarks Only the host is able to use this call,
    this is because the host should have the correct version of the game, anyone else is wrong.
    */
    PS_NETWORKING_API void SNetValidateRPC(uint16_t id);
    /**
    * @brief Validates all RPCs on other clients. Just as ValidateRPC does
    * @remarks This function should be used occasionally, as it might eat up your bandwith however for debugging this can be nice
    * @remarks Only the host is able to use this call,
    this is because the host should have the correct version of the game, anyone else is wrong.
    */
    PS_NETWORKING_API void SNetValidateAllRPCs();
    /**
    * @brief Set the value of a syncable value
    * @param[in] id (sulphur::networking::SyncValueID) The id of the syncvalue we want to set the value
    * @param[in] value (const NetworkValue&) The value
    */
    PS_NETWORKING_API void SNetSetSyncValue(SyncValueID id, const NetworkValue& value);
    /**
    * @brief Get the value of a syncable value
    * @param[in] id (sulphur::networking::SyncValueID) The id of the syncvalue we want to get the value
    * @param[out] value (NetworkValue*) A pointer to a networkvalue
    * @return (bool) If getting the value succeeded
    */
    PS_NETWORKING_API bool SNetGetSyncValue(SyncValueID id, NetworkValue* value);
    /**
    * @brief Clears the RPC buffer, usefull to call after a point where everybody is equal again (example: back to the lobby)
    */
    PS_NETWORKING_API void SNetClearRPCBuffer();
    /**
    * @brief Sorts IDs for value sync and rpc systems, this is useful when objects get destroyed that rely on this
    */
    PS_NETWORKING_API void SNetSortIDs();
    namespace editor
    {
      /**
      * @enum sulphur::networking::editor::ErrorMessage
      * @brief Enum that displays error messages
      */
      enum struct ErrorMessage : unsigned int
      {
        kOk, //!< ok
        kEnetInitFailed, //!< Enet failed to initialise
        kHostCreateFailed, //!< Creating a host failed
        kResolveIpFailed, //!< Resolving the IP address failed
        kConnectFailed, //!< Connecting to IP failed
        kPacketCreationFailed, //!< Failed to create a packet
        kNoPeerConnected, //!< No peer connected
        kPacketSendFailed //!< Failed to send a packet

      };

      static constexpr unsigned int kMaxPacketSize = 512; //!< Max size of packet
      static constexpr unsigned int kMaxPayloadSize = 
        kMaxPacketSize - sizeof(uint32_t); //!< Max size of a payload

      /**
      * @struct sulphur::networking::editor::ConnectionData
      * @brief Struct that holds internal data
      */
      static struct ConnectionData
      {
        ENetHost* host = nullptr; //!< Host
        ENetPeer* peer = nullptr; //!< Peer
      } connection_data;

      /**
      * @brief Initialises the engine networking system
      * @param[in] port (unsigned int) The port we need to host the engine on
      * @remarks The engine is the host, and the editor acts as a client
      * @return (ErrorMessage) The type of error that has occured
      */
      PS_NETWORKING_C_API ErrorMessage SNetInitEngine(unsigned int port);
      /**
      * @brief Initialises the editor networking system
      * @remarks The engine is the host, and the editor acts as a client
      * @return (ErrorMessage) The type of error that has occured
      */
      PS_NETWORKING_C_API ErrorMessage SNetInitEditor();
      /**
      * @brief Connectes the editor to the engine
      * @param[in] ip (const char*) The ip we need to connect to
      * @param[in] port (unsigned int) The port we need to host the engine on
      * @return (ErrorMessage) The type of error that has occured
      */
      PS_NETWORKING_C_API ErrorMessage SNetEditorConnect(const char* ip, 
        unsigned int port);
      /**
      * @brief Check for incomming packets
      * @param[out] id (uint32_t*) An integer value that identifies the type of 
      the incoming packet
      * @param[out] data (void*) The buffer we are supposed to write our data in
      * @param[in] size (unsigned int) Size of the buffer we are allowed to write to
      * @remarks function should be used in a while loop every frame as multiple 
      packets can be recieved in a frame
      * @return (bool) If we have recieved a packet
      */
      PS_NETWORKING_C_API bool SNetRetrievePacket(uint32_t* id, void* data, 
        unsigned int size);
      /**
      * @brief Send packets immediately
      * @remarks Normally packets are send when calling RetrievePackets(), 
      the use of this function is optional
      */
      PS_NETWORKING_C_API void SNetFlushPackets();
      /**
      * @brief Send data to our peer
      * @param[in] id (uint32_t) An integer value that identifies the message's type
      * @param[in] actual_data (const void*) The data we want to send
      * @param[in] size (unsigned int) Size of the data we want to send
      * @return (ErrorMessage) The type of error that has occured
      */
      PS_NETWORKING_C_API ErrorMessage SNetSendData(uint32_t id, 
        const void* actual_data, unsigned int size);
      /**
      * @brief Destroys the networking system
      */
      PS_NETWORKING_C_API void SNetDestroy();
      /**
      * @brief Check whether we are connected
      * @return (bool) If we are connected
      */
      PS_NETWORKING_C_API bool SNetIsConnected();
    }
  }
}