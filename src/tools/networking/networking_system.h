#pragma once
#include "tools/networking/enet_fwd.h"
#include "tools/networking/networking_logger.h"
#include "tools/networking/connection_state.h"
#include "tools/networking/message.h"
#include "tools/networking/network_player.h"
#include "tools/networking/network_value.h"
#include "tools/networking/value_owner.h"
#include "tools/networking/disconnection_info.h"
#include <foundation/containers/list.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>
#include <functional>

namespace sulphur
{
  namespace networking
  {
    class PacketHandler;
    class ValueSyncer;
    /**
    * @class sulphur::networking::NetworkingSystem
    * @brief Main networking class
    * @remarks The system should be initialised first, the either a client or
    host should be initialised before it can be used
    * @remarks The length of a name is up to 64 characters
    * @author Joren Bolhuis
    */
    class NetworkingSystem
    {
      friend PacketHandler;
    public:
      /**
      * @brief Constructor
      */
      NetworkingSystem();
      /**
      * @brief Destructor
      */
      ~NetworkingSystem();
      /**
      * @brief Initialises the networking system
      * @return (bool) Whether the initialising has succeeded
      */
      bool InitSystem();
      /**
      * @brief Destroys the networking system
      * @remarks The system should be initialised first,
      the either a client or host should be initialised before it can be used
      * @remarks The length of a name is up to 64 characters
      */
      void DestroySystem();
      /**
      * @brief Create a host version for your networking setup
      * @param[in] port (unsigned int) The port opened by the host
      * @param[in] max_client (unsigned int) Maximum amount of clients the hsot allows
      * @param[in] player_name (const char*) The name of the host, used in the playerlist
      * @return (bool) Whether the initialising has succeeded
      */
      bool InitHost(unsigned int port, unsigned int max_clients,
        const char* player_name);
      /**
      * @brief Create a client version for your networking setup
      * @param[in] name (const char*) The name of the client, used in the playerlist
      * @return (bool) Whether the initialising has succeeded
      * @remarks The system should be initialised first,
      the either a client or host should be initialised before it can be used
      * @remarks The length of a name is up to 64 characters
      */
      bool InitClient(const char* name);
      /**
      * @brief Destruction of the host or client initialisation
      * @return (bool) Whether the destruction has succeeded
      * @remarks The system should be initialised first,
      the either a client or host should be initialised before it can be used
      * @remarks The length of a name is up to 64 characters
      */
      bool DestroyHostOrClient();
      /**
      * @brief Updating the network system, should be called every frame
      * @return (bool) Whether the update has succeeded
      */
      bool Update(float deltatime);
      /**
      * @brief Late Updating the network system, mainly for sending the packets we created,
      should be called every frame
      * @return (bool) Whether the update has succeeded
      */
      bool LateUpdate();
      /**
      * @brief Connecting to a host
      * @param[in] ip (const char*) The ip we want to connect to
      * @param[in] port (unsigned int) The port we want to connect to
      * @remarks Function should only be called by a client
      * @remarks Client is not immediately connected but is put in a connecting state,
      if connection succeeds, the state will but put into connected
      * @return (bool) Whether the connection call has succeeded
      */
      bool Connect(const char* ip, unsigned int port);
      /**
      * @brief Disconnect from host
      * @return (bool) Whether the connection call has succeeded
      */
      bool Disconnect();
      /**
      * @brief Change the name of the client,
      can be used anytime since it will be synced by hosts and clients
      * @param[in] name (const char*) The name we want to use
      */
      void SetName(const char* name);
      /**
      * @brief Get the name of the client
      * @return (const char*) name
      * @remarks The length of a name is up to 64 characters
      */
      const char* GetName();
      /**
      * @brief Get the id of the client
      * @return (uint8_t) id
      */
      uint8_t GetID();
      /**
      * @brief Check whether we are connected
      * @return (bool) are we connected?
      * @remarks The length of a name is up to 64 characters
      */
      bool is_connected() const;
      /**
      * @brief Check whether we are a host
      * @return (bool) are we a host?
      */
      bool is_host() const;
      /**
      * @brief Check whether we are a client
      * @return (bool) are we a client?
      */
      bool is_client() const;
      /**
      * @brief Check the amount of players connected
      * @remarks this includes yourself, as you are a player too,
      no matter if you are a host or a client
      * @return (unsigned int) amount of players
      */
      unsigned int num_players() const;
      /**
      * @brief Recieve a networkplayer list
      * @remark Type is NetworkPlayerData as the main struct should not be exposed
      * @return (sulphur::foundation::Vector<NetworkPlayerData>) List of NetworkPlayerData
      */
      foundation::Vector<NetworkPlayerData> GetNetworkPlayers();
      /**
      * @brief Retrieves the connection setup (host, client or not initialised)
      * @return (sulphur::networking::ConnectionSetup) the connection setup
      */
      ConnectionSetup connection_setup() const;
      /**
      * @brief Retrieves the connection status
      (disconnected, connecting, connected or disconnecting)
      * @remarks If we are a host, it will return connected
      * @return (sulphur::networking::ConnectionStatus) the connection status
      */
      ConnectionStatus connection_status() const;
      /**
      * @brief Kick a client, only works if we are a host. We cannot kick ourselves
      * @param[in] id (uint8_t) The client that will be kicked.
      * @return (bool) Whether the kick succeeded.
      */
      bool KickClient(uint8_t id);
      /**
      * @brief Set the time after we should forcequit our connection
      * @param[in] timeout (unsigned int) The timeout in miliseconds
      */
      void SetConnectionTimeout(unsigned int timeout);
      /**
      * @brief Get the value for the connection timeout
      * @return (bool) The connection timeout
      */
      unsigned int connection_timeout() const;
      /**
      * @brief Set the tickrate of the networking system
      * @remarks An value that is too low will result in an inacurate simulation
      * @remarks An value that is too high will create more overhead than possibly could be noticed
      * @remarks Finding a good balance for your game is important
      * @remarks Defaults at 60hz
      * @param[in] tickrate (unsigned int) The tickrate value, in hertz.
      */
      void SetTickRate(unsigned int tickrate);
      /**
      * @brief Set the tickrate of the networking system
      * @return (unsigned int) The tickrate value, in hertz
      */
      unsigned int tick_rate();
      /**
      * @brief Create a value for syncing
      * @param[in] location (void*) The location
      * @param[in] type (const sulphur::networking::NetworkValueType) The type
      * @param[in] owner (sulphur::networking::NetworkValueOwner) The owner of the value
      * @return (uint16_t) The id of the value
      */
      uint16_t CreateSyncValue(const NetworkValueType type, void* location, NetworkValueOwner owner);
      /**
      * @brief Remove a sync value
      * @param[in] id (uint16_t) The id of the value
      */
      void RemoveSyncValue(uint16_t id);
      /**
      * @brief Validates a value on other clients
      * @param[in] id (uint8_t) The id we want to validate
      * @remarks This function should be used occasionally, 
      as it might eat up your bandwith however for debugging this can be nice
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      @remarks If something is invalid, it wont report back to the host,
      the client should decide if something is really wrong and should be disconnected.
      */
      void ValidateValue(uint16_t id);
      /**
      * @brief Validates all values on other clients. Just as ValidateValue does
      * @remarks This function should be used occasionally, 
      as it might eat up your bandwith however for debugging this can be nice
      * @remarks Only the host is able to use this call,
      this is because the host should have the correct version of the game, anyone else is wrong.
      @remarks If something is invalid, it wont report back to the host,
      the client should decide if something is really wrong and should be disconnected.
      */
      void ValidateAllValues();
      /**
      * @brief Set the OnConnectedToServer callback
      * @param[in] function (std::function<void()>) The function
      */
      void SetOnConnectedToServerCallback(std::function<void()> function);
      /**
      * @brief Set the OnDisconnectedFromServer callback
      * @param[in] function (std::function<void()>) The function
      */
      void SetOnDisconnectedFromServerCallback(std::function<void(DisconnectionInfo)> function);
      /**
      * @brief Set the OnPlayerConnected callback
      * @param[in] function (std::function<void(NetworkPlayerData)>) The function
      */
      void SetOnPlayerConnected(std::function<void(NetworkPlayerData)> function);
      /**
      * @brief Set the OnPlayerDisconnected callback
      * @param[in] function (std::function<void(NetworkPlayerData)>) The function
      */
      void SetOnPlayerDisconnected(std::function<void(NetworkPlayerData)> function);
      /**
      * @brief Set the OnFailedToConnect callback
      * @param[in] function (std::function<void()>) The function
      */
      void SetOnFailedToConnect(std::function<void()> function);
    private:
      PacketHandler* packet_handler_; //!<The packet handler
      ValueSyncer* value_syncer_; //!<The value syncer
      ConnectionSetup connection_setup_; //!<The current connection setup
      ConnectionStatus connection_status_; //!<The current connection status
      ENetHost* host_; //!< The ENet host
      unsigned int tick_rate_; //!< The tickrate in hertz
      unsigned int connection_timeout_; //!<The connection timeout in ms
      const float heartbeat_timeout_; //!<The heartbeat timeout in seconds
      const float heartbeat_frequency_; //!<The frequency of the heartbeat in seconds
      float timer_; //!<Internal timer
      float last_tick_update_; //!<Last time we updated
      float last_tick_late_update_; //!<Last time we late updated
      NetworkPlayer* network_player_; //!<Our Networkplayer in the networkplayer list
      foundation::List<NetworkPlayer> network_player_list_; //!<The list with all the networkplayers
      foundation::Deque<uint8_t> available_ids_; //!<The available IDs that can be used
      std::function<void()> on_connected_to_server_; //!<The OnConnectedToServer callback
      std::function<void(DisconnectionInfo)> on_disconnected_from_server_; //!<The OnDisconnectedFromServer callback
      std::function<void(NetworkPlayerData)> on_player_connected_; //!<The OnPlayerConnected callback
      std::function<void(NetworkPlayerData)> on_player_disconnected_; //!<The OnPlayerDisconnected callback
      std::function<void()> on_failed_to_connect_; //!<The OnFailedToConnect callback
      /**
      * @brief handle a connect event
      * @remarks Internal use only
      * @param[in] network_event (ENetEvent*) event that occured
      */
      void HandleConnectEvent(ENetEvent* network_event);
      /**
      * @brief handle a disconnect event
      * @remarks Internal use only
      * @param[in] network_event (ENetEvent*) event that occured
      */
      void HandleDisconnectEvent(ENetEvent* network_event);
      /**
      * @brief handle a recieve event
      * @remarks Internal use only
      * @param[in] network_event (ENetEvent*) event that occured
      */
      void HandleRecieveEvent(ENetEvent* network_event);

      /**
      * @brief Assign a name on the host side
      * @remarks Internal use only
      * @param[in] data (uint8_t*) Pointer to the data we can start reading from
      * @param[in] peer (ENetPeer*) The peer associated
      */
      void HostAssignName(uint8_t* data, ENetPeer* peer);
      /**
      * @brief Update a name on the client side
      * @remarks Internal use only
      * @param[in] data (uint8_t*) Pointer to the data we can start reading from
      */
      void ClientUpdateName(uint8_t* data);
      /**
      * @brief Assign our id on the client side
      * @remarks Internal use only
      * @param[in] id (uint8_t) Asssign ID
      */
      void ClientAssignID(uint8_t id);
      /**
      * @brief Add a player on the client side
      * @remarks Internal use only
      * @param[in] name (const char*) The name of the player
      * @param[in] id (uint8_t) The id of the player
      */
      void ClientAddPlayer(const char* name, uint8_t id);
      /**
      * @brief Remove a player on the client side
      * @remarks Internal use only
      * @param[in] id (uint8_t) The id of the player
      */
      void ClientRemovePlayer(uint8_t id);
      /**
      * @brief Update the heartbeat variable
      * @remarks Internal use only
      * @param[in] player (NetworkPlayer*) The player we just recieved a heartbeat from
      */
      void SetLastHeartBeat(NetworkPlayer* player);
      /**
      * @brief Check the heartbeats
      * @remarks Internal use only
      */
      void CheckHeartbeats();
      /**
      * @brief Force disconnect
      * @remarks Internal use only
      */
      void ForceDisconnect();
      /**
      * @brief Retrieve an unique ID
      * @remarks Internal use only
      * @return (uint8_t) The unique ID
      */
      uint8_t GetNewID();
      /**
      * @brief Reset the pool of IDs
      * @remarks Internal use only
      */
      void ResetIDPool();
      /**
      * @brief Free an unused ID
      * @remarks Internal use only
      * @param[in] id (uint8_t) The if we want to free
      */
      void FreeID(uint8_t id);
      /**
      * @brief Find a networkplayer by peer
      * @remarks Internal use only
      * @param[in] peer (ENetPeer*) The peer
      * @return (NetworkPlayer*) The networkplayer
      */
      NetworkPlayer* FindNetworkPlayerByPeer(ENetPeer* peer);
      /**
      * @brief Find a networkplayer by id
      * @remarks Internal use only
      * @param[in] id (uint8_t) The id
      * @return (NetworkPlayer*) The networkplayer
      */
      NetworkPlayer* FindNetworkPlayerByID(uint8_t id);
      /**
      * @brief Find a networkplayer iterator by peer
      * @remarks Internal use only
      * @param[in] peer (ENetPeer*) The peer
      * @return (foundation::List<NetworkPlayer>::iterator) The networkplayer iterator
      */
      foundation::List<NetworkPlayer>::iterator
        FindNetworkPlayerIteratorByPeer(ENetPeer* peer);
      /**
      * @brief Find a networkplayer iterator by id
      * @remarks Internal use only
      * @param[in] id (uint8_t) The id
      * @return (foundation::List<NetworkPlayer>::iterator) The networkplayer iterator
      */
      foundation::List<NetworkPlayer>::iterator
        FindNetworkPlayerIteratorByID(uint8_t id);
    };
  }
}