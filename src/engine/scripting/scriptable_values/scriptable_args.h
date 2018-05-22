#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/script_utils.h"

namespace sulphur
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptableArgs
    * @brief Various functions to deal with variable arguments and return values from Lua
    * @author Rodi Lankester
    */
    class ScriptableArgs
    {
    public:
      /**
      * @brief Constructs scriptable args
      * @param[in] args (foundation::Vector<foundation::SharedPointer<sulphur::engine::ScriptableValue>>)
      * list of arguments received from Lua
      * @param[in] lua_state() (lua_State*) the lua state the function is called from
      */
      ScriptableArgs(
        foundation::Vector<foundation::SharedPointer<sulphur::engine::ScriptableValue>> args,
        ScriptSystem* script_system);

      /**
      * @brief Get the number of arguments
      * @return (int) The number of arguments
      */
      int NumArgs();
      /**
      * @brief Checks the incoming arguments and throws a lua error if they are not the same
      * @details  Accepts the following types for format:
      * @details - string as S
      * @details - number as N
      * @details - bool as B
      * @details - object as O
      * @details - userdata as U
      * @details - thread as T
      * @details - function as F
      * @param[in] format (const char*) Types to check arguments against
      * @param[opt in] check_args (int) amount of args to check, -1 means all args
      */
      void Check(const char* format, int check_args = -1);
      /**
      * @brief Gets an argument at index idx
      * @param[in] idx (int) the index of the argument to fetch
      * @return (sulphur::foundation::SharedPointer<ScriptableValue>) the argument
      */
      foundation::SharedPointer<ScriptableValue> GetArg(size_t idx);

      /**
      * @brief Gets an argument at index idx a type T
      * @param[in] idx (int) the index of the argument to fetch
      * @return (T) the argument
      */
      template <typename T>
      T GetArgAs(size_t idx)
      {
        return ScriptUtils::As<T>(GetArg(idx));
      };


      /**
      * @brief Adds a return value
      * @param[in] return_value (foundation::SharedPointer<ScriptableValue>) the value to return
      */
      void AddReturn(foundation::SharedPointer<ScriptableValue> return_value);

      /**
      * @brief Adds a return value of type T
      * @param[in] return_value (T) the value to return
      */
      template <typename T>
      void AddReturn(T return_value)
      {
        AddReturn(ScriptUtils::Instantiate(script_system_, return_value));
      };

      /**
      * @brief Get the script state where the function was called from
      * @return (ScriptSystem*) the lua state
      */
      ScriptSystem* script_system();

      /**
      * @brief Return the current list of return values to Lua
      */
      void Return();

      /**
      * @brief Returns the amount of return values
      * @return (int) The amount of return values
      */
      int ReturnCount();
    private:
      foundation::Vector<
        foundation::SharedPointer<sulphur::engine::ScriptableValue>
      > args_; //!< List of arguments received from Lua
      foundation::Vector<
        foundation::SharedPointer<sulphur::engine::ScriptableValue>
      > return_values_; //!< List of return values to return to Lua
      ScriptSystem* script_system_; //!< The script state the function is called from
    };

  }
}