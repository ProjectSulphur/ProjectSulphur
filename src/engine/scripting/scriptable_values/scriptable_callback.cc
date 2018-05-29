#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/script_utils.h"

//------------------------------------------------------------------------------
sulphur::engine::ScriptableCallback::ScriptableCallback() :
  script_state_(nullptr),
  key_(""),
  ref_(-1),
  global_(false),
  uninitialized_log_(false),
  ptr_(nullptr)
{

}

//------------------------------------------------------------------------------
bool sulphur::engine::ScriptableCallback::FromGlobal(ScriptState* script_state, const char* name)
{

  lua_getglobal(script_state->lua_state(), name);

  if (static_cast<ScriptableValueType>(lua_type(script_state->lua_state(), -1))
    == ScriptableValueType::kFunction)
  {
    key_ = name;
    ptr_ = reinterpret_cast<void*>(lua_tocfunction(script_state->lua_state(), -1));
    lua_pop(script_state->lua_state(), -1);
    global_ = true;
    script_state_ = script_state;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool sulphur::engine::ScriptableCallback::FromValue(foundation::SharedPointer<ScriptableValue> value)
{
  value->Push();
   
  if (static_cast<ScriptableValueType>(lua_type(value->script_state()->lua_state(), -1))
    == ScriptableValueType::kFunction)
  {
    script_state_ = value->script_state();
    ptr_ = reinterpret_cast<void*>(lua_tocfunction(script_state_->lua_state(), -1));
    ref_ = luaL_ref(script_state_->lua_state(), LUA_REGISTRYINDEX);
    global_ = false;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool sulphur::engine::ScriptableCallback::operator==(const ScriptableCallback & other)
{
  return ptr_ == other.ptr_;
}

//------------------------------------------------------------------------------
sulphur::foundation::Vector<sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>>
sulphur::engine::ScriptableCallback::Call
(foundation::Vector<foundation::SharedPointer<ScriptableValue>> args)
{


  foundation::Vector<foundation::SharedPointer<ScriptableValue>> result
    = foundation::Vector<foundation::SharedPointer<ScriptableValue>>();
  if (script_state_ == nullptr)
  {
    if (uninitialized_log_ == false)
    {
      PS_LOG(Error, "Trying to call uninitialized callback");
      uninitialized_log_ = true;
    }
    return result;
  }

  if (global_)
    lua_getglobal(script_state_->lua_state(), key_);
  else
    lua_getref(script_state_->lua_state(), ref_);

  for (size_t i = 0; i < args.size(); ++i)
  {
    args[i]->Push();
  }

  int status = lua_pcall(
    script_state_->lua_state(),
    static_cast<int>(args.size()),
    LUA_MULTRET,
    0
  );

  if (status) {
    ScriptUtils::ScriptLog(
      foundation::Verbosity::kError,
      foundation::String(lua_tostring(script_state_->lua_state(), -1))
    );

    return result;
  }

  int res_count = lua_gettop(script_state_->lua_state());

  for (int i = 1; i <= res_count; i++)
  {
    foundation::SharedPointer<ScriptableValue> res
      = foundation::Memory::ConstructShared<ScriptableValue>(script_state_, i);
    result.push_back(res);
  }

  return result;
}
