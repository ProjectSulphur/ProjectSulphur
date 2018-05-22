#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableBool : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable bool
    * @author Rodi Lankester
    */
    class ScriptableBool : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;
    protected:
      /**
      * @brief Create a ScriptableBool
      * @param[in] lua_state() (lua_State*) the Lua state to create the ScriptableBool in
      * @param[in] value (bool) the initial value of the ScriptableBool
      */
      ScriptableBool(ScriptSystem* script_system, bool value);
    };

  }
}