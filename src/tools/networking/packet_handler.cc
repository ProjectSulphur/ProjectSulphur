#include "tools/networking/packet_handler.h"
#include "tools/networking/networking_system.h"
#include "tools/networking/value_syncer.h"
#include <enet/enet.h>
#include <foundation/containers/map.h>

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    PacketHandler::PacketHandler(NetworkingSystem* system) :
      system_(system)
    {

    }

    //-------------------------------------------------------------------------
    PacketHandler::~PacketHandler()
    {

    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientSendName(const Message& message)
    {
      QueueMessage(message, MessageDataType::kName);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostSendName(PacketDelivery delivery,
      const Message& message)
    {
      if (delivery == PacketDelivery::kSend)
      {
        QueueMessage(message, MessageDataType::kUpdateName);
      }
      else if (delivery == PacketDelivery::kBroadcast)
      {
        QueueBroadcastMessage(message, MessageDataType::kUpdateName);
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostSyncName(const char* name, uint8_t id)
    {
      size_t name_size = strlen(name);
      uint8_t name_size_byte = static_cast<uint8_t>(name_size);
      Message message;
      memcpy(&message.data[0], &id, 1);
      memcpy(&message.data[1], &name_size_byte, 1);
      memcpy(&message.data[2], name, name_size);
      message.peer = nullptr;
      message.size = name_size + 2;
      HostSendName(PacketDelivery::kBroadcast, message);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostSendID(const Message& message)
    {
      QueueMessage(message, MessageDataType::kPlayerID);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostAddPlayerSync(ENetPeer* peer)
    {
      Message message;
      message.peer = peer;

      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = system_->network_player_list_.begin(); iter !=
        system_->network_player_list_.end(); ++iter)
      {
        if (iter->peer() != peer)
        {
          ConstructAddPlayerData(iter->name(), iter->id(), message.data,
            &message.size);
          QueueMessage(message, MessageDataType::kPlayerAdd);
        }
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostAddPlayer(uint8_t id, const char* name,
      ENetPeer* peer)
    {
      Message message;
      message.peer = nullptr;
      ConstructAddPlayerData(name, id, message.data, &message.size);

      foundation::List<NetworkPlayer>::iterator iter;
      for (iter = system_->network_player_list_.begin(); iter !=
        system_->network_player_list_.end(); ++iter)
      {
        if (iter->peer() != peer && iter->peer() != nullptr)
        {
          message.peer = iter->peer();
          QueueMessage(message, MessageDataType::kPlayerAdd);
        }
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostRemovePlayer(uint8_t id)
    {
      Message message;
      message.peer = nullptr;
      memcpy(message.data, &id, 1);
      message.size = 1;

      QueueBroadcastMessage(message, MessageDataType::kPlayerDelete);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostClientSyncValue(uint16_t id,
      const NetworkValue value, ENetPeer* peer_to_exclude)
    {
      Message message;
      message.peer = nullptr;
      message.size = 2;
      message.reliable = false;
      memcpy(&message.data[0], &id, 2);
      switch (value.type)
      {
      case NetworkValueType::kFloat:
        memcpy(&message.data[2], &value.f, sizeof(float));
        message.size += sizeof(float);
        break;
      case NetworkValueType::kDouble:
        memcpy(&message.data[2], &value.d, sizeof(double));
        message.size += sizeof(double);
        break;
      case NetworkValueType::kInt:
        memcpy(&message.data[2], &value.i, sizeof(int));
        message.size += sizeof(int);
        break;
      case NetworkValueType::kUnsignedInt:
        memcpy(&message.data[2], &value.ui, sizeof(unsigned int));

        message.size += sizeof(unsigned int);
        break;
      case NetworkValueType::kBool:
        memcpy(&message.data[2], &value.b, sizeof(bool));
        message.size += sizeof(bool);
        break;
      case NetworkValueType::kVec2:
        memcpy(&message.data[2], &value.v2, sizeof(glm::vec2));
        message.size += sizeof(glm::vec2);
        break;
      case NetworkValueType::kVec3:
        memcpy(&message.data[2], &value.v3, sizeof(glm::vec3));
        message.size += sizeof(glm::vec3);
        break;
      case NetworkValueType::kVec4:
        memcpy(&message.data[2], &value.v4, sizeof(glm::vec4));
        message.size += sizeof(glm::vec4);
        break;
      }

      if (peer_to_exclude == nullptr)
      {
        QueueBroadcastMessage(message, MessageDataType::kValueSync);
      }
      else
      {
        QueueBroadcastMessageExceptOne(message, MessageDataType::kValueSync,
          peer_to_exclude);
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostValidateValue(uint16_t id, NetworkValueType type)
    {
      Message message;
      message.size = 3;
      memcpy(&message.data[0], &id, 2);
      memcpy(&message.data[2], &type, 1);

      QueueBroadcastMessage(message, MessageDataType::kValueSyncValidation);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientHostSendHeartbeat(ENetPeer* peer)
    {
      Message message;
      message.size = 0;
      message.peer = peer;
      QueueMessage(message, MessageDataType::kHeartBeat);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ProcessPacket(ENetPeer* peer, uint8_t* data,
      const size_t size)
    {
      size_t index = 0;
      while (index < size)
      {
        MessageDataType msg_type = static_cast<MessageDataType>(data[index]);
        ++index;
        size_t msg_size = ProcessMessage(&data[index], msg_type, peer);
        index += msg_size;
      }

      system_->SetLastHeartBeat(system_->FindNetworkPlayerByPeer(peer));
    }

    //-------------------------------------------------------------------------
    void PacketHandler::DispatchMessages()
    {
      foundation::VectorMap<PacketKey, Packet> packets;
      while (message_queue_.empty() == false)
      {
        const QueuedMessage& msg = message_queue_.front();
        foundation::VectorMap<PacketKey, Packet>::iterator iter =
          packets.find(PacketKey(msg.peer, msg.reliable));
        if (iter == packets.end())
        {
          Packet p;
          p.peer = msg.peer;
          p.size = 0;
          p.reliable = msg.reliable;
          iter = packets.insert(eastl::pair<PacketKey, Packet>(
            PacketKey(msg.peer, msg.reliable), p)).first;
        }
        else
        {
          size_t new_size = iter->second.size + msg.size;
          if (new_size > kMaxPacketSize)
          {
            SendPacket(iter->second);
            iter->second.size = 0;
            memset(iter->second.data, '\0', kMaxPacketSize);
          }
        }

        memcpy(&iter->second.data[iter->second.size], msg.data, msg.size);
        iter->second.size += msg.size;

        message_queue_.pop_front();
      }

      foundation::VectorMap<PacketKey, Packet>::iterator iter;
      for (iter = packets.begin(); iter != packets.end(); ++iter)
      {
        if (iter->second.size > 0)
        {
          SendPacket(iter->second);
        }
      }

      packets.clear();
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientProcessName(uint8_t* message_data)
    {
      system_->ClientUpdateName(message_data);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::HostProcessName(uint8_t* message_data, ENetPeer* peer)
    {
      system_->HostAssignName(message_data, peer);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientProcessID(uint8_t* message_data)
    {
      uint8_t id = message_data[0];
      system_->ClientAssignID(id);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientProcessAddPlayer(uint8_t* message_data)
    {
      uint8_t id = message_data[0];
      uint8_t name_length = message_data[1];
      char name[kNameLength];
      memset(name, '\0', kNameLength);
      memcpy(name, &message_data[2], name_length);

      system_->ClientAddPlayer(name, id);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientHostProcessHeartbeat(ENetPeer* peer)
    {
      if (system_->connection_setup() == ConnectionSetup::kClient)
      {
        ClientHostSendHeartbeat(peer);
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ClientProcessRemovePlayer(uint8_t* message_data)
    {
      uint8_t id = message_data[0];
      system_->ClientRemovePlayer(id);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::ConstructAddPlayerData(const char* name, uint8_t id,
      uint8_t* data, size_t* size)
    {
      uint8_t name_size = static_cast<uint8_t>(strlen(name));

      memcpy(data, &id, 1);
      memcpy(&data[1], &name_size, 1);
      memcpy(&data[2], name, name_size);
      *size = name_size + 2;
    }

    //-------------------------------------------------------------------------
    bool PacketHandler::SendPacket(const Packet& packet)
    {
      if (packet.peer == nullptr)
      {
        PS_LOG_WITH(NetworkLogger, Error, "Peer == nullptr - @SendPacket()");
        return false;
      }

      ENetPacket* p = CreatePacket(packet);
      if (p == nullptr)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Failed to create packet - @SendPacket()");
        return false;
      }

      int result = enet_peer_send(packet.peer, packet.reliable ? 0 : 1, p);
      if (result < 0)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Failed to send packet - @SendPacket()");
        return false;
      }

      return true;
    }

    //-------------------------------------------------------------------------
    void PacketHandler::QueueMessage(const Message& message,
      const MessageDataType type)
    {
      uint8_t identifier = static_cast<uint8_t>(type);
      QueuedMessage queued_message;
      queued_message.peer = message.peer;
      queued_message.size = message.size + 1;
      queued_message.reliable = message.reliable;
      memcpy(&queued_message.data[0], &identifier, 1);
      if (message.size > 0)
      {
        memcpy(&queued_message.data[1], &message.data, message.size);
      }
      message_queue_.push_back(queued_message);
    }

    //-------------------------------------------------------------------------
    void PacketHandler::QueueBroadcastMessage(const Message& message,
      const MessageDataType type)
    {
      ENetPeer* currentPeer;
      ENetHost* host = system_->host_;
      Message new_message(message);
      for (currentPeer = host->peers; currentPeer <
        &host->peers[host->peerCount]; ++currentPeer)
      {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED)
        {
          continue;
        }
        new_message.peer = currentPeer;
        QueueMessage(new_message, type);
      }
    }

    //-------------------------------------------------------------------------
    void PacketHandler::QueueBroadcastMessageExceptOne(const Message& message,
      const MessageDataType type, const ENetPeer* peer)
    {
      ENetPeer* currentPeer;
      ENetHost* host = system_->host_;
      Message new_message(message);
      for (currentPeer = host->peers; currentPeer <
        &host->peers[host->peerCount]; ++currentPeer)
      {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED
          || currentPeer == peer)
        {
          continue;
        }
        new_message.peer = currentPeer;
        QueueMessage(new_message, type);
      }
    }

    //-------------------------------------------------------------------------
    size_t PacketHandler::ProcessMessage(uint8_t* data,
      const MessageDataType type, ENetPeer* peer)
    {
      switch (type)
      {
      case MessageDataType::kName:
      {
        size_t name_size = static_cast<size_t>(data[0]);
        HostProcessName(data, peer);
        return name_size + 1;
      }
      break;
      case MessageDataType::kUpdateName:
      {
        size_t name_size = static_cast<size_t>(data[1]);
        ClientProcessName(data);
        return name_size + 2;
      }
      break;
      case MessageDataType::kPlayerID:
      {
        ClientProcessID(data);
        return 1;
      }
      break;
      case MessageDataType::kPlayerAdd:
      {
        size_t name_size = static_cast<size_t>(data[1]);
        ClientProcessAddPlayer(data);
        return name_size + 2;
      }
      break;
      case MessageDataType::kHeartBeat:
      {
        ClientHostProcessHeartbeat(peer);
        return 0;
      }
      break;
      case MessageDataType::kPlayerDelete:
      {
        ClientProcessRemovePlayer(data);
        return 1;
      }
      break;
      case MessageDataType::kValueSync:
      {
        return system_->value_syncer_->ClientHostProcessValueSync(data, peer);
      }
      break;
      case MessageDataType::kValueSyncValidation:
      {
        system_->value_syncer_->ClientProcessValueValidation(data);
        return 3;
      }
      break;
      default:
        break;
      }

      return 0;
    }

    //-------------------------------------------------------------------------
    ENetPacket* PacketHandler::CreatePacket(const Packet& packet)
    {
      enet_uint32 flags = 0;
      if (packet.reliable == true)
      {
        flags |= ENET_PACKET_FLAG_RELIABLE;
      }
      return enet_packet_create(packet.data, packet.size, flags);
    }
  }
}