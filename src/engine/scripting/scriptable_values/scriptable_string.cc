#include "engine/scripting/scriptable_values/scriptable_string.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableString::ScriptableString(ScriptState* script_state, const char* value)
  : ScriptableValue(script_state)
{
  lua_pushstring(script_state->lua_state(), value);
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
}