#pragma once

#include "engine/systems/service_system.h"

#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/scriptable_object.h"

#include <foundation/containers/vector.h>

#include <functional>

#include <tools/networking/connection_state.h>
#include <tools/networking/network_player.h>
#include <tools/networking/disconnection_info.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::NetworkSystem : public sulphur::engine::IServiceSystem<NetworkSystem>
    * @brief The networking system inside the engine
    * @author Joren Bolhuis
    */
    class NetworkSystem : public IServiceSystem<NetworkSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      NetworkSystem();
      /**
      * @see sulphur::engine::ISystemBase::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @see sulphur::engine::ISystemBase::OnTerminate
      */
      void OnTerminate() override;
      /**
      * @see sulphur::networking::SNetInitHost
      */
      bool CreateHost(unsigned int port, unsigned int max_clients,
        const foundation::String& player_name);
      /**
      * @see sulphur::networking::SNetInitClient
      */
      bool CreateClient(const foundation::String& player_name);
      /**
      * @see sulphur::networking::SNetDestroyHostOrClient
      */
      bool DestroyHostOrClient();
      /**
      * @see sulphur::networking::SNetConnect
      */
      bool Connect(const foundation::String& ip, unsigned int port);
      /**
      * @see sulphur::networking::SNetDisconnect
      */
      bool Disconnect();
      /**
      * @see sulphur::networking::SNetSetName
      */
      void SetName(const foundation::String& name);
      /**
      * @see sulphur::networking::SNetGetName
      */
      foundation::String GetName();
      /**
      * @see sulphur::networking::SNetGetID
      */
      unsigned int GetID();
      /**
      * @see sulphur::networking::SNetIsConnected
      */
      bool IsConnected();
      /**
      * @see sulphur::networking::SNetIsHost
      */
      bool IsHost();
      /**
      * @see sulphur::networking::SNetIsClient
      */
      bool IsClient();
      /**
      * @see sulphur::networking::SNetGetNumClients
      */
      unsigned int GetNumClients();
      /**
      * @see sulphur::networking::SNetGetNetworkPlayers
      */
      foundation::Vector<networking::NetworkPlayerData> GetNetworkPlayers();
      /**
      * @see sulphur::networking::SNetConnectionSetup
      */
      networking::ConnectionSetup GetConnectionSetup();
      /**
      * @see sulphur::networking::SNetConnectionStatus
      */
      networking::ConnectionStatus GetConnectionStatus();
      /**
      * @see sulphur::networking::SNetKickClient
      */
      bool KickClient(unsigned int id);
      /**
      * @see sulphur::networking::SNetSetConnectionTimeout
      */
      void SetConnectionTimeout(unsigned int timeout);
      /**
      * @see sulphur::networking::SNetGetConnectionTimeout
      */
      unsigned int GetConnectionTimeout();
      /**
      * @see sulphur::networking::SNetSetTickRate
      */
      void SetTickRate(unsigned int tickrate);
      /**
      * @see sulphur::networking::SNetGetTickRate
      */
      unsigned int GetTickRate();
      /**
      * @see sulphur::networking::SNetGetPing
      */
      unsigned int GetPing(uint8_t id = 0);
      /**
      * @brief Set the OnConnectedToServer callback. This function is called when a client is connected to a host
      * @see sulphur::networking::SNetSetOnConnectedToServerCallback
      * @param[in] function (eastl::function<void()>) The function
      */
      void SetOnConnectedToServerCallback(eastl::function<void()> function);
      /**
      * @brief Set the OnDisconnectedFromServer callback. This function is called when a client is disconnected from a host
      * @see sulphur::networking::SNetSetOnDisconnectedFromServerCallback
      * @param[in] function (eastl::function<void()>) The function
      */
      void SetOnDisconnectedFromServerCallback(eastl::function<void(networking::DisconnectionInfo)> function);
      /**
      * @brief Set the OnPlayerConnected callback. This function is called when any player is connected.
      * @see sulphur::networking::SNetSetOnPlayerConnected
      * @param[in] function (eastl::function<void(NetworkPlayerData)>) The function
      */
      void SetOnPlayerConnected(eastl::function<void(networking::NetworkPlayerData)> function);
      /**
      * @brief Set the OnPlayerDisconnected callback. This function is called when any player is disconnected.
      * @see sulphur::networking::SNetSetOnPlayerDisconnected
      * @param[in] function (eastl::function<void(NetworkPlayerData)>) The function
      */
      void SetOnPlayerDisconnected(eastl::function<void(networking::NetworkPlayerData)> function);
      /**
      * @brief Set the OnFailedToConnect callback. This function is called when a connection could not be established.
      * @see sulphur::networking::SNetSetOnFailedToConnect
      * @param[in] function (eastl::function<void()>) The function
      */
      void SetOnFailedToConnect(eastl::function<void()> function);
      /**
      * @brief Clears the RPC buffer, usefull to call after a point where everybody is equal again (example: back to the lobby)
      */
      void ClearRPCBuffer();
    private:
    };

    SCRIPT_CLASS() class ScriptableNetworkPlayer : public ScriptableObject
    {
    public:
      SCRIPT_NAME(NetworkPlayer);

      SCRIPT_FUNC() uint8_t id();
      SCRIPT_FUNC() foundation::String name();
      SCRIPT_FUNC() uint32_t ping();

      networking::NetworkPlayerData data;
    };
    /**
    * @struct sulphur::engine::ScriptableNetworking : public sulphur::engine::ScriptableObject
    * @brief The network system exposed to the scripting environment, statically
    * @author Joren Bolhuis
    */
    SCRIPT_CLASS() class ScriptableNetworking : public ScriptableObject
    {
    public:

      /**
      * @brief Scriptable ConnectionSetup
      * @see sulphur::networking::ConnectionSetup
      */
      SCRIPT_ENUM() enum ConnectionSetup
      {
        kNone, /**< setup is not initialised */
        kHost, /**< Setup is a host */
        kClient /**< Setup is a client */
      };

      /**
      * @brief Scriptable ConnectionStatus
      * @see sulphur::networking::ConnectionStatus
      */
      SCRIPT_ENUM() enum ConnectionStatus
      {
        kConnecting, //!< connecting to host
        kConnected, //!< connected to a host
        kDisconnecting, //!< trying to disconnect from a host
        kDisconnected //!< Initial value, disconnected from a host
      };

      /**
      * @brief Scriptable DisconnectionInfo
      * @see sulphur::networking::DisconnectionInfo
      */
      SCRIPT_ENUM() enum DisconnectionInfo
      {
        kDisconnect,
        kLostConnection,
        kKicked
      };

      SCRIPT_NAME(Network);
      /**
      * @brief Initializes scriptable networking with the network system
      */
      static void Initialize(NetworkSystem* network, ScriptState* state);
      /**
      * @see sulphur::engine::CreateHost
      */
      SCRIPT_FUNC(static) bool CreateHost(uint32_t port, uint32_t max_clients,
        const foundation::String& player_name);
      /**
      * @see sulphur::engine::CreateClient
      */
      SCRIPT_FUNC(static) bool CreateClient(const foundation::String& player_name);
      /**
      * @see sulphur::engine::DestroyHostOrClient
      */
      SCRIPT_FUNC(static) bool DestroyHostOrClient();
      /**
      * @see sulphur::engine::Connect
      */
      SCRIPT_FUNC(static) bool Connect(const foundation::String& ip, uint32_t port);
      /**
      * @see sulphur::engine::Disconnect
      */
      SCRIPT_FUNC(static)bool Disconnect();
      /**
      * @see sulphur::engine::SetName
      */
      SCRIPT_FUNC(static)void SetName(const foundation::String& name);
      /**
      * @see sulphur::engine::GetName
      */
      SCRIPT_FUNC(static) foundation::String GetName();
      /**
      * @see sulphur::engine::GetID
      */
      SCRIPT_FUNC(static) uint32_t GetID();
      /**
      * @see sulphur::engine::IsConnected
      */
      SCRIPT_FUNC(static) bool IsConnected();
      /**
      * @see sulphur::engine::IsHost
      */
      SCRIPT_FUNC(static) bool IsHost();
      /**
      * @see sulphur::networking::IsClient
      */
      SCRIPT_FUNC(static) bool IsClient();
      /**
      * @see sulphur::engine::GetNumClients
      */
      SCRIPT_FUNC(static) uint32_t GetNumClients();
      /**
      * @see sulphur::engine::GetNetworkPlayers
      */
      SCRIPT_FUNC(static) ScriptHandle GetNetworkPlayers();
      /**
      * @see sulphur::engine::GetConnectionSetup
      */
      SCRIPT_FUNC(static) ScriptableNetworking::ConnectionSetup GetConnectionSetup();
      /**
      * @see sulphur::engine::GetConnectionStatus
      */
      SCRIPT_FUNC(static) ScriptableNetworking::ConnectionStatus GetConnectionStatus();
      /**
      * @see sulphur::engine::KickClient
      */
      SCRIPT_FUNC(static) bool KickClient(uint32_t id);
      /**
      * @see sulphur::engine::SetConnectionTimeout
      */
      SCRIPT_FUNC(static) void SetConnectionTimeout(uint32_t timeout);
      /**
      * @see sulphur::engine::GetConnectionTimeout
      */
      SCRIPT_FUNC(static) uint32_t GetConnectionTimeout();
      /**
      * @see sulphur::engine::SetTickRate
      */
      SCRIPT_FUNC(static) void SetTickRate(uint32_t tickrate);
      /**
      * @see sulphur::engine::GetTickRate
      */
      SCRIPT_FUNC(static) uint32_t GetTickRate();
      /**
      * @see sulphur::engine::GetPing
      */
      SCRIPT_FUNC(static) uint32_t GetPing(uint8_t id);
      /**
      * @see sulphur::engine::SetOnConnectedToServerCallback
      */
      SCRIPT_FUNC(static) void SetOnConnectedToServerCallback(ScriptHandle self, ScriptableCallback function);
      /**
      * @see sulphur::engine::SetOnDisconnectedFromServerCallback
      */
      SCRIPT_FUNC(static) void SetOnDisconnectedFromServerCallback(ScriptHandle self, ScriptableCallback function);
      /**
      * @see sulphur::engine::SetOnPlayerConnected
      */
      SCRIPT_FUNC(static) void SetOnPlayerConnectedCallback(ScriptHandle self, ScriptableCallback function);
      /**
      * @see sulphur::engine::SetOnPlayerDisconnected
      */
      SCRIPT_FUNC(static) void SetOnPlayerDisconnectedCallback(ScriptHandle self, ScriptableCallback function);
      /**
      * @see sulphur::engine::SetOnFailedToConnect
      */
      SCRIPT_FUNC(static) void SetOnFailedToConnectCallback(ScriptHandle self, ScriptableCallback function);
      /**
      * @brief Clears the RPC buffer, usefull to call after a point where everybody is equal again (example: back to the lobby)
      */
      SCRIPT_FUNC(static) void ClearRPCBuffer();
    private:
      static NetworkSystem* network_; //!< The actual input system
      static ScriptState* script_state_; //!< The script state
      static ScriptableCallback on_connected_to_server_callback_; //!< The lua callback for on connected to server
      static ScriptableCallback on_disconnected_from_server_callback_; //!< The lua callback for on disconnected from server
      static ScriptableCallback on_player_connected_callback_; //!< The lua callback for on player connected
      static ScriptableCallback on_player_disconnected_callback_; //!< The lua callback for on player disconnected
      static ScriptableCallback on_failed_to_connect_callback_; //!< The lua callback for on failed to connect
    };
  }
}