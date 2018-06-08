#define PS_NETWORKING_API_EXPORT
#include "tools/networking/export.h"
#include "tools/networking/networking_system.h"
#include <enet/enet.h>
#include <foundation/memory/memory.h>
#include <foundation/utils/lib_loader.h>

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    void SNetConstruct(size_t size)
    {
      if (network_manager_ == nullptr)
      {
        foundation::LibLoader::AllocateSharedMemory(size);
        network_manager_ = foundation::Memory::Construct<NetworkingSystem>();
      }
    }

    //-------------------------------------------------------------------------
    void SNetDestruct()
    {
      foundation::Memory::Destruct<NetworkingSystem>(network_manager_);
      network_manager_ = nullptr;
    }

    //-------------------------------------------------------------------------
    bool SNetInitSystem()
    {
      return network_manager_->InitSystem();
    }

    //-------------------------------------------------------------------------
    void SNetDestroySystem()
    {
      network_manager_->DestroySystem();
    }

    //-------------------------------------------------------------------------
    bool SNetInitHost(unsigned int port, unsigned int max_clients,
      const foundation::String& player_name)
    {
      return network_manager_->InitHost(port, max_clients, 
        player_name.c_str());
    }

    //-------------------------------------------------------------------------
    bool SNetInitClient(const foundation::String& name)
    {
      return network_manager_->InitClient(name.c_str());
    }

    //-------------------------------------------------------------------------
    bool SNetDestroyHostOrClient()
    {
      return network_manager_->DestroyHostOrClient();
    }

    //-------------------------------------------------------------------------
    bool SNetUpdate(float deltatime)
    {
      return network_manager_->Update(deltatime);
    }

    //-------------------------------------------------------------------------
    bool SNetLateUpdate()
    {
      return network_manager_->LateUpdate();
    }

    //-------------------------------------------------------------------------
    bool SNetConnect(const foundation::String& ip, unsigned int port)
    {
      return network_manager_->Connect(ip.c_str(), port);
    }

    //-------------------------------------------------------------------------
    bool SNetDisconnect()
    {
      return network_manager_->Disconnect();
    }

    //-------------------------------------------------------------------------
    void SNetSetName(const foundation::String& name)
    {
      network_manager_->SetName(name.c_str());
    }

    //-------------------------------------------------------------------------
    foundation::String SNetGetName()
    {
      const char* name = network_manager_->GetName();
      if (name == nullptr)
      {
        return "";
      }
      return network_manager_->GetName();
    }

    //-------------------------------------------------------------------------
    unsigned int SNetGetID()
    {
      return static_cast<unsigned int>(network_manager_->GetID());
    }

    //-------------------------------------------------------------------------
    bool SNetIsConnected()
    {
      return network_manager_->is_connected();
    }

    //-------------------------------------------------------------------------
    bool SNetIsHost()
    {
      return network_manager_->is_host();
    }

    //-------------------------------------------------------------------------
    bool SNetIsClient()
    {
      return network_manager_->is_client();
    }

    //-------------------------------------------------------------------------
    unsigned int SNetGetNumClients()
    {
      return network_manager_->num_players();
    }

    //-------------------------------------------------------------------------
    foundation::Vector<NetworkPlayerData> SNetGetNetworkPlayers()
    {
      return network_manager_->GetNetworkPlayers();
    }

    //-------------------------------------------------------------------------
    ConnectionSetup SNetConnectionSetup()
    {
      return network_manager_->connection_setup();
    }

    //-------------------------------------------------------------------------
    ConnectionStatus SNetConnectionStatus()
    {
      return network_manager_->connection_status();
    }

    //-------------------------------------------------------------------------
    bool SNetKickClient(uint8_t id)
    {
      return network_manager_->KickClient(id);
    }

    //-------------------------------------------------------------------------
    void SNetSetConnectionTimeout(unsigned int rate)
    {
      network_manager_->SetConnectionTimeout(rate);
    }

    //-------------------------------------------------------------------------
    unsigned int SNetGetConnectionTimeout()
    {
      return network_manager_->connection_timeout();
    }

    //-------------------------------------------------------------------------
    void SNetSetTickRate(unsigned int tickrate)
    {
      network_manager_->SetTickRate(tickrate);
    }

    //-------------------------------------------------------------------------
    unsigned int SNetGetTickRate()
    {
      return network_manager_->tick_rate();
    }

    //-------------------------------------------------------------------------
    uint16_t SNetCreateSyncValue(const NetworkValueType type, NetworkValueOwner owner)
    {
      return network_manager_->CreateSyncValue(type, owner);
    }

    //-------------------------------------------------------------------------
    void SNetRemoveSyncValue(uint16_t id)
    {
      network_manager_->RemoveSyncValue(id);
    }

    //-------------------------------------------------------------------------
    void SNetValidateValue(uint16_t id)
    {
      network_manager_->ValidateValue(id);
    }

    //-------------------------------------------------------------------------
    void SNetValidateAllValues()
    {
      network_manager_->ValidateAllValues();
    }

    //-------------------------------------------------------------------------
    void SNetSetOnConnectedToServerCallback(eastl::function<void()> function)
    {
      network_manager_->SetOnConnectedToServerCallback(function);
    }

    //-------------------------------------------------------------------------
    void SNetSetOnDisconnectedFromServerCallback(
      eastl::function<void(DisconnectionInfo)> function)
    {
      network_manager_->SetOnDisconnectedFromServerCallback(function);
    }

    //-------------------------------------------------------------------------
    void SNetSetOnPlayerConnected(
      eastl::function<void(NetworkPlayerData)> function)
    {
      network_manager_->SetOnPlayerConnected(function);
    }

    //-------------------------------------------------------------------------
    void SNetSetOnPlayerDisconnected(
      eastl::function<void(NetworkPlayerData)> function)
    {
      network_manager_->SetOnPlayerDisconnected(function);
    }

    //-------------------------------------------------------------------------
    void SNetSetOnFailedToConnect(eastl::function<void()> function)
    {
      network_manager_->SetOnFailedToConnect(function);
    }

    //-------------------------------------------------------------------------
    unsigned int SNetGetPing(uint8_t id)
    {
      return network_manager_->GetPing(id);
    }

    //-------------------------------------------------------------------------
    RPCHandle SNetRegisterRPC(eastl::function<void(RPCHandle, NetworkPlayerData,
      const foundation::Vector<NetworkValue>&)> function, 
      const foundation::Vector<NetworkValueType>& arguments)
    {
      return network_manager_->RegisterRPC(function, arguments);
    }

    //-------------------------------------------------------------------------
    void SNetUnregisterRPC(const RPCHandle handle)
    {
      network_manager_->UnregisterRPC(handle);
    }

    //-------------------------------------------------------------------------
    void SNetInvokeRPC(const RPCHandle handle, const RPCMode mode,
      const foundation::Vector<NetworkValue>& arguments)
    {
      network_manager_->InvokeRPC(handle, mode, arguments);
    }

    //-------------------------------------------------------------------------
    void SNetValidateRPC(uint16_t id)
    {
      network_manager_->ValidateRPC(id);
    }

    //-----------------------------------------------------------------------
    void SNetValidateAllRPCs()
    {
      network_manager_->ValidateAllRPCs();
    }

    //-------------------------------------------------------------------------
    void SNetSetSyncValue(SyncValueID id, const NetworkValue& value)
    {
      return network_manager_->SetSyncValue(id, value);
    }

    //-------------------------------------------------------------------------
    bool SNetGetSyncValue(SyncValueID id, NetworkValue* value)
    {
      return network_manager_->GetSyncValue(id, value);
    }

    //-------------------------------------------------------------------------
    void SNetClearRPCBuffer()
    {
      network_manager_->ClearRPCBuffer();
    }

    //-------------------------------------------------------------------------
    void SNetSortIDs()
    {
      network_manager_->SortIDS();
    }

    namespace editor
    {
      //-----------------------------------------------------------------------
      ErrorMessage SNetInitEngine(unsigned int port)
      {
        connection_data.host = nullptr;
        connection_data.peer = nullptr;

        if (enet_initialize() != 0)
        {
          return ErrorMessage::kEnetInitFailed;
        }

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = static_cast<enet_uint16>(port);

        connection_data.host = enet_host_create(&address, 1, 1, 0, 0);

        if (connection_data.host == nullptr)
        {
          enet_deinitialize();
          return ErrorMessage::kHostCreateFailed;
        }

        connection_data.peer = nullptr;
        return ErrorMessage::kOk;
      }

      //-----------------------------------------------------------------------
      ErrorMessage SNetInitEditor()
      {
        connection_data.host = nullptr;
        connection_data.peer = nullptr;

        if (enet_initialize() != 0)
        {
          return ErrorMessage::kEnetInitFailed;
        }

        connection_data.host = enet_host_create(nullptr, 1, 1, 0, 0);
        if (connection_data.host == nullptr)
        {
          enet_deinitialize();
          return ErrorMessage::kHostCreateFailed;
        }

        return ErrorMessage::kOk;
      }

      //-----------------------------------------------------------------------
      ErrorMessage SNetEditorConnect(const char* ip, unsigned int port)
      {
        ENetAddress address;
        if (enet_address_set_host(&address, ip) < 0)
        {
          return ErrorMessage::kResolveIpFailed;
        }
        address.port = static_cast<enet_uint16>(port);

        ENetPeer* temp_peer = 
          enet_host_connect(connection_data.host, &address, 1, 0);
        if (temp_peer == nullptr)
        {
          return ErrorMessage::kConnectFailed;
        }

        SNetFlushPackets();
        return ErrorMessage::kOk;
      }

      //-----------------------------------------------------------------------
      bool SNetRetrievePacket(uint32_t* id, void* data, unsigned int capacity)
      {
        if (connection_data.host == nullptr)
        {
          return false;
        }

        ENetEvent network_event;
        int result;
        if ((result = enet_host_service(connection_data.host, 
          &network_event, 0)) != 0)
        {
          if (result < 0)
          {
            return false;
          }

          switch (network_event.type)
          {
          case ENET_EVENT_TYPE_CONNECT:
          {
            connection_data.peer = network_event.peer;
            return false;
          }
          break;
          case ENET_EVENT_TYPE_DISCONNECT:
          {
            if (connection_data.peer != nullptr)
            {
              enet_peer_reset(connection_data.peer);
            }
            connection_data.peer = nullptr;
            return false;
          }
          break;
          case ENET_EVENT_TYPE_RECEIVE:
          {
            uint32_t source_size = 
              static_cast<uint32_t>(network_event.packet->dataLength);

            const uint32_t payload_size = source_size - sizeof(uint32_t);
            if (capacity < payload_size)
            {
              source_size = capacity;
            }

            *id = *network_event.packet->data;
            const enet_uint8* payload_src = 
              network_event.packet->data + sizeof(uint32_t);
            if (payload_size > 0 && capacity > 0)
            {
              memcpy_s(data, capacity, payload_src, source_size);
            }

            enet_packet_destroy(network_event.packet);
            return true;
          }
          break;
          default:
            return false;
            break;
          }
        }
        return false;
      }

      //-----------------------------------------------------------------------
      void SNetFlushPackets()
      {
        enet_host_flush(connection_data.host);
      }

      //-----------------------------------------------------------------------
      ErrorMessage SNetSendData(uint32_t id, const void* actual_data, 
        unsigned int size)
      {
        enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE;

        enet_uint8 packet_buff[kMaxPacketSize];
        const size_t packet_size = sizeof(uint32_t) + size;

        enet_uint8* id_dst = packet_buff;
        enet_uint8* payload_dst = packet_buff + sizeof(uint32_t);

        memcpy_s(id_dst, sizeof(uint32_t), &id, sizeof id);
        memcpy_s(payload_dst, kMaxPayloadSize, actual_data, size);

        ENetPacket* packet = 
          enet_packet_create(packet_buff, packet_size, flags);

        if (packet == nullptr)
        {
          return ErrorMessage::kPacketCreationFailed;
        }

        if (connection_data.peer == nullptr)
        {
          return ErrorMessage::kNoPeerConnected;
        }

        int result = enet_peer_send(connection_data.peer, 0, packet);
        if (result < 0)
        {
          return ErrorMessage::kPacketSendFailed;
        }

        return ErrorMessage::kOk;
      }

      //-----------------------------------------------------------------------
      void SNetDestroy()
      {
        if (connection_data.peer != nullptr)
        {
          enet_peer_reset(connection_data.peer);
        }
        if (connection_data.host != nullptr)
        {
          enet_host_destroy(connection_data.host);
        }
        enet_deinitialize();
        connection_data.host = nullptr;
        connection_data.peer = nullptr;
      }

      //-----------------------------------------------------------------------
      bool SNetIsConnected()
      {
        return connection_data.peer != nullptr;
      }
    }
  }
}