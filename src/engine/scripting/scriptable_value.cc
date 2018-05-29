#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableValue::ScriptableValue(ScriptState* script_state, int idx, bool is_lib)
{
  script_state_ = script_state;
  lua_pushvalue(script_state->lua_state() , idx);
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
  global_ = false;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::ScriptableValue(ScriptState* script_state, bool is_lib)
{
  script_state_ = script_state;
  ref_ = LUA_REFNIL;
  global_ = false;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::~ScriptableValue()
{
  if (!global_ && !is_lib_)
    lua_unref(script_state_->lua_state(), ref_);
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValue::ScriptableValue(ScriptState* script_state, const char* key, bool is_lib)
{
  global_ = true;
  key_ = key;
  script_state_ = script_state;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
void sulphur::engine::ScriptableValue::Push()
{
  if (global_ == false)
    lua_getref(script_state_->lua_state(), ref_);
  else
    lua_getglobal(script_state_->lua_state(), key_);
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptableValueType sulphur::engine::ScriptableValue::GetType()
{
  Push();
  ScriptableValueType result = static_cast<ScriptableValueType>(lua_type(script_state_->lua_state(), -1));
  lua_pop(script_state_->lua_state(), -1);
  return result;
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptState* sulphur::engine::ScriptableValue::script_state()
{
  return script_state_;
}
