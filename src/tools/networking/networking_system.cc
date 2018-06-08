#include "tools/networking/networking_system.h"
#include "tools/networking/packet_handler.h"
#include "tools/networking/value_syncer.h"
#include "tools/networking/rpc_system.h"
#include <foundation/utils/timer.h>
#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>
#include <enet/enet.h>

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    NetworkingSystem::NetworkingSystem() :
      packet_handler_(nullptr),
      value_syncer_(nullptr),
      rpc_system_(nullptr),
      connection_setup_(ConnectionSetup::kNone),
      connection_status_(ConnectionStatus::kDisconnected),
      host_(nullptr),
      tick_rate_(64),
      connection_timeout_(5000),
      heartbeat_timeout_(20.0f),
      heartbeat_frequency_(8.0f),
      timer_(0.0f),
      last_tick_update_(0),
      last_tick_late_update_(0),
      network_player_(nullptr),
      on_connected_to_server_(nullptr),
      on_disconnected_from_server_(nullptr),
      on_player_connected_(nullptr),
      on_player_disconnected_(nullptr),
      on_failed_to_connect_(nullptr)
    {
      packet_handler_ = foundation::Memory::Construct<PacketHandler>(this);
      value_syncer_ = foundation::Memory::Construct<ValueSyncer>(this, packet_handler_);
      rpc_system_ = foundation::Memory::Construct<RPCSystem>(this, packet_handler_);
    }

    //-------------------------------------------------------------------------
    NetworkingSystem::~NetworkingSystem()
    {
      foundation::Memory::Destruct(packet_handler_);
      foundation::Memory::Destruct(value_syncer_);
      foundation::Memory::Destruct(rpc_system_);
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::InitSystem()
    {
      if (enet_initialize() != 0)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "An error occurred while initializing ENet - @InitSystem()\n");
        return false;
      }

      return true;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::DestroySystem()
    {
      enet_deinitialize();
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::InitHost(unsigned int port,
      unsigned int max_clients, const char* player_name)
    {
      ResetIDPool();

      if (connection_setup_ != ConnectionSetup::kNone)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Already initialized, call DeInit() before creating a new connection - @InitHost()\n");
        return false;
      }

      ENetAddress address;
      address.host = ENET_HOST_ANY;
      address.port = static_cast<enet_uint16>(port);

      size_t clients = static_cast<size_t>(max_clients - 1);
      if (clients > 256)
      {
        clients = 256;
        PS_LOG_WITH(NetworkLogger, Warning,
          "Max clients more then 255, which is the limit.\n");
      }
      if (clients < 1)
      {
        clients = 1;
        PS_LOG_WITH(NetworkLogger, Warning,
          "Max clients less then 2, we need atleast 1 other spot to call it 'networking', resetting it to 2 clients..\n");
      }

      host_ = enet_host_create(&address, clients, 2, 0, 0);

      if (host_ == nullptr)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Error occurred while trying to create a host - @InitHost()\n");
        return false;
      }

      char hostip_[kIpLength];
      enet_address_get_host_ip(&host_->address, hostip_, kIpLength);
      foundation::String ip = hostip_;
      ip.append(":");
      ip.append(foundation::to_string(port));

      NetworkPlayer pl = NetworkPlayer(0, nullptr, player_name, ip.c_str(),
        timer_);
      network_player_list_.push_back(pl);
      network_player_ = &*network_player_list_.begin();
      connection_setup_ = ConnectionSetup::kHost;
      connection_status_ = ConnectionStatus::kConnected;

      if (on_connected_to_server_ != nullptr)
      {
        on_connected_to_server_();
      }
      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::InitClient(const char* name)
    {
      if (connection_setup_ != ConnectionSetup::kNone)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Already initialized, call DeInit() before creating a new connection - @InitClient()\n");
        return false;
      }

      host_ = enet_host_create(nullptr, 1, 2, 0, 0);
      if (host_ == nullptr)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Error occurred while trying to create a client - @InitClient()\n");
        return false;
      }

      NetworkPlayer pl = NetworkPlayer(0, nullptr, name, "", timer_);
      network_player_list_.push_back(pl);
      network_player_ = &*network_player_list_.begin();
      connection_setup_ = ConnectionSetup::kClient;
      connection_status_ = ConnectionStatus::kDisconnected;

      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::DestroyHostOrClient()
    {
      if (connection_setup_ == ConnectionSetup::kNone)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "No setup was initialized in the first place - @DestroyHostOrClient()\n");
        return false;
      }

      if (is_host())
      {
        if (on_disconnected_from_server_ != nullptr)
        {
          on_disconnected_from_server_(DisconnectionInfo::kDisconnected);
        }
      }

      enet_host_destroy(host_);

      network_player_ = nullptr;
      network_player_list_.clear();

      connection_setup_ = ConnectionSetup::kNone;
      connection_status_ = ConnectionStatus::kDisconnected;
      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::Update(float deltatime)
    {
      timer_ += deltatime;

      if (connection_setup_ == ConnectionSetup::kNone)
      {
        return true;
      }

      float tick_rate_seconds = (1000.0f / tick_rate_) / 1000.0f;
      if (timer_ < last_tick_update_ + tick_rate_seconds)
      {
        return true;
      }
      last_tick_update_ += tick_rate_seconds;

      float connection_timeout_seconds = connection_timeout_ * 0.001f;
      if (connection_status_ == ConnectionStatus::kConnecting)
      {
        if (timer_ >= connection_timeout_seconds)
        {
          PS_LOG_WITH(NetworkLogger, Warning, "Connection timed out.\n");
          ForceDisconnect();
          if (on_failed_to_connect_ != nullptr)
          {
            on_failed_to_connect_();
          }
        }
      }
      else if (connection_status_ == ConnectionStatus::kDisconnecting)
      {
        if (timer_ >= connection_timeout_seconds)
        {
          ForceDisconnect();
          if (on_disconnected_from_server_ != nullptr)
          {
            on_disconnected_from_server_(DisconnectionInfo::kLostConnection);
          }
        }
      }

      if (is_connected() == true)
      {
        CheckHeartbeats();
        UpdatePing();
      }

      ENetEvent network_event;
      int result;
      while ((result = enet_host_service(host_, &network_event, 0)) != 0)
      {
        if (result < 0)
        {
          if (is_connected() == false)
          {
            return false;
          }

          PS_LOG_WITH(NetworkLogger, Error,
            "Error occurred while checking for events - @Update()\n");
          return false;
        }

        switch (network_event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
          HandleConnectEvent(&network_event);
        }
        break;
        case ENET_EVENT_TYPE_DISCONNECT:
        {
          HandleDisconnectEvent(&network_event);
        }
        break;
        case ENET_EVENT_TYPE_RECEIVE:
        {
          HandleRecieveEvent(&network_event);
        }
        break;
        default:
          break;
        }
      }
      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::LateUpdate()
    {
      if (connection_setup_ == ConnectionSetup::kNone)
      {
        return true;
      }

      float tick_rate_seconds = (1000.0f / tick_rate_) / 1000.0f;
      if (timer_ < last_tick_late_update_ + tick_rate_seconds)
      {
        return true;
      }
      last_tick_late_update_ += tick_rate_seconds;

      if (is_connected())
      {
        value_syncer_->Sync();
      }
      packet_handler_->DispatchMessages();
      enet_host_flush(host_);
      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::Connect(const char* ip, unsigned int port)
    {
      if (connection_status_ == ConnectionStatus::kConnecting ||
        connection_status_ == ConnectionStatus::kConnected)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Already connecting or connected - @Connect()\n");
        return false;
      }

      connection_status_ = ConnectionStatus::kDisconnected;
      ENetAddress address;
      if (enet_address_set_host(&address, ip) < 0)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Failed to convert hostname - @Connect()\n");
        return false;
      }
      address.port = static_cast<enet_uint16>(port);

      network_player_->set_peer(enet_host_connect(host_, &address, 2, 0));
      if (network_player_->peer() == nullptr)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "No available host for initiating a connection - @Connect()\n");
        return false;
      }

      connection_status_ = ConnectionStatus::kConnecting;
      timer_ = 0.0f;
      last_tick_update_ = 0;
      last_tick_late_update_ = 0;
      enet_host_flush(host_);
      return true;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::Disconnect()
    {
      if (connection_status_ == ConnectionStatus::kDisconnecting ||
        connection_status_ == ConnectionStatus::kDisconnected)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Already disconnected or disconnecting - @Disconnect()\n");
        return false;
      }

      enet_peer_disconnect(network_player_->peer(),
        static_cast<unsigned int>(DisconnectionInfo::kDisconnected));
      connection_status_ = ConnectionStatus::kDisconnecting;
      timer_ = 0.0f;
      last_tick_update_ = 0;
      last_tick_late_update_ = 0;
      if (network_player_list_.size() > 1)
      {
        foundation::List<NetworkPlayer>::iterator iter =
          network_player_list_.begin();
        iter++;
        network_player_list_.erase(iter, network_player_list_.end());
      }

      rpc_system_->SortIDs();
      value_syncer_->SortIDs();
      return true;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetName(const char* name)
    {
      if (network_player_ == nullptr)
      {
        return;
      }

      if (strlen(name) <= 0)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Name should not be empty - @SetName()\n");
        return;
      }

      network_player_->set_name(name);

      if (connection_setup_ == ConnectionSetup::kHost)
      {
        packet_handler_->HostSyncName(network_player_->name(),
          network_player_->id());
      }
      else if (connection_setup_ == ConnectionSetup::kClient)
      {
        Message message;
        uint8_t name_length = static_cast<uint8_t>(
          strlen(network_player_->name()));
        memcpy(&message.data[0], &name_length, 1);
        memcpy(&message.data[1], network_player_->name(),
          strlen(network_player_->name()));
        message.peer = network_player_->peer();
        message.size = strlen(network_player_->name()) + 1;
        if (network_player_->peer() != nullptr)
        {
          packet_handler_->ClientSendName(message);
        }
      }
    }

    //-------------------------------------------------------------------------
    const char* NetworkingSystem::GetName()
    {
      if (connection_setup_ == ConnectionSetup::kNone)
      {
        return nullptr;
      }

      return network_player_->name();
    }

    //-------------------------------------------------------------------------
    uint8_t NetworkingSystem::GetID()
    {
      if (connection_setup_ == ConnectionSetup::kNone || is_connected() == false)
      {
        return 0;
      }

      return network_player_->id();
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::is_connected() const
    {
      return connection_status_ == ConnectionStatus::kConnected ||
        connection_status_ == ConnectionStatus::kDisconnecting;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::is_host() const
    {
      return connection_setup_ == ConnectionSetup::kHost;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::is_client() const
    {
      return connection_setup_ == ConnectionSetup::kClient;
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkingSystem::num_players() const
    {
      return static_cast<unsigned int>(network_player_list_.size());
    }

    //-------------------------------------------------------------------------
    foundation::Vector<NetworkPlayerData> NetworkingSystem::GetNetworkPlayers()
    {
      foundation::Vector<NetworkPlayerData> players;

      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        NetworkPlayerData data_;
        memset(data_.name, '\0', kNameLength);
        strcpy_s(data_.name, kNameLength, iter->name());
        data_.id = iter->id();
        data_.ping = iter->round_trip_time();
        players.push_back(data_);
      }
      return players;
    }

    //-------------------------------------------------------------------------
    ConnectionSetup NetworkingSystem::connection_setup() const
    {
      return connection_setup_;
    }

    //-------------------------------------------------------------------------
    ConnectionStatus NetworkingSystem::connection_status() const
    {
      return connection_status_;
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::KickClient(uint8_t id)
    {
      if (is_host() == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Failed to kick client: we are not a host.\n");
        return false;
      }

      if (id == 0)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Failed to kick client: we cant kick ourselves.\n");
        return false;
      }

      NetworkPlayer* player = FindNetworkPlayerByID(id);
      if (player != nullptr)
      {
        enet_peer_disconnect(player->peer(),
          static_cast<unsigned int>(DisconnectionInfo::kKicked));
        FreeID(id);
        return true;
      }

      PS_LOG_WITH(NetworkLogger, Warning,
        "Failed to kick client: no client matches with id.\n");
      return false;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetConnectionTimeout(unsigned int rate)
    {
      if (rate < 1000)
      {
        rate = 1000;
        PS_LOG_WITH(NetworkLogger, Warning,
          "Timeout rate below 1000 ms, atleast 1000 ms is required. Forcing it to 1000ms\n");
      }
      connection_timeout_ = rate;
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkingSystem::connection_timeout() const
    {
      return connection_timeout_;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetTickRate(unsigned int tickrate)
    {
      tick_rate_ = tickrate;
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkingSystem::tick_rate()
    {
      return tick_rate_;
    }

    //-------------------------------------------------------------------------
    SyncValueID NetworkingSystem::CreateSyncValue(const NetworkValueType type,
      NetworkValueOwner owner)
    {
      return value_syncer_->CreateValue(type, owner);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::RemoveSyncValue(SyncValueID id)
    {
      value_syncer_->RemoveValue(id);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ValidateValue(SyncValueID id)
    {
      value_syncer_->ValidateValue(id);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ValidateAllValues()
    {
      value_syncer_->ValidateAllValues();
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetOnConnectedToServerCallback(
      eastl::function<void()> function)
    {
      on_connected_to_server_ = function;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetOnDisconnectedFromServerCallback(
      eastl::function<void(DisconnectionInfo)> function)
    {
      on_disconnected_from_server_ = function;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetOnPlayerConnected(
      eastl::function<void(NetworkPlayerData)> function)
    {
      on_player_connected_ = function;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetOnPlayerDisconnected(
      eastl::function<void(NetworkPlayerData)> function)
    {
      on_player_disconnected_ = function;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetOnFailedToConnect(eastl::function<void()> function)
    {
      on_failed_to_connect_ = function;
    }

    //-------------------------------------------------------------------------
    unsigned int NetworkingSystem::GetPing(uint8_t id)
    {
      if (is_connected() == true)
      {
        if ((is_host() == true && id != 0))
        {
          NetworkPlayer* player = FindNetworkPlayerByID(id);
          if (player == nullptr)
          {
            return 0;
          }
          return player->round_trip_time();
        }
        else if (is_client() == true)
        {
          return network_player_->round_trip_time();
        }
        else
        {
          PS_LOG_WITH(NetworkLogger, Warning, "Requesting ping from ID that is not the host");
          return 0;
        }
      }
      return 0;
    }

    //-------------------------------------------------------------------------
    RPCHandle NetworkingSystem::RegisterRPC(eastl::function<void(RPCHandle, NetworkPlayerData, 
      const foundation::Vector<NetworkValue>&)> function, 
      const foundation::Vector<NetworkValueType>& arguments)
    {
      return rpc_system_->RegisterRPC(function, arguments);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::UnregisterRPC(const RPCHandle handle)
    {
      rpc_system_->UnregisterRPC(handle);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::InvokeRPC(const RPCHandle handle, const RPCMode mode, 
      const foundation::Vector<NetworkValue>& arguments)
    {
      rpc_system_->InvokeRPC(handle, mode, arguments);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ValidateRPC(uint16_t id)
    {
      rpc_system_->ValidateRPC(id);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ValidateAllRPCs()
    {
      rpc_system_->ValidateAllRPCs();
    }

    //-------------------------------------------------------------------------
    NetworkPlayer* NetworkingSystem::network_player()
    {
      return network_player_;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetSyncValue(SyncValueID id, const NetworkValue& value)
    {
      value_syncer_->SetValue(id, value);
    }

    //-------------------------------------------------------------------------
    bool NetworkingSystem::GetSyncValue(SyncValueID id, NetworkValue * value)
    {
      return value_syncer_->GetValue(id, value);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ClearRPCBuffer()
    {
      rpc_system_->ClearRPCBuffer();
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SortIDS()
    {
      rpc_system_->SortIDs();
      value_syncer_->SortIDs();
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ClientUpdateName(uint8_t* data)
    {
      const uint8_t& id = data[0];
      const uint8_t& name_length = data[1];
      const char* begin = static_cast<const char*>(static_cast<void *>(
        &data[2]));
      foundation::String name(begin, begin + name_length);

      NetworkPlayer* player = FindNetworkPlayerByID(id);
      if (player != nullptr)
      {
        player->set_name(name.c_str());
      }
      else
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Could not find networkplayer - @ClientHostAssignName()\n");
        return;
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::HandleConnectEvent(ENetEvent * network_event)
    {
      char hostname[128];
      char hostip[128];
      enet_address_get_host(&network_event->peer->address, hostname, 128);
      enet_address_get_host_ip(&network_event->peer->address, hostip, 128);
      PS_LOG_WITH(NetworkLogger, Debug, "[EVENT CONNECT] %s %s %u\n",
        hostname, hostip, network_event->peer->address.port);

      enet_peer_timeout(network_event->peer, 0, connection_timeout_,
        connection_timeout_);

      if (connection_setup_ == ConnectionSetup::kHost)
      {
        foundation::String ip_(hostip);
        ip_.append(":");
        ip_.append(foundation::to_string(
          network_event->peer->address.port).c_str());
        NetworkPlayer pl(GetNewID(), network_event->peer, "", ip_.c_str(),
          timer_);
        network_player_list_.push_back(pl);
      }
      else if (connection_setup_ == ConnectionSetup::kClient)
      {
        network_player_->set_last_heartbeat(timer_);
        network_player_->set_last_heartbeat_sent(timer_);
        uint8_t name_size = static_cast<uint8_t>(
          strlen(network_player_->name()));

        Message message;
        memcpy(&message.data[0], &name_size, 1);
        memcpy(&message.data[1], network_player_->name(),
          strlen(network_player_->name()));
        message.peer = network_player_->peer();
        message.size = strlen(network_player_->name()) + 1;
        packet_handler_->ClientSendName(message);
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::HandleDisconnectEvent(ENetEvent* network_event)
    {
      char hostname[128];
      char hostip[128];
      enet_address_get_host(&network_event->peer->address, hostname, 128);
      enet_address_get_host_ip(&network_event->peer->address, hostip, 128);
      PS_LOG_WITH(NetworkLogger, Debug, "[EVENT DISCONNECT] %s %s %u\n",
        hostname, hostip, network_event->peer->address.port);
      if (connection_setup_ == ConnectionSetup::kHost)
      {
        foundation::List<NetworkPlayer>::iterator player =
          FindNetworkPlayerIteratorByPeer(network_event->peer);

        if (on_player_disconnected_ != nullptr)
        {
          NetworkPlayerData npd;
          memset(npd.name, '\0', kNameLength);
          memcpy(npd.name, player->name(), strlen(player->name()));
          npd.id = player->id();
          npd.ping = player->round_trip_time();
          on_player_disconnected_(npd);
        }

        FreeID(player->id());
        if (player != network_player_list_.end())
        {
          packet_handler_->HostRemovePlayer(player->id());
          network_player_list_.erase(player);
        }
      }
      else if (connection_setup_ == ConnectionSetup::kClient)
      {
        connection_status_ = ConnectionStatus::kDisconnected;
        ForceDisconnect();
        if (on_disconnected_from_server_ != nullptr)
        {
          on_disconnected_from_server_(
            static_cast<DisconnectionInfo>(network_event->data));
        }
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::HandleRecieveEvent(ENetEvent * network_event)
    {
      size_t length = network_event->packet->dataLength;
      packet_handler_->ProcessPacket(network_event->peer,
        network_event->packet->data, length);
      enet_packet_destroy(network_event->packet);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::HostAssignName(uint8_t* data, ENetPeer* peer)
    {
      const uint8_t& name_length = data[0];
      const char* begin = static_cast<const char*>(static_cast<void *>(&data[1]));
      foundation::String name(begin, begin + name_length);

      bool already_exists = false;
      NetworkPlayer* player = FindNetworkPlayerByPeer(peer);
      if (player != nullptr)
      {
        if (foundation::String(player->name()) != "") //dirty check
        {
          already_exists = true;
        }
        player->set_name(name.c_str());
      }
      else
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Could not find networkplayer - @ClientHostAssignName()\n");
        return;
      }

      if (already_exists == true)
      {
        //if we already exist, we just need to update the name
        packet_handler_->HostSyncName(player->name(), player->id());
        return;
      }

      Message id_message;
      uint8_t id = player->id();
      memcpy(id_message.data, &id, 1);
      id_message.peer = player->peer();
      id_message.size = 1;

      packet_handler_->HostSendID(id_message);

      //send the playerlist to the new player
      packet_handler_->HostAddPlayerSync(player->peer());

      //update everyone about new player
      packet_handler_->HostAddPlayer(player->id(), player->name(),
        player->peer());

      rpc_system_->SendRPCBuffer(player->peer());

      NetworkPlayerData np_data = {};
      np_data.id = id;
      np_data.ping = player->round_trip_time();
      memset(np_data.name, '\0', kNameLength);
      memcpy(np_data.name, player->name(), strlen(player->name()));
      if (on_player_connected_ != nullptr)
      {
        on_player_connected_(np_data);
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ClientAssignID(uint8_t id)
    {
      network_player_->set_id(id);
      connection_status_ = ConnectionStatus::kConnected;
      if (on_connected_to_server_ != nullptr)
      {
        on_connected_to_server_();
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ClientAddPlayer(const char* name, uint8_t id)
    {
      float current_time = timer_;
      NetworkPlayer player(id, nullptr, name, "", current_time);
      network_player_list_.push_back(player);

      NetworkPlayerData data = {};
      data.id = id;
      data.ping = player.round_trip_time();
      memset(data.name, '\0', kNameLength);
      memcpy(data.name, name, strlen(name));
      if (on_player_connected_ != nullptr)
      {
        on_player_connected_(data);
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ClientRemovePlayer(uint8_t id)
    {
      foundation::List<NetworkPlayer>::iterator player =
        FindNetworkPlayerIteratorByID(id);
      if (player == network_player_list_.end())
      {
        return;
      }

      NetworkPlayerData data = {};
      data.id = id;
      data.ping = player->round_trip_time();
      memset(data.name, '\0', kNameLength);
      memcpy_s(data.name, kNameLength, player->name(), strlen(player->name()));
      if (on_player_disconnected_ != nullptr)
      {
        on_player_disconnected_(data);
      }
      network_player_list_.erase(player);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::SetLastHeartBeat(NetworkPlayer * player)
    {
      float heartbeat_time = timer_;
      if (connection_setup_ == ConnectionSetup::kClient)
      {
        network_player_->set_last_heartbeat(heartbeat_time);
      }
      else if (connection_setup_ == ConnectionSetup::kHost)
      {
        if (player != nullptr)
        {
          player->set_last_heartbeat(heartbeat_time);
        }
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::CheckHeartbeats()
    {
      float current_time = timer_;
      if (connection_setup_ == ConnectionSetup::kHost)
      {
        foundation::List<NetworkPlayer>::iterator iter;
        foundation::Vector<foundation::List<NetworkPlayer>::iterator>
          to_be_removed;
        for (iter = network_player_list_.begin(); iter !=
          network_player_list_.end(); ++iter)
        {
          if (&*iter != network_player_)
          {
            //check if we need to beat again
            if (iter->last_heartbeat_sent() + heartbeat_frequency_ <
              current_time)
            {
              //send beat
              iter->set_last_heartbeat_sent(current_time);
              packet_handler_->ClientHostSendHeartbeat(iter->peer());
            }

            if (iter->last_heartbeat() + heartbeat_timeout_ < current_time)
            {
              to_be_removed.push_back(iter);
            }
          }
        }

        for (unsigned int i = 0; i < to_be_removed.size(); ++i)
        {
          packet_handler_->HostRemovePlayer(to_be_removed[i]->id());

          NetworkPlayerData data = {};
          data.id = to_be_removed[i]->id();
          data.ping = to_be_removed[i]->round_trip_time();
          memset(data.name, '\0', kNameLength);
          memcpy(data.name, to_be_removed[i]->name(),
            strlen(to_be_removed[i]->name()));
          if (on_player_disconnected_ != nullptr)
          {
            on_player_disconnected_(data);
          }

          network_player_list_.erase(to_be_removed[i]);
        }
      }
      else if (connection_setup_ == ConnectionSetup::kClient)
      {
        //check heartbeat with host
        if (network_player_->last_heartbeat() + heartbeat_timeout_ <
          current_time)
        {
          ForceDisconnect();
          if (on_disconnected_from_server_ != nullptr)
          {
            on_disconnected_from_server_(DisconnectionInfo::kLostConnection);
          }
        }
      }
    }


    //-------------------------------------------------------------------------
    void NetworkingSystem::ForceDisconnect()
    {
      timer_ = 0.0f;
      last_tick_update_ = 0.0f;
      last_tick_late_update_ = 0.0f;
      enet_peer_disconnect_now(network_player_->peer(),
        static_cast<unsigned int>(DisconnectionInfo::kDisconnected));
      connection_status_ = ConnectionStatus::kDisconnected;
      network_player_->set_peer(nullptr);
      if (network_player_list_.size() > 1)
      {
        foundation::List<NetworkPlayer>::iterator iter =
          network_player_list_.begin();
        iter++;
        network_player_list_.erase(iter, network_player_list_.end());
      }
      rpc_system_->SortIDs();
      value_syncer_->SortIDs();
    }

    //-------------------------------------------------------------------------
    uint8_t NetworkingSystem::GetNewID()
    {
      uint8_t id = available_ids_.front();
      available_ids_.pop_front();
      return id;
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::ResetIDPool()
    {
      available_ids_.clear();
      for (unsigned int i = 1; i < 256u; ++i)
      {
        uint8_t value = static_cast<uint8_t>(i);
        available_ids_.push_back(value);
      }
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::FreeID(uint8_t id)
    {
      available_ids_.push_back(id);
    }

    //-------------------------------------------------------------------------
    void NetworkingSystem::UpdatePing()
    {
      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        if (iter->peer() != nullptr)
        {
          iter->set_round_trip_time(iter->peer()->roundTripTime);
        }
      }
    }

    //-------------------------------------------------------------------------
    NetworkPlayer* NetworkingSystem::FindNetworkPlayerByPeer(ENetPeer* peer)
    {
      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        if (iter->peer() == peer)
        {
          return &*iter;
        }
      }
      return nullptr;
    }

    //-------------------------------------------------------------------------
    NetworkPlayer* NetworkingSystem::FindNetworkPlayerByID(uint8_t id)
    {
      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        if (iter->id() == id)
        {
          return &*iter;
        }
      }
      return nullptr;
    }

    //-------------------------------------------------------------------------
    foundation::List<NetworkPlayer>::iterator
      NetworkingSystem::FindNetworkPlayerIteratorByPeer(ENetPeer* peer)
    {
      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        if (iter->peer() == peer)
        {
          return iter;
        }
      }
      return network_player_list_.end();
    }

    //-------------------------------------------------------------------------
    foundation::List<NetworkPlayer>::iterator
      NetworkingSystem::FindNetworkPlayerIteratorByID(uint8_t id)
    {
      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = network_player_list_.begin(); iter !=
        network_player_list_.end(); ++iter)
      {
        if (iter->id() == id)
        {
          return iter;
        }
      }
      return network_player_list_.end();
    }
  }
}
