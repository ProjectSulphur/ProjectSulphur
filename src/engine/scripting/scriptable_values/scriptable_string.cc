#include "engine/scripting/scriptable_values/scriptable_string.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableString::ScriptableString(ScriptSystem* script_system, const char* value)
  : ScriptableValue(script_system)
{
  lua_pushstring(script_system->lua_state(), value);
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
}