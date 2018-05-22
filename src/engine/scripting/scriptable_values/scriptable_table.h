#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableTable : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable table
    * @author Rodi Lankester
    */
    class ScriptableTable : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;
    protected:
      /**
      * @brief Create a ScriptableObject
      * @param[in] lua_state() (lua_State*) the Lua state to create the ScriptableObject in
      */
      ScriptableTable(ScriptSystem* script_system);
    };
  }
}