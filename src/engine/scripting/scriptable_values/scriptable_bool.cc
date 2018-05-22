#include "engine/scripting/scriptable_values/scriptable_bool.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableBool::ScriptableBool(ScriptSystem* script_system, bool value)
  : ScriptableValue(script_system)
{
  lua_pushboolean(script_system->lua_state(), value);
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
}
