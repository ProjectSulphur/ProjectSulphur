#include "engine/systems/components/network_component_system.h"
#include <tools/networking/networking_logger.h>
#include "engine/networking/network_system.h"

#include "engine/scripting/script_system.h"

#include <lua-classes/network_component_system.lua.cc>
#include <tools/networking/export.h>

namespace sulphur
{
  namespace engine
  {
    //---------------------------------------------------------------------------------------------
    NetworkComponentSystem::NetworkComponentSystem() :
      IComponentSystem("NetworkComponentSystem"),
      last_network_id_(0)
    {

    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::OnInitialize(Application& app, foundation::JobGraph& /*job_graph*/)
    {
      last_network_id_ = 0;
      script_state_ = app.GetService<ScriptSystem>().script_state();
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::OnTerminate()
    {
      component_data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------
    NetworkComponent NetworkComponentSystem::Create(Entity& entity)
    {
      ++last_network_id_;
      return NetworkComponent(*this, component_data_.data.Add(entity, last_network_id_,
        foundation::Map<foundation::String, networking::SyncValueID>(),
        foundation::Map<foundation::String, networking::RPCHandle>(),
        foundation::Map<uint16_t, ScriptableCallback>()));
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::Destroy(ComponentHandleBase handle)
    {
      if (component_data_.data.IsValid(handle))
      {
        foundation::Map<foundation::String, networking::SyncValueID>* sync_handles = &component_data_.data.
          Get<static_cast<size_t>(NetworkComponentElements::kValueSyncIds)>(handle);
        foundation::Map<foundation::String, networking::SyncValueID>::iterator sync_iter;
        for (sync_iter = sync_handles->begin(); sync_iter != sync_handles->end(); ++sync_iter)
        {
          networking::SNetRemoveSyncValue(sync_iter->second);
        }

        foundation::Map<foundation::String, networking::RPCHandle>* handles =
          &component_data_.data.Get<static_cast<size_t>(NetworkComponentElements::kRPCHandles)>(handle);
        foundation::Map<foundation::String, networking::RPCHandle>::iterator iter;
        for (iter = handles->begin(); iter != handles->end(); ++iter)
        {
          networking::SNetUnregisterRPC(iter->second);
        }

        component_data_.data.Remove(handle);
      }
    }

    //---------------------------------------------------------------------------------------------
    uint16_t NetworkComponentSystem::GetNetworkID(NetworkComponent handle)
    {
      return component_data_.data.Get<static_cast<size_t>(NetworkComponentElements::kID)>(handle);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::AddSyncValue(NetworkComponent handle, const foundation::String& name,
      networking::SyncValueID sync_id)
    {
      component_data_.data.
        Get<static_cast<size_t>(NetworkComponentElements::kValueSyncIds)>(handle).
        insert(eastl::pair<foundation::String, networking::SyncValueID>(name, sync_id));
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::SetSyncValue(NetworkComponent handle, const foundation::String& name, const networking::NetworkValue& value)
    {
      foundation::Map<foundation::String, networking::SyncValueID>* ids =
        &component_data_.data.Get<static_cast<size_t>(NetworkComponentElements::kValueSyncIds)>(handle);

      foundation::Map<foundation::String, networking::SyncValueID>::iterator iter = ids->find(name);
      if (iter == ids->end())
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Failed to set sync value '%s'", name.c_str());
        return;
      }

      networking::SyncValueID id = iter->second;
      networking::SNetSetSyncValue(id, value);
    }

    //---------------------------------------------------------------------------------------------
    networking::NetworkValue NetworkComponentSystem::GetSyncValue(NetworkComponent handle, const foundation::String& name)
    {
      networking::NetworkValue value = { };

      foundation::Map<foundation::String, networking::SyncValueID>* ids =
        &component_data_.data.Get<static_cast<size_t>(NetworkComponentElements::kValueSyncIds)>(handle);

      foundation::Map<foundation::String, networking::SyncValueID>::iterator iter = ids->find(name);
      if (iter == ids->end())
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Failed to get sync value '%s'", name.c_str());
        return value;
      }

      if (networking::SNetGetSyncValue(iter->second, &value) == false)
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Failed to get sync value internally. name: %s", name.c_str());
      }

      return value;
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::RegisterRPC(NetworkComponent handle, const foundation::String& name,
      networking::RPCHandle rpc_handle)
    {
      component_data_.data.
        Get<static_cast<size_t>(NetworkComponentElements::kRPCHandles)>(handle).insert({ name, rpc_handle });
    }

    //---------------------------------------------------------------------------------------------
    bool NetworkComponentSystem::GetRPCHandle(NetworkComponent handle, const foundation::String& name, networking::RPCHandle& h)
    {
      foundation::Map<foundation::String, networking::RPCHandle>* handles =
        &component_data_.data.Get<static_cast<size_t>(NetworkComponentElements::kRPCHandles)>(handle);

      foundation::Map<foundation::String, networking::RPCHandle>::iterator iter = handles->find(name);
      if (iter == handles->end())
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Failed to invoke RPC with name: %s", name.c_str());
        return false;
      }


      h = iter->second;
      return true;
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponentSystem::AddScriptRPC(NetworkComponent handle, uint16_t id, ScriptableCallback function)
    {
      component_data_.data.
        Get<static_cast<size_t>(NetworkComponentElements::kRPCScriptHandles)>(handle).insert({ id, function });
    }

    //---------------------------------------------------------------------------------------------
    ScriptableCallback* NetworkComponentSystem::GetScriptRPC(NetworkComponent handle, uint16_t id)
    {
      foundation::Map<uint16_t, ScriptableCallback>* handles = &component_data_.data.
        Get<static_cast<size_t>(NetworkComponentElements::kRPCScriptHandles)>(handle);

      foundation::Map<uint16_t, ScriptableCallback>::iterator iter = handles->find(id);
      if (iter == handles->end())
      {
        PS_LOG_WITH(NetworkLogger, Warning, "Failed to find script RPC, some bookkeeping is going horribly wrong!");
        return nullptr;
      }

      return &iter->second;
    }

    //---------------------------------------------------------------------------------------------
    ScriptHandle NetworkComponentSystem::ConvertNetworkValueToScriptHandle(const networking::NetworkValue& value)
    {
      switch (value.type)
      {
      case networking::NetworkValueType::kFloat:
        return ScriptUtils::Instantiate(script_state_, value.f);
      case networking::NetworkValueType::kDouble:
        return ScriptUtils::Instantiate(script_state_, value.d);
      case networking::NetworkValueType::kInt:
        return ScriptUtils::Instantiate(script_state_, value.i);
      case networking::NetworkValueType::kUnsignedInt:
        return ScriptUtils::Instantiate(script_state_, value.ui);
      case networking::NetworkValueType::kBool:
        return ScriptUtils::Instantiate(script_state_, value.b);
      case networking::NetworkValueType::kVec2:
        return ScriptUtils::Instantiate(script_state_, value.v2);
      case networking::NetworkValueType::kVec3:
        return ScriptUtils::Instantiate(script_state_, value.v3);
      case networking::NetworkValueType::kVec4:
        return ScriptUtils::Instantiate(script_state_, value.v4);
      case networking::NetworkValueType::kInt8:
        return ScriptUtils::Instantiate(script_state_, value.i8);
      case networking::NetworkValueType::kInt16:
        return ScriptUtils::Instantiate(script_state_, value.i16);
      case networking::NetworkValueType::kUnsignedInt8:
        return ScriptUtils::Instantiate(script_state_, value.ui8);
      case networking::NetworkValueType::kUnsignedInt16:
        return ScriptUtils::Instantiate(script_state_, value.ui16);
      case networking::NetworkValueType::kString:
        return ScriptUtils::Instantiate(script_state_, value.str);
      default:
        return 0;
      }
    }

    networking::NetworkValue NetworkComponentSystem::ConvertScriptHandleToNetworkValue(ScriptHandle handle, networking::NetworkValueType type)
    {
      networking::NetworkValue value;
      value.type = type;
      switch (type)
      {
      case networking::NetworkValueType::kFloat:
        value.f = ScriptUtils::As<float>(handle);
        break;
      case networking::NetworkValueType::kDouble:
        value.d = ScriptUtils::As<double>(handle);
        break;
      case networking::NetworkValueType::kInt:
        value.i = ScriptUtils::As<int>(handle);
        break;
      case networking::NetworkValueType::kUnsignedInt:
        value.ui = ScriptUtils::As<unsigned int>(handle);
        break;
      case networking::NetworkValueType::kBool:
        value.b = ScriptUtils::As<bool>(handle);
        break;
      case networking::NetworkValueType::kVec2:
        value.v2 = ScriptUtils::As<glm::vec2>(handle);
        break;
      case networking::NetworkValueType::kVec3:
        value.v3 = ScriptUtils::As<glm::vec3>(handle);
        break;
      case networking::NetworkValueType::kVec4:
        value.v4 = ScriptUtils::As<glm::vec4>(handle);
        break;
      case networking::NetworkValueType::kInt8:
        value.i8 = ScriptUtils::As<int8_t>(handle);
        break;
      case networking::NetworkValueType::kInt16:
        value.i16 = ScriptUtils::As<int16_t>(handle);
        break;
      case networking::NetworkValueType::kUnsignedInt8:
        value.ui8 = ScriptUtils::As<uint8_t>(handle);
        break;
      case networking::NetworkValueType::kUnsignedInt16:
        value.ui16 = ScriptUtils::As<uint16_t>(handle);
        break;
      case networking::NetworkValueType::kString:
        foundation::String s = ScriptUtils::As<foundation::String>(handle);
        memset(value.str, '\0', networking::kStringLength);
        memcpy_s(value.str, networking::kStringLength, s.c_str(), s.size());
        break;
      }

      return value;
    }

    //---------------------------------------------------------------------------------------------
    ScriptState* NetworkComponentSystem::GetScriptState()
    {
      return script_state_;
    }

    //---------------------------------------------------------------------------------------------
    NetworkComponent::NetworkComponent() : system_(nullptr)
    {

    }

    //---------------------------------------------------------------------------------------------
    NetworkComponent::NetworkComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }

    //---------------------------------------------------------------------------------------------
    uint16_t NetworkComponent::GetNetworkID()
    {
      return system_->GetNetworkID(*this);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::RegisterSyncValue(const foundation::String& name,
      const networking::NetworkValueType type, networking::NetworkValueOwner owner)
    {
      networking::SyncValueID id = networking::SNetCreateSyncValue(type, owner);
      system_->AddSyncValue(*this, name, id);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::SetSyncValueEngine(const foundation::String& name, const networking::NetworkValue value)
    {
      system_->SetSyncValue(*this, name, value);
    }

    //---------------------------------------------------------------------------------------------
    networking::NetworkValue NetworkComponent::GetSyncValueEngine(const foundation::String& name)
    {
      return system_->GetSyncValue(*this, name);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::RegisterSyncValue(const foundation::String& name, const NetworkComponent::NetworkValueType type, const NetworkComponent::NetworkValueOwner owner)
    {
      RegisterSyncValue(name, static_cast<networking::NetworkValueType>(type), static_cast<networking::NetworkValueOwner>(owner));
    }

    //---------------------------------------------------------------------------------------------
    ScriptHandle NetworkComponent::GetSyncValue(const foundation::String& name)
    {
      networking::NetworkValue value = system_->GetSyncValue(*this, name);
      return system_->ConvertNetworkValueToScriptHandle(value);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::SetSyncValue(const foundation::String& name, ScriptHandle value)
    {
      networking::NetworkValue old_value = system_->GetSyncValue(*this, name);
      networking::NetworkValue v = system_->ConvertScriptHandleToNetworkValue(value, old_value.type);
      return system_->SetSyncValue(*this, name, v);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::RegisterRPC(const foundation::String & name,
      eastl::function<void(networking::RPCHandle, networking::NetworkPlayerData,
        const foundation::Vector<networking::NetworkValue>&)> function,
      const foundation::Vector<networking::NetworkValueType>& arguments)
    {
      networking::RPCHandle h = networking::SNetRegisterRPC(function, arguments);
      system_->RegisterRPC(*this, name, h);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::InvokeRPC(const foundation::String & name, const networking::RPCMode mode,
      const foundation::Vector<networking::NetworkValue>& arguments)
    {
      networking::RPCHandle h;
      if (system_->GetRPCHandle(*this, name, h) == true)
      {
        networking::SNetInvokeRPC(h, mode, arguments);
      }
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::RegisterRPC(ScriptableArgs* args)
    {
      args->Check("UUSF", 4);
      ScriptHandle self = args->GetArg(1);
      foundation::String name = ScriptUtils::As<foundation::String>(args->GetArg(2));
      ScriptableCallback function = ScriptUtils::As<ScriptableCallback>(args->GetArg(3));
      foundation::Vector<networking::NetworkValueType> types;
      for (int i = 4; i < args->NumArgs(); i++)
      {
        ScriptHandle value = args->GetArg(i);
        if (value->GetType() != ScriptableValueType::kNumber)
        {
          ScriptUtils::ScriptLog(foundation::Verbosity::kError, "Arg %n expected number but got something else", i);
          return;
        }
        types.push_back(ScriptUtils::As<networking::NetworkValueType>(value));
      }

      eastl::function<void(networking::RPCHandle h, networking::NetworkPlayerData player, const foundation::Vector<networking::NetworkValue>& args)> test =
        eastl::function<void(networking::RPCHandle h, networking::NetworkPlayerData player, const foundation::Vector<networking::NetworkValue>& args)>(eastl::allocator_arg, foundation::EASTLAllocator(),
          [&, self](networking::RPCHandle h, networking::NetworkPlayerData player, const foundation::Vector<networking::NetworkValue>& args)
      {
        ScriptableCallback* func = system_->GetScriptRPC(*this, h.id);
        if (func != nullptr)
        {
          foundation::Vector<ScriptHandle> handles;
          handles.push_back(self);
          for (unsigned int i = 0; i < args.size(); ++i)
          {
            handles.push_back(system_->ConvertNetworkValueToScriptHandle(args[i]));
          }
          ScriptableNetworkPlayer pl;
          pl.data = player;

          handles.push_back(ScriptUtils::Instantiate<ScriptableNetworkPlayer>(system_->GetScriptState(), pl));

          func->Call(handles);
        }
      });

      networking::RPCHandle h = networking::SNetRegisterRPC(test, types);

      system_->RegisterRPC(*this, name, h);
      system_->AddScriptRPC(*this, h.id, function);
    }

    //---------------------------------------------------------------------------------------------
    void NetworkComponent::InvokeRPC(ScriptableArgs* args)
    {
      args->Check("USN", 3);
      foundation::String name = ScriptUtils::As<foundation::String>(args->GetArg(1));
      networking::RPCMode mode = ScriptUtils::As<networking::RPCMode>(args->GetArg(2));
      foundation::Vector<networking::NetworkValue> arguments;

      networking::RPCHandle h;

      if (system_->GetRPCHandle(*this, name, h) == true)
      {
        for (int i = 3; i < args->NumArgs(); i++)
        {
          networking::NetworkValue value = system_->ConvertScriptHandleToNetworkValue(args->GetArg(i), h.arguments[i - 3]);
          arguments.push_back(value);
        }

        networking::SNetInvokeRPC(h, mode, arguments);
      }
    }
  }
}
