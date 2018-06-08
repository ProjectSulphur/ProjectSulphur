#include "tools/networking/rpc_system.h"
#include "tools/networking/networking_logger.h"
#include "tools/networking/networking_system.h"
#include "tools/networking/packet_handler.h"
#include "tools/networking/rpc_data.h"
#include <EASTL/sort.h>

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    RPCSystem::RPCSystem(NetworkingSystem* networking_system,
      PacketHandler* packet_handler) :
      networking_system_(networking_system),
      packet_handler_(packet_handler)
    {
      if (networking_system_->is_client())
      {
        packet_handler_->DispatchMessages();
      }
      GrowStack();
    }

    //-------------------------------------------------------------------------
    RPCSystem::~RPCSystem()
    {

    }

    //-------------------------------------------------------------------------
    RPCHandle RPCSystem::RegisterRPC(eastl::function<void(RPCHandle, NetworkPlayerData,
      const foundation::Vector<NetworkValue>&)> function,
      const foundation::Vector<NetworkValueType>& arguments)
    {
      uint16_t id = RequestID();
      RPCHandle handle;
      handle.id = id;
      handle.arguments = arguments;
      RPCData data;
      data.function = function;
      data.arguments = arguments;
      rpc_stack_[id] = data;
      return handle;
    }

    //-------------------------------------------------------------------------
    void RPCSystem::UnregisterRPC(const RPCHandle handle)
    {
      if (ValidateID(handle.id))
      {
        id_stack_.push_back(handle.id);
        rpc_stack_[handle.id] = { };
        SortIDs();
      }
    }

    //-------------------------------------------------------------------------
    void RPCSystem::InvokeRPC(const RPCHandle handle, const RPCMode mode,
      const foundation::Vector<NetworkValue>& arguments)
    {
      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        PS_LOG_WITH(NetworkLogger, Warning, 
          "Failed to invoke rpc, no host or client has been initialized");
        return;
      }
      if (ValidateID(handle.id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Invalid handle when calling invoke RPC");
        return;
      }
      if (ValidateArgumentsAgainstHandle(handle, arguments) == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning, 
          "Arguments don't match the RPC handle");
        return;
      }

      const RPCData& rpc = rpc_stack_[handle.id];
      const uint8_t& player_id = networking_system_->network_player()->id();

      NetworkPlayerData pl_data = {};
      NetworkPlayer* pl = networking_system_->network_player();
      memset(pl_data.name, '\0', kNameLength);
      memcpy(pl_data.name, pl->name(), strlen(pl->name()));
      pl_data.id = pl->id();
      pl_data.ping = pl->round_trip_time();

      if (networking_system_->is_host())
      {
        Message m;
        switch (mode)
        {
        case RPCMode::kHost:
          //call locally
          rpc.function(handle, pl_data, arguments);
          break;
        case RPCMode::kAll:
        case RPCMode::kBufferedAll:
          //call locally
          rpc.function(handle, pl_data, arguments);
        case RPCMode::kOthers:
        case RPCMode::kBufferedOthers:
          //send to others
          m = packet_handler_->ClientHostSendRPC(handle.id, player_id, mode, arguments,
            networking_system_->network_player()->peer());
          if (mode == RPCMode::kBufferedAll || mode == RPCMode::kBufferedOthers)
          {
            rpc_buffer_.push_back(m);
          }
          break;
        default:
          break;
        }
      }
      else if (networking_system_->is_client())
      {
        switch (mode)
        {
        case RPCMode::kAll:
          //call locally
          rpc.function(handle, pl_data, arguments);
        case RPCMode::kOthers:
        case RPCMode::kHost:
          //call to host and spread except me
          packet_handler_->ClientHostSendRPC(handle.id, player_id, mode, arguments,
            networking_system_->network_player()->peer());
          break;
        case RPCMode::kBufferedOthers:
        case RPCMode::kBufferedAll:
          PS_LOG_WITH(NetworkLogger, Warning, "Buffered RPCs are not supported on clients");
          break;
        default:
          break;
        }
      }
    }

    //-------------------------------------------------------------------------
    size_t RPCSystem::ClientHostProcessRPC(uint8_t* data, ENetPeer* from)
    {
      uint16_t id = 0;
      uint8_t mode_uint = 0;
      size_t size = 4;
      uint8_t player_id = 0;
      memcpy(&id, &data[0], 2);
      memcpy(&player_id, &data[2], 1);
      memcpy(&mode_uint, &data[3], 1);
      RPCMode mode = static_cast<RPCMode>(mode_uint);
      if (ValidateID(id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Error, "RPC with ID: %u is not registered", 
          static_cast<unsigned int>(id));
        return 3;
      }
      RPCData rpc = rpc_stack_[id];
      foundation::Vector<NetworkValue> values;
      for (unsigned int i = 0; i < rpc.arguments.size(); ++i)
      {
        NetworkValue value = { };
        value.type = rpc.arguments[i];
        switch (value.type)
        {
        case NetworkValueType::kFloat:
          memcpy(&value.f, &data[size], value.GetSize());
          break;
        case NetworkValueType::kDouble:
          memcpy(&value.d, &data[size], value.GetSize());
          break;
        case NetworkValueType::kInt:
          memcpy(&value.i, &data[size], value.GetSize());
          break;
        case NetworkValueType::kUnsignedInt:
          memcpy(&value.ui, &data[size], value.GetSize());
          break;
        case NetworkValueType::kBool:
          memcpy(&value.b, &data[size], value.GetSize());
          break;
        case NetworkValueType::kVec2:
          memcpy(&value.v2, &data[size], value.GetSize());
          break;
        case NetworkValueType::kVec3:
          memcpy(&value.v3, &data[size], value.GetSize());
          break;
        case NetworkValueType::kVec4:
          memcpy(&value.v4, &data[size], value.GetSize());
          break;
        case NetworkValueType::kInt8:
          memcpy(&value.i8, &data[size], value.GetSize());
          break;
        case NetworkValueType::kInt16:
          memcpy(&value.i16, &data[size], value.GetSize());
          break;
        case NetworkValueType::kUnsignedInt8:
          memcpy(&value.ui8, &data[size], value.GetSize());
          break;
        case NetworkValueType::kUnsignedInt16:
          memcpy(&value.ui16, &data[size], value.GetSize());
          break;
        case NetworkValueType::kString:
        {
          uint8_t str_len = data[size];
          memcpy(&value.str, &data[size + 1], str_len);
          size++; //because we read the value + 1 byte of length
        }
          break;
        }
        size += value.GetSize();
        values.push_back(value);
      }


      RPCHandle handle;
      handle.id = id;
      handle.arguments = rpc.arguments;

      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        return size;
      }
      if (ValidateArgumentsAgainstHandle(handle, values) == false)
      {
        PS_LOG_WITH(NetworkLogger, Error, "RPC call is invalid!")
        return size;
      }

      NetworkPlayerData pl_data = { };
      NetworkPlayer* pl = networking_system_->FindNetworkPlayerByID(player_id);
      memset(pl_data.name, '\0', kNameLength);
      memcpy(pl_data.name, pl->name(), strlen(pl->name()));
      pl_data.id = pl->id();
      pl_data.ping = pl->round_trip_time();

      rpc.function(handle, pl_data, values);

      if (networking_system_->is_host())
      {
        if (mode == RPCMode::kAll || mode == RPCMode::kOthers)
        {
          packet_handler_->ClientHostSendRPC(id, player_id, mode, values, from);
        }
      }

      return size;
    }

    //-------------------------------------------------------------------------
    size_t RPCSystem::ClientProcessRPCValidation(uint8_t* data)
    {
      uint16_t id = 0;
      memcpy(&id, &data[0], 2);
      uint8_t num_arg = 0;
      memcpy(&num_arg, &data[2], 1);

      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        return 3 + num_arg;
      }
      if (ValidateID(id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "RPC with id: %u is invalid!", static_cast<unsigned int>(id));
        return 3 + num_arg;
      }

      const RPCData& rpc = rpc_stack_[id];

      if (rpc.arguments.size() != num_arg)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "RPC with id: %u is invalid! Argument count is not the same as expected!", 
          static_cast<unsigned int>(id));
        return 3 + num_arg;
      }

      for (unsigned int i = 0; i < num_arg; ++i)
      {
        uint8_t type_raw = 0;
        memcpy(&type_raw, &data[3 + i], 1);
        NetworkValueType type = static_cast<NetworkValueType>(type_raw);
        if (rpc.arguments[i] != type)
        {
          PS_LOG_WITH(NetworkLogger, Error,
            "RPC with id: %u is invalid! Type is not the same as expected!",
            static_cast<unsigned int>(id));
          return 3 + num_arg;
        }
      }

      return 3 + num_arg;
    }

    //-------------------------------------------------------------------------
    void RPCSystem::ValidateRPC(uint16_t id)
    {
      if (networking_system_->is_host() == false)
      {
        return;
      }

      if (ValidateID(id) == false)
      {
        return;
      }

      packet_handler_->HostValidateRPC(id, rpc_stack_[id].arguments);
    }

    //-------------------------------------------------------------------------
    void RPCSystem::ValidateAllRPCs()
    {
      if (networking_system_->is_host() == false)
      {
        return;
      }

      for (unsigned int i = 0; i < rpc_stack_.size(); ++i)
      {
        ValidateRPC(static_cast<uint16_t>(i));
      }
    }

    //-------------------------------------------------------------------------
    void RPCSystem::CleanUp()
    {
      id_stack_.clear();
      for (unsigned int i = 0; i < rpc_stack_.size(); ++i)
      {
        id_stack_.push_back(static_cast<uint16_t>(i));
      }
    }

    //-------------------------------------------------------------------------
    void RPCSystem::SortIDs()
    {
      eastl::sort(id_stack_.begin(), id_stack_.end());
    }

    //-------------------------------------------------------------------------
    void RPCSystem::ClearRPCBuffer()
    {
      if (networking_system_->is_client() == false)
      {
        rpc_buffer_.clear();
      }
      else
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Please note that clearing the rpc buffer on a client doesn't do anything.");
      }
    }

    //-------------------------------------------------------------------------
    void RPCSystem::SendRPCBuffer(ENetPeer* peer)
    {
      if (networking_system_->is_host())
      {
        for (int i = 0; i < rpc_buffer_.size(); ++i)
        {
          Message m = rpc_buffer_[i];
          m.peer = peer;
          networking_system_->packet_handler_->ClientHostSendRPC(m);
        }
      }
    }

    //-------------------------------------------------------------------------
    void RPCSystem::GrowStack()
    {
      unsigned int current_size =
        static_cast<unsigned int>(rpc_stack_.size());
      unsigned int new_size = current_size + kRPCStackSize;

      if (new_size > UINT16_MAX)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Requesting too many RPC slots, you are most likely doing something wrong!");
        return;
      }

      rpc_stack_.resize(new_size);

      for (unsigned int i = current_size; i < new_size; ++i)
      {
        id_stack_.push_back(static_cast<uint16_t>(i));
      }
    }

    //-------------------------------------------------------------------------
    uint16_t RPCSystem::RequestID()
    {
      if (id_stack_.size() == 0)
      {
        GrowStack();
      }

      uint16_t id = id_stack_.front();
      id_stack_.pop_front();
      return id;
    }

    //-------------------------------------------------------------------------
    bool RPCSystem::ValidateID(uint16_t id)
    {
      if (id >= rpc_stack_.size())
      {
        return false;
      }

      return eastl::find(id_stack_.begin(), id_stack_.end(), id)
        == id_stack_.end();
    }

    //-------------------------------------------------------------------------
    bool RPCSystem::ValidateArgumentsAgainstHandle(const RPCHandle handle,
      const foundation::Vector<NetworkValue>& arguments)
    {
      if (handle.arguments.size() != arguments.size())
      {
        return false;
      }

      for (unsigned int i = 0; i < arguments.size(); ++i)
      {
        if (handle.arguments[i] != arguments[i].type)
        {
          return false;
        }
      }

      return true;
    }
  }
}