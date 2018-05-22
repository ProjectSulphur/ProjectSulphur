#include "engine/scripting/scriptable_values/scriptable_userdata.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableUserData::ScriptableUserData(ScriptSystem* script_system, void* value)
  : ScriptableValue(script_system)
{
  lua_pushlightuserdata(script_system->lua_state(), value);
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
}