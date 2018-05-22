#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"
namespace sulphur
{
  namespace engine 
  {
    class ScriptableArgs;

    using ScriptFunction = eastl::function<void(ScriptableArgs*)>; //<! function template for functions that will be exposed to Lua

    /**
    * @class sulphur::engine::ScriptableFunction : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a C function that is exposed to Lua
    * @author Rodi Lankester
    */
    class ScriptableFunction : public ScriptableValue
    {
      friend class foundation::Memory;
    protected:
      /**
      * @brief Constructs a scriptable function
      * @param[in] script_system (ScriptSystem*) the script state to create the function in
      * @param[in] function (ScriptFunction) the function to expose to Lua
      */
      ScriptableFunction(ScriptSystem* script_system, ScriptFunction function, bool is_lib = false);
      /**
      * @brief The function that Lua actually calls
      * @remark This should only ever be called from Lua
      * @param[in] lua_state() (lua_State*) the Lua state that is calling the function
      */
      static int Call(lua_State* lua_state);

      ScriptFunction script_function_; //!< the function that is exposed to Lua
      ScriptSystem* script_system_; //!< The script state this function was created in
    };
  }
}