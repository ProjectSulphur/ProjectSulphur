#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableCallback
    * @brief Wrapper around Lua functions callable from C
    * @author Rodi Lankester
    */
    class ScriptableCallback
    {
    public:
      /**
      * @brief Get a Lua function from the global namespace
      * @param[in] script_system (sulphur::engine::ScriptSystem*) The state to get the function from
      * @param[in] name (const char*) The name of the function in the global namespace
      * @result (bool) If the function was found
      */
      bool FromGlobal(ScriptSystem* script_system, const char* name);

      /**
      * @brief Get a Lua function from a scriptable value
      * @param[in] value (foundation::SharedPointer <ScriptableValue>) The scriptable value
      * @result (bool) If the value was a function and loaded correctly
      */
      bool FromValue(foundation::SharedPointer<ScriptableValue> value);

      /**
      * @brief Call the Lua function
      * @param[in] args (sulphur::foundation::Vector
      *                    <sulphur::foundation::SharedPointer
      *                      <sulphur::engine::ScriptableValue>>
      *               The arguments to pass to the function
      * @result (sulphur::foundation::Vector
      *                    <sulphur::foundation::SharedPointer
      *                      <sulphur::engine::ScriptableValue>>
      *           The results from the function
      */
      foundation::Vector<foundation::SharedPointer<ScriptableValue>> Call
      (foundation::Vector<foundation::SharedPointer<ScriptableValue>> args);
    protected:
      ScriptSystem* script_system_; //!< The ScriptSystem the function is in
      const char * key_; //!< Reference to the function in the Lua global table
      int ref_; //!< Reference to the function in the Lua reference table

      bool global_; //!< Is the function on the global table or the reference table
    };
  }
}