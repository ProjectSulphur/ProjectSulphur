#include "engine/networking/network_system.h"
#include "engine/application/application.h"
#include "engine/scripting/script_system.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job_graph.h>
#include <foundation/utils/frame.h>

#include <tools/networking/export.h>
#include <tools/networking/networking_logger.h>
#include <tools/networking/disconnection_info.h>

#include <lua-classes/network_system.lua.cc>

#include <EASTL/functional.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    NetworkSystem::NetworkSystem() :
      IServiceSystem("NetworkSystem")
    {
      size_t size = 2ul * 1024ul * 1024ul;
      sulphur::networking::SNetConstruct(size);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      if (networking::SNetInitSystem() == false)
      {
        PS_LOG_WITH(NetworkLogger, Error, "Failed to initialise networking system!");
      }

      auto update = [](NetworkSystem&)
      {
        networking::SNetUpdate(foundation::Frame::delta_time());
        networking::SNetLateUpdate();
      };

      job_graph.Add(make_job("networksystem_update", "update", update, bind_write(*this)));
      ScriptableNetworking::Initialize(this, app.GetService<ScriptSystem>().script_state());
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::OnTerminate()
    {
      networking::SNetDestroySystem();
      networking::SNetDestruct();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::CreateHost(unsigned int port, unsigned int max_clients,
      const foundation::String& player_name)
    {
      return networking::SNetInitHost(port, max_clients, player_name);
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::CreateClient(const foundation::String& player_name)
    {
      return networking::SNetInitClient(player_name);
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::DestroyHostOrClient()
    {
      return networking::SNetDestroyHostOrClient();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::Connect(const foundation::String& ip, unsigned int port)
    {
      return networking::SNetConnect(ip, port);
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::Disconnect()
    {
      return networking::SNetDisconnect();
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetName(const foundation::String& name)
    {
      networking::SNetSetName(name);
    }

    //-------------------------------------------------------------------------
    foundation::String NetworkSystem::GetName()
    {
      return networking::SNetGetName();
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkSystem::GetID()
    {
      return networking::SNetGetID();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::IsConnected()
    {
      return networking::SNetIsConnected();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::IsHost()
    {
      return networking::SNetIsHost();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::IsClient()
    {
      return networking::SNetIsClient();
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkSystem::GetNumClients()
    {
      return networking::SNetGetNumClients();
    }

    //-------------------------------------------------------------------------
    foundation::Vector<networking::NetworkPlayerData> NetworkSystem::GetNetworkPlayers()
    {
      return networking::SNetGetNetworkPlayers();
    }

    //-------------------------------------------------------------------------
    networking::ConnectionSetup NetworkSystem::GetConnectionSetup()
    {
      return networking::SNetConnectionSetup();
    }

    //-------------------------------------------------------------------------
    networking::ConnectionStatus NetworkSystem::GetConnectionStatus()
    {
      return networking::SNetConnectionStatus();
    }

    //-------------------------------------------------------------------------
    bool NetworkSystem::KickClient(unsigned int id)
    {
      return networking::SNetKickClient(static_cast<uint8_t>(id));
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetConnectionTimeout(unsigned int timeout)
    {
      networking::SNetSetConnectionTimeout(timeout);
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkSystem::GetConnectionTimeout()
    {
      return networking::SNetGetConnectionTimeout();
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetTickRate(unsigned int tickrate)
    {
      networking::SNetSetTickRate(tickrate);
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkSystem::GetTickRate()
    {
      return networking::SNetGetTickRate();
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkSystem::GetPing(uint8_t id)
    {
      return networking::SNetGetPing(id);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetOnConnectedToServerCallback(eastl::function<void()> function)
    {
      networking::SNetSetOnConnectedToServerCallback(function);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetOnDisconnectedFromServerCallback(eastl::function<void(networking::DisconnectionInfo)> function)
    {
      networking::SNetSetOnDisconnectedFromServerCallback(function);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetOnPlayerConnected(eastl::function<void(networking::NetworkPlayerData)> function)
    {
      networking::SNetSetOnPlayerConnected(function);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetOnPlayerDisconnected(eastl::function<void(networking::NetworkPlayerData)> function)
    {
      networking::SNetSetOnPlayerDisconnected(function);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SetOnFailedToConnect(eastl::function<void()> function)
    {
      networking::SNetSetOnFailedToConnect(function);
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::ClearRPCBuffer()
    {
      networking::SNetClearRPCBuffer();
    }

    //-------------------------------------------------------------------------
    void NetworkSystem::SortIDs()
    {
      networking::SNetSortIDs();
    }


    //-------------------------------------------------------------------------
    NetworkSystem* ScriptableNetworking::network_ = nullptr;
    ScriptState* ScriptableNetworking::script_state_ = nullptr;
    ScriptableCallback ScriptableNetworking::on_connected_to_server_callback_;
    ScriptableCallback ScriptableNetworking::on_disconnected_from_server_callback_;
    ScriptableCallback ScriptableNetworking::on_player_connected_callback_;
    ScriptableCallback ScriptableNetworking::on_player_disconnected_callback_;
    ScriptableCallback ScriptableNetworking::on_failed_to_connect_callback_;

    //-------------------------------------------------------------------------
    void ScriptableNetworking::Initialize(NetworkSystem* network, ScriptState* state)
    {
      network_ = network;
      script_state_ = state;
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::CreateHost(uint32_t port, uint32_t max_clients,
      const foundation::String & player_name)
    {
      return network_->CreateHost(port, max_clients, player_name);
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::CreateClient(const foundation::String& player_name)
    {
      return network_->CreateClient(player_name);
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::DestroyHostOrClient()
    {
      return network_->DestroyHostOrClient();
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::Connect(const foundation::String& ip, uint32_t port)
    {
      return network_->Connect(ip, port);
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::Disconnect()
    {
      return network_->Disconnect();
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetName(const foundation::String& name)
    {
      network_->SetName(name);
    }

    //-------------------------------------------------------------------------
    foundation::String ScriptableNetworking::GetName()
    {
      return network_->GetName();
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworking::GetID()
    {
      return network_->GetID();
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::IsConnected()
    {
      return network_->IsConnected();
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::IsHost()
    {
      return network_->IsHost();
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::IsClient()
    {
      return network_->IsClient();
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworking::GetNumClients()
    {
      return network_->GetNumClients();
    }

    //-------------------------------------------------------------------------
    ScriptHandle ScriptableNetworking::GetNetworkPlayers()
    {
      foundation::Vector<networking::NetworkPlayerData> data
        = network_->GetNetworkPlayers();

      ScriptTableHandle handle = ScriptUtils::CreateTable(script_state_);
      for (size_t i = 0; i < data.size(); ++i)
      {
        ScriptableNetworkPlayer player;
        player.data = data.at(i);
        handle->Insert(static_cast<int>(i), ScriptUtils::Instantiate<ScriptableNetworkPlayer>(script_state_, player));
      }

      return eastl::static_pointer_cast<ScriptableTable, ScriptableValue>(handle);
    }

    //-------------------------------------------------------------------------
    ScriptableNetworking::ConnectionSetup ScriptableNetworking::GetConnectionSetup()
    {
      return static_cast<ScriptableNetworking::ConnectionSetup>(network_->GetConnectionSetup());
    }

    //-------------------------------------------------------------------------
    ScriptableNetworking::ConnectionStatus ScriptableNetworking::GetConnectionStatus()
    {
      return static_cast<ScriptableNetworking::ConnectionStatus>(network_->GetConnectionStatus());
    }

    //-------------------------------------------------------------------------
    bool ScriptableNetworking::KickClient(uint32_t id)
    {
      return network_->KickClient(id);
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetConnectionTimeout(uint32_t timeout)
    {
      return network_->SetConnectionTimeout(timeout);
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworking::GetConnectionTimeout()
    {
      return network_->GetConnectionTimeout();
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetTickRate(uint32_t tickrate)
    {
      return network_->SetTickRate(tickrate);
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworking::GetTickRate()
    {
      return network_->GetTickRate();
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworking::GetPing(uint8_t id)
    {
      return network_->GetPing(id);
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetOnConnectedToServerCallback(ScriptHandle self, ScriptableCallback function)
    {
      on_connected_to_server_callback_ = function;
      network_->SetOnConnectedToServerCallback([self]() {
        on_connected_to_server_callback_.Call({ self });
      });
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetOnDisconnectedFromServerCallback(ScriptHandle self, ScriptableCallback function)
    {
      on_disconnected_from_server_callback_ = function;
      network_->SetOnDisconnectedFromServerCallback([self](networking::DisconnectionInfo info) {
        int in = static_cast<int>(info);
        ScriptHandle handle = ScriptUtils::Instantiate(script_state_, in);
        on_disconnected_from_server_callback_.Call({ self, handle });
      });
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetOnPlayerConnectedCallback(ScriptHandle self, ScriptableCallback function)
    {
      on_player_connected_callback_ = function;
      network_->SetOnPlayerConnected([self](networking::NetworkPlayerData player) {
        ScriptableNetworkPlayer pl;
        pl.data = player;
        ScriptHandle handle = ScriptUtils::Instantiate(script_state_, pl);
        on_player_connected_callback_.Call({ self, handle });
      });
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetOnPlayerDisconnectedCallback(ScriptHandle self, ScriptableCallback function)
    {
      on_player_disconnected_callback_ = function;
      network_->SetOnPlayerDisconnected([self](networking::NetworkPlayerData player) {
        ScriptableNetworkPlayer pl;
        pl.data = player;
        ScriptHandle handle = ScriptUtils::Instantiate(script_state_, pl);
        on_player_disconnected_callback_.Call({ self, handle });
      });
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SetOnFailedToConnectCallback(ScriptHandle self, ScriptableCallback function)
    {
      on_failed_to_connect_callback_ = function;
      network_->SetOnFailedToConnect([self]() {
        on_failed_to_connect_callback_.Call({ self });
      });
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::ClearRPCBuffer()
    {
      network_->ClearRPCBuffer();
    }

    //-------------------------------------------------------------------------
    void ScriptableNetworking::SortIDs()
    {
      network_->SortIDs();
    }

    //-------------------------------------------------------------------------
    uint8_t ScriptableNetworkPlayer::id()
    {
      return data.id;
    }

    //-------------------------------------------------------------------------
    foundation::String ScriptableNetworkPlayer::name()
    {
      return foundation::String(data.name);
    }

    //-------------------------------------------------------------------------
    uint32_t ScriptableNetworkPlayer::ping()
    {
      return data.ping;
    }
  }
}
