#include "engine/scripting/scriptable_values/scriptable_bool.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableBool::ScriptableBool(ScriptState* script_state, bool value)
  : ScriptableValue(script_state)
{
  lua_pushboolean(script_state->lua_state(), value);
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
}
