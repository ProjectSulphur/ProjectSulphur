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
      friend class foundation::Memory;
    protected:
      /**
      * @brief Create a ScriptableString
      * @param[in] script_system (ScriptSystem*) the script system to create the ScriptableString in
      * @param[in] value (const char*) the initial value of the ScriptableString
      */
      ScriptableString(ScriptSystem* script_system, const char* value);
    };
  }
}