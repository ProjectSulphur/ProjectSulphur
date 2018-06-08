#include "tools/networking/value_syncer.h"
#include "tools/networking/networking_logger.h"
#include "tools/networking/networking_system.h"
#include "tools/networking/packet_handler.h"
#include <eastl/sort.h>

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    ValueSyncer::ValueSyncer(NetworkingSystem* networking_system,
      PacketHandler* packet_handler) :
      networking_system_(networking_system),
      packet_handler_(packet_handler)
    {
      GrowStack();
    }

    //-------------------------------------------------------------------------
    ValueSyncer::~ValueSyncer()
    {

    }

    //-------------------------------------------------------------------------
    SyncValueID ValueSyncer::CreateValue(NetworkValueType type,
      NetworkValueOwner owner)
    {
      SyncValueID id = RequestID();
      SyncableNetworkValue nv;
      nv.updated = false;
      nv.value = DefaultNetworkValue(type);
      value_stack_[id] = nv;
      all_values_.push_back({ id, owner });

      return id;
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::RemoveValue(SyncValueID id)
    {
      foundation::Vector<SyncValueIDWithOwner>::iterator iter_ =
        eastl::find(all_values_.begin(), all_values_.end(), id);
      if (iter_ == all_values_.end())
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Tried removing invalid value: %u", static_cast<unsigned int>(id));
        return;
      }
      if (iter_ != all_values_.end() - 1)
      {
        eastl::iter_swap(iter_, all_values_.end() - 1);
      }
      id_stack_.push_back(id);
      all_values_.pop_back();
      SortIDs();
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::Sync()
    {
      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        return;
      }

      foundation::Vector<SyncValueIDWithOwner>::iterator iter_;
      for (iter_ = all_values_.begin(); iter_ != all_values_.end(); ++iter_)
      {
        const SyncValueID& id = iter_->id;
        if (value_stack_[id].updated == true)
        {
          if ((iter_->owner == NetworkValueOwner::kHost && networking_system_->is_host())
            || (iter_->owner == NetworkValueOwner::kMe))
          {
            value_stack_[id].updated = false;
            packet_handler_->HostClientSyncValue(id, value_stack_[id].value);
          }
        }
      }
    }

    //-------------------------------------------------------------------------
    size_t ValueSyncer::ClientHostProcessValueSync(uint8_t* data,
      ENetPeer* from)
    {
      SyncValueID id;
      memcpy(&id, data, sizeof(SyncValueID));

      NetworkValueType type = value_stack_[id].value.type;
      NetworkValue temp_value = { };
      temp_value.type = type;
      size_t size = temp_value.GetSize();
      size_t size_string = 0;
      if (type == NetworkValueType::kString)
      {
        size_string = static_cast<size_t>(data[2]);
        size = size_string;
        size++;
      }

      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        return 2 + size;
      }

      if (ValidateID(id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Tried to sync a value with an invalid id: %u", id);
        return 2 + size;
      }

      if (type == NetworkValueType::kString)
      {
        memset(value_stack_[id].value.str, '\0', kStringLength);
        memcpy(value_stack_[id].value.str, &data[3], size_string);
      }
      else
      {
        memcpy(static_cast<void*>(&value_stack_[id].value.f), &data[2], size);
      }

      if (networking_system_->is_host())
      {
        //if we recieve a message as a host, it means that we need to send it to the other clients
        packet_handler_->HostClientSyncValue(id, value_stack_[id].value, from);
      }
      return 2 + size;
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::ClientProcessValueValidation(uint8_t* data)
    {
      SyncValueID id = 0;
      memcpy(&id, &data[0], 2);
      uint8_t type = 0;
      memcpy(&type, &data[2], 1);

      if (networking_system_->connection_setup() == ConnectionSetup::kNone)
      {
        return;
      }

      if (ValidateID(id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Value with id: %u is invalid!", static_cast<unsigned int>(id));
        return;
      }

      if (value_stack_[id].value.type != static_cast<NetworkValueType>(type))
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Value with id: %u is invalid!", static_cast<unsigned int>(id));
      }
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::ValidateValue(SyncValueID id)
    {
      if (networking_system_->is_host() == false)
      {
        return;
      }

      if (ValidateID(id) == false)
      {
        return;
      }

      packet_handler_->HostValidateValue(id, value_stack_[id].value.type);
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::ValidateAllValues()
    {
      if (networking_system_->is_host() == false)
      {
        return;
      }

      for (unsigned int i = 0; i < value_stack_.size(); ++i)
      {
        ValidateValue(static_cast<SyncValueID>(i));
      }
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::CleanUp()
    {
      id_stack_.clear();
      for (unsigned int i = 0; i < value_stack_.size(); ++i)
      {
        id_stack_.push_back(static_cast<SyncValueID>(i));
      }
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::SortIDs()
    {
      eastl::sort(id_stack_.begin(), id_stack_.end());
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::SetValue(SyncValueID id, const NetworkValue& value)
    {
      if (ValidateID(id) == true)
      {
        if (value_stack_[id].value != value)
        {
          value_stack_[id].updated = true;
          value_stack_[id].value = value;
        }
      }
    }

    //-------------------------------------------------------------------------
    bool ValueSyncer::GetValue(SyncValueID id, NetworkValue* value)
    {
      bool result = ValidateID(id);
      if (result == true)
      {
        *value = value_stack_[id].value;
      }
      return result;
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::GrowStack()
    {
      unsigned int current_size =
        static_cast<unsigned int>(value_stack_.size());
      unsigned int new_size = current_size + kValueStackSize;

      if (new_size > UINT16_MAX)
      {
        PS_LOG_WITH(NetworkLogger, Error,
          "Requesting too many value sync slots, you are most likely doing something wrong!");
        return;
      }

      value_stack_.resize(new_size);

      for (unsigned int i = current_size; i < new_size; ++i)
      {
        id_stack_.push_back(static_cast<SyncValueID>(i));
      }
    }

    //-------------------------------------------------------------------------
    SyncValueID ValueSyncer::RequestID()
    {
      if (id_stack_.size() == 0)
      {
        GrowStack();
      }

      SyncValueID id = id_stack_.front();
      id_stack_.pop_front();
      return id;
    }

    //-------------------------------------------------------------------------
    bool ValueSyncer::ValidateID(SyncValueID id)
    {
      if (id >= value_stack_.size())
      {
        return false;
      }

      return eastl::find(id_stack_.begin(), id_stack_.end(), id)
        == id_stack_.end();
    }

    //-------------------------------------------------------------------------
    NetworkValue ValueSyncer::DefaultNetworkValue(const NetworkValueType type)
    {
      NetworkValue nv = { };
      nv.type = type;
      switch (type)
      {
      case NetworkValueType::kFloat:
        nv.f = 0.0f;
        break;
      case NetworkValueType::kDouble:
        nv.d = 0.0;
        break;
      case NetworkValueType::kInt:
        nv.i = 0;
        break;
      case NetworkValueType::kUnsignedInt:
        nv.ui = 0u;
        break;
      case NetworkValueType::kBool:
        nv.b = false;
        break;
      case NetworkValueType::kVec2:
        nv.v2 = glm::vec2(0.0f);
        break;
      case NetworkValueType::kVec3:
        nv.v3 = glm::vec3(0.0f);
        break;
      case NetworkValueType::kVec4:
        nv.v4 = glm::vec4(0.0f);
        break;
      case NetworkValueType::kInt8:
        nv.i8 = 0;
        break;
      case NetworkValueType::kInt16:
        nv.i16 = 0;
        break;
      case NetworkValueType::kUnsignedInt8:
        nv.ui8 = 0u;
        break;
      case NetworkValueType::kUnsignedInt16:
        nv.ui16 = 0u;
        break;
      case NetworkValueType::kString:
        memset(nv.str, '\0', kStringLength);
        break;
      }
      return nv;
    }
  }
}