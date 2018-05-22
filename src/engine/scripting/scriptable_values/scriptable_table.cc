#include "engine/scripting/scriptable_values/scriptable_table.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableTable::ScriptableTable(ScriptSystem* script_system)
  : ScriptableValue(script_system)
{
  lua_newtable(script_system->lua_state());
  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
}