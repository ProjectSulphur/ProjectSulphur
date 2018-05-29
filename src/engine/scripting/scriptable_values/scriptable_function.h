#pragma once

#include "engine/scripting/scriptable_value.h"
#include <foundation/memory/memory.h>

struct lua_State;

namespace sulphur
{
  namespace engine 
  {
    class ScriptableArgs;

    /**
    * @brief Function template for functions that will be exposed to Lua
    */
    using ScriptFunction = eastl::function<void(ScriptableArgs*)>;

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
      * @param[in] script_state (ScriptState*) the script state to create the function in
      * @param[in] function (ScriptFunction) the function to expose to Lua
      */
      ScriptableFunction(ScriptState* script_state, ScriptFunction function, bool is_lib = false);
      /**
      * @brief The function that Lua actually calls
      * @remark This should only ever be called from Lua
      * @param[in] lua_state() (lua_State*) the Lua state that is calling the function
      */
      static int Call(lua_State* lua_state);

      ScriptFunction script_function_; //!< the function that is exposed to Lua
      ScriptState* script_state_; //!< The script state this function was created in
    };
  }
}