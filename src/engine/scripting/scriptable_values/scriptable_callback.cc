#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/script_utils.h"

namespace sulphur
{
  namespace engine
  {
    bool ScriptableCallback::FromGlobal(ScriptSystem* script_system, const char* name)
    {
      lua_getglobal(script_system->lua_state(), name);

      if (static_cast<ScriptableValueType>(lua_type(script_system->lua_state(), -1))
        == ScriptableValueType::kFunction)
      {
        key_ = name;
        lua_pop(script_system->lua_state(), -1);
        global_ = true;
        script_system_ = script_system;
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------
    bool ScriptableCallback::FromValue(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();

      if (static_cast<ScriptableValueType>(lua_type(value->script_system()->lua_state(), -1))
        == ScriptableValueType::kFunction)
      {
        script_system_ = value->script_system();
        ref_ = luaL_ref(script_system_->lua_state(), LUA_REGISTRYINDEX);
        global_ = false;
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------
    foundation::Vector<foundation::SharedPointer<ScriptableValue>>
      ScriptableCallback::Call
      (foundation::Vector<foundation::SharedPointer<ScriptableValue>> args)
    {
      foundation::Vector<foundation::SharedPointer<ScriptableValue>> result
        = foundation::Vector<foundation::SharedPointer<ScriptableValue>>();
      if (script_system_ == nullptr)
      {
        PS_LOG(Error, "Trying to call uninitialized callback");
        return result;
      }

      if (global_ == true)
      {
        lua_getglobal(script_system_->lua_state(), key_);
      }
      else
      {
        lua_getref(script_system_->lua_state(), ref_);
      }

      for (size_t i = 0; i < args.size(); ++i)
      {
        args[i]->Push();
      }

      int status = lua_pcall(
        script_system_->lua_state(),
        static_cast<int>(args.size()),
        LUA_MULTRET,
        0
      );

      if (status != 0)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          foundation::String(lua_tostring(script_system_->lua_state(), -1))
        );

        return result;
      }

      int res_count = lua_gettop(script_system_->lua_state());

      for (int i = 1; i <= res_count; i++)
      {
        foundation::SharedPointer<ScriptableValue> res
          = foundation::Memory::ConstructShared<ScriptableValue>(script_system_, i);
        result.push_back(res);
      }

      return result;
    }
  }
}
