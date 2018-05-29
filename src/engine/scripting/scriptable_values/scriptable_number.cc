#include "engine/scripting/scriptable_values/scriptable_number.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableNumber::ScriptableNumber(ScriptState* script_state, lua_Number value)
  : ScriptableValue(script_state)
{
  lua_pushnumber(script_state->lua_state(), value);
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
}
