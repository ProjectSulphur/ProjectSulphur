#include "tools/networking/value_syncer.h"
#include "tools/networking/networking_logger.h"
#include "tools/networking/networking_system.h"
#include "tools/networking/packet_handler.h"

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
    uint16_t ValueSyncer::CreateValue(NetworkValueType type, void* location,
      NetworkValueOwner owner)
    {
      uint16_t id = RequestID();
      SyncableNetworkValue nv;
      nv.location = location;
      nv.value = NetworkValueFromLocation(location, type);
      value_stack_[id] = nv;
      if ((owner == NetworkValueOwner::kHost && networking_system_->is_host())
        || (owner == NetworkValueOwner::kMe))
      {
        our_values_.push_back(id);
      }
      return id;
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::RemoveValue(uint16_t id)
    {
      id_stack_.push_back(id);

      foundation::Vector<uint16_t>::iterator iter_ = 
        eastl::find(our_values_.begin(), our_values_.end(), id);
      if (iter_ == our_values_.end())
      {
        PS_LOG_WITH(NetworkLogger, Warning, 
          "Tried removing invalid value: %u", static_cast<unsigned int>(id));
        return;
      }
      if (iter_ != our_values_.end() - 1)
      {
        eastl::iter_swap(iter_, our_values_.end() - 1);
      }
      our_values_.pop_back();
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::Sync()
    {
      foundation::Vector<uint16_t>::iterator iter_;
      for (iter_ = our_values_.begin(); iter_ != our_values_.end(); ++iter_)
      {
        const uint16_t& id = *iter_;
        if (NeedsUpdate(value_stack_[id]))
        {
          value_stack_[id].value = NetworkValueFromLocation(
            value_stack_[id].location, value_stack_[id].value.type);
          packet_handler_->HostClientSyncValue(id, value_stack_[id].value);
        }
      }
    }

    //-------------------------------------------------------------------------
    size_t ValueSyncer::ClientHostProcessValueSync(uint8_t* data, 
      ENetPeer* from)
    {
      uint16_t id;
      memcpy(&id, data, 2);

      NetworkValueType type = value_stack_[id].value.type;
      size_t size = 0;

      switch (type)
      {
      case NetworkValueType::kFloat:
        size = sizeof(float);
        break;
      case NetworkValueType::kDouble:
        size = sizeof(double);
        break;
      case NetworkValueType::kInt:
        size = sizeof(int);
        break;
      case NetworkValueType::kUnsignedInt:
        size = sizeof(unsigned int);
        break;
      case NetworkValueType::kBool:
        size = sizeof(bool);
        break;
      case NetworkValueType::kVec2:
        size = sizeof(glm::vec2);
        break;
      case NetworkValueType::kVec3:
        size = sizeof(glm::vec3);
        break;
      case NetworkValueType::kVec4:
        size = sizeof(glm::vec4);
        break;
      default:
        break;
      }

      if (ValidateID(id) == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning,
          "Tried to sync a value with an invalid id: %u", id);
        return 2 + size;
      }

      memcpy(value_stack_[id].location, &data[2], size);

      if (networking_system_->is_host())
      {
        //if we recieve a message as a host, it means that we need to send it to the other clients
        packet_handler_->HostClientSyncValue(id, 
          NetworkValueFromLocation(value_stack_[id].location, 
            value_stack_[id].value.type), from);
      }
      return 2 + size;
    }

    //-------------------------------------------------------------------------
    void ValueSyncer::ClientProcessValueValidation(uint8_t* data)
    {
      uint16_t id = 0;
      memcpy(&id, &data[0], 2);
      uint8_t type = 0;
      memcpy(&type, &data[2], 1);

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
    void ValueSyncer::ValidateValue(uint16_t id)
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
        ValidateValue(static_cast<uint16_t>(i));
      }
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
        id_stack_.push_back(static_cast<uint16_t>(i));
      }
    }

    //-------------------------------------------------------------------------
    uint16_t ValueSyncer::RequestID()
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
    bool ValueSyncer::ValidateID(uint16_t id)
    {
      if (id >= value_stack_.size())
      {
        return false;
      }
      
      return eastl::find(id_stack_.begin(), id_stack_.end(), id)
        == id_stack_.end();
    }

    //-------------------------------------------------------------------------
    bool ValueSyncer::NeedsUpdate(const SyncableNetworkValue& nv)
    {
      NetworkValue other =
        NetworkValueFromLocation(nv.location, nv.value.type);
      return other != nv.value;
    }

    //-------------------------------------------------------------------------
    NetworkValue ValueSyncer::NetworkValueFromLocation(void* location,
      const NetworkValueType type)
    {
      NetworkValue nv;
      nv.type = type;
      switch (type)
      {
      case NetworkValueType::kFloat:
        nv.f = *static_cast<float*>(location);
        break;
      case NetworkValueType::kDouble:
        nv.d = *static_cast<double*>(location);
        break;
      case NetworkValueType::kInt:
        nv.i = *static_cast<int*>(location);
        break;
      case NetworkValueType::kUnsignedInt:
        nv.ui = *static_cast<unsigned int*>(location);
        break;
      case NetworkValueType::kBool:
        nv.b = *static_cast<bool*>(location);
        break;
      case NetworkValueType::kVec2:
        nv.v2 = *static_cast<glm::vec2*>(location);
        break;
      case NetworkValueType::kVec3:
        nv.v3 = *static_cast<glm::vec3*>(location);
        break;
      case NetworkValueType::kVec4:
        nv.v4 = *static_cast<glm::vec4*>(location);
        break;
      }
      return nv;
    }
  }
}