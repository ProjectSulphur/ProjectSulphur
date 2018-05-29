#pragma once

#include "engine/scripting/scriptable_value.h"
#include <foundation/memory/memory.h>

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
      * @brief Default constructor
      */
      ScriptableCallback();

      /**
      * @brief Get a Lua function from the global namespace
      * @param[in] script_state (sulphur::engine::ScriptState*) The state to get the function from
      * @param[in] name (const char*) The name of the function in the global namespace
      * @result (bool) If the function was found
      */
      bool FromGlobal(ScriptState* script_state, const char* name);

      /**
      * @brief Get a Lua function from a scriptable value
      * @param[in] value (foundation::SharedPointer <ScriptableValue>) The scriptable value
      * @result (bool) If the value was a function and loaded correctly
      */
      bool FromValue(foundation::SharedPointer<ScriptableValue> value);

      /**
      * @brief Overload the compare operator to check against the ptr_ instead of comparing classes directly
      * @param[in] other (const ScriptableCallback&) The scriptable callback to compare this class to
      * @returns (bool) Was it the same function
      */
      bool operator==(const ScriptableCallback& other);

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
      ScriptState* script_state_; //!< The ScriptState the function is in
      const char * key_; //!< Reference to the function in the Lua global table
      int ref_; //!< Reference to the function in the Lua reference table

      bool global_; //!< Is the function on the global table or the reference table
      bool uninitialized_log_; //!< Used to log if this callback is initialized, but not spam the console

      void* ptr_; //!< Raw pointer to the Lua function
    };
  }
}