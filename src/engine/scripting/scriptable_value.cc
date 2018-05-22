#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableValue::ScriptableValue(ScriptSystem* script_system, int idx, bool is_lib)
{
  script_system_ = script_system;
  lua_pushvalue(script_system->lua_state() , idx);
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
  global_ = false;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::ScriptableValue(ScriptSystem* script_system, bool is_lib)
{
  script_system_ = script_system;
  ref_ = LUA_REFNIL;
  global_ = false;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::~ScriptableValue()
{
  if (!global_ && !is_lib_)
    lua_unref(script_system_->lua_state(), ref_);
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::ScriptableValue(ScriptSystem* script_system, const char* key, bool is_lib)
{
  global_ = true;
  key_ = key;
  script_system_ = script_system;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
void sulphur::engine::ScriptableValue::Push()
{
  if (global_ == false)
    lua_getref(script_system_->lua_state(), ref_);
  else
    lua_getglobal(script_system_->lua_state(), key_);
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValueType sulphur::engine::ScriptableValue::GetType()
{
  Push();
  ScriptableValueType result = static_cast<ScriptableValueType>(lua_type(script_system_->lua_state(), -1));
  lua_pop(script_system_->lua_state(), -1);
  return result;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptSystem* sulphur::engine::ScriptableValue::script_system()
{
  return script_system_;
}
