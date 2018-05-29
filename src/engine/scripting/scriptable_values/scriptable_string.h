#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableString : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable string
    * @author Rodi Lankester
    */
    class ScriptableString : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;
    protected:
      /**
      * @brief Create a ScriptableString
      * @param[in] lua_state() (lua_State*) the Lua state to create the ScriptableString in
      * @param[in] value (const char*) the initial value of the ScriptableString
      */
      ScriptableString(ScriptState* script_state, const char * value);
    };
  }
}