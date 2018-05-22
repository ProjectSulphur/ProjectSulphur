#include "engine/scripting/scriptable_values/scriptable_number.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableNumber::ScriptableNumber(ScriptSystem* script_system, lua_Number value)
  : ScriptableValue(script_system)
{
  lua_pushnumber(script_system->lua_state(), value);
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
}
