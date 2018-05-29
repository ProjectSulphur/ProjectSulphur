#include "engine/scripting/scriptable_values/scriptable_userdata.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableUserData::ScriptableUserData(ScriptState* script_state, void* value)
  : ScriptableValue(script_state)
{
  lua_pushlightuserdata(script_state->lua_state(), value);
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
}