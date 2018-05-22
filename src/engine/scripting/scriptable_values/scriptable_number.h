#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableNumber : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable number
    * @author Rodi Lankester
    */
    class ScriptableNumber : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;
    protected:
      /**
      * @brief Create a ScriptableNumber
      * @param[in] lua_state() (lua_State*) the Lua state to create the ScriptableNumber in
      * @param[in] value (lua_Number) the initial value of the ScriptableNumber
      */
      ScriptableNumber(ScriptSystem* script_system, lua_Number value);
    };
  }
}