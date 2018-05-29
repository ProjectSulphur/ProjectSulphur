#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableUserData : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to scriptable userdata
    * @author Rodi Lankester
    */
    class ScriptableUserData : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;
    protected:
      /**
      * @brief Create an instance of ScriptableUserdata
      * @param[in] lua_state() (lua_State*)
      *            The Lua state to create an instance of ScriptableUserData in
      * @param[in] value (void*) pointer to the UserData to expose to Lua
      */
      ScriptableUserData(ScriptState* script_state, void* value);
    };

  }
}