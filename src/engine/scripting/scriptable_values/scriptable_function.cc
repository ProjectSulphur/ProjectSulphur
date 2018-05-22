#include "engine/scripting/scriptable_values/scriptable_function.h"

#include "engine/scripting/scriptable_values/scriptable_args.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableFunction::ScriptableFunction(
  ScriptSystem* script_system,
  ScriptFunction function,
  bool is_lib)
  : ScriptableValue(script_system)
{
  script_function_ = function;

  lua_pushlightuserdata(script_system->lua_state(), this);
  lua_pushcclosure(script_system->lua_state(), ScriptableFunction::Call, 1);

  ref_ = luaL_ref(script_system->lua_state(), LUA_REGISTRYINDEX);
  script_system_ = script_system;
  is_lib_ = is_lib;
}

//------------------------------------------------------------------------------
int sulphur::engine::ScriptableFunction::Call(lua_State* lua_state)
{
  ScriptableFunction* scriptable_function = reinterpret_cast<ScriptableFunction*>(lua_touserdata(lua_state, lua_upvalueindex(1)));
  int argCount = lua_gettop(lua_state);

  foundation::Vector<foundation::SharedPointer<ScriptableValue>> arglist =
    foundation::Vector<foundation::SharedPointer<ScriptableValue>>();

  lua_State* L = lua_state;
  lua_getmetatable(L, 1);

  for (int i = 1; i <= argCount; i++)
  {
    foundation::SharedPointer<ScriptableValue> arg
      = foundation::Memory::ConstructShared<ScriptableValue>(scriptable_function->script_system_, i);
    arglist.push_back(arg);
  }

  lua_pop(lua_state, -1);

  ScriptableArgs args = ScriptableArgs(arglist, scriptable_function->script_system_);

  (scriptable_function->script_function_)(&args);

  args.Return();

  return args.ReturnCount();
}