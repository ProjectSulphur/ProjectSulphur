#pragma once
#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"

#include <foundation/memory/memory.h>
#include <lua.hpp>

namespace sulphur
{
  namespace engine 
  {
    class ScriptableObject;
    class ComponentHandleBase;

    /**
    * @class sulphur::engine::ScriptableArgs
    * @brief Various functions to deal with variable arguments and return values from Lua
    * @author Rodi Lankester
    */
    class ScriptableArgs
    {
    public:

      /**
      * @brief Used to check if a T param is an enumerator
      * @tparam (T) The type to check for
      * @tparam (C) Should it be an enumerator or not?
      * @remarks Used with SFINAE to create the appropriate overload for scriptable value conversion
      */
      template <typename T, bool C>
      using is_enum = typename std::enable_if<std::is_enum<T>::value == C, void>::type;

      /**
      * @brief Constructs scriptable args
      * @param[in] args (foundation::Vector<foundation::SharedPointer<sulphur::engine::ScriptableValue>>)
      * list of arguments received from Lua
      * @param[in] lua_state() (lua_State*) the lua state the function is called from
      */
      ScriptableArgs(
        foundation::Vector<foundation::SharedPointer<sulphur::engine::ScriptableValue>>& args,
        ScriptState* script_state);

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
      * @brief Adds a return value
      * @param[in] return_value (foundation::SharedPointer<ScriptableValue>) the value to return
      */
      void AddReturn(foundation::SharedPointer<ScriptableValue> return_value);

      /**
      * @brief Adds a return value of type T
      * @param[in] return_value (T) the value to return
      * @remarks Default call when T is not of a supported argument type
      */
      template <typename T>
      void AddReturn(T return_value)
      {
        AddReturn(ScriptUtils::Instantiate(script_state_, return_value));
      };

      /**
      * @see https://stackoverflow.com/questions/257288/is-it-possible-to-write-a-template-to-check-for-a-functions-existence
      */
      template <typename T>
      struct has_scriptable_name
      {
        typedef char one;
        typedef long two;

        template <typename C> static one test(decltype(&C::Name));
        template <typename C> static two test(...);

      public:
        enum { value = sizeof(test<T>(0)) == sizeof(char) };
      };

      /**
      * @brief Adds userdata as a return type
      * @remarks memcpys the data from C++ memory into script state memory, 
      * don't use objects allocated through our memory interface, use stack allocated objects instead
      * @param[in] return_value (T) The value to return
      */
      template <typename T>
      is_enum<T, false> AddUserData(T return_value)
      {
        static_assert(
          (std::is_base_of<ScriptableObject, T>::value ||
          std::is_base_of<ComponentHandleBase, T>::value) &&
          has_scriptable_name<T>::value,
          "Argument T for AddUserData is not of a supported scriptable value");

        T* mem = reinterpret_cast<T*>(lua_newuserdata(script_state()->lua_state(), sizeof(T)));
        mem = new (mem) T();
        *mem = return_value;

        foundation::SharedPointer<ScriptableValue> ud = script_state()->GetFromStack(-1);
        ScriptableObject::SetMetaTable(ud, T::Name());

        AddReturn(ud);
      }

      template <typename T>
      is_enum<T, true> AddUserData(T return_value)
      {
        AddReturn<int>(static_cast<int>(return_value));
      }

      /**
      * @brief Get the script state where the function was called from
      * @return (ScriptState*) the lua state
      */
      ScriptState* script_state();

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
      >& args_; //!< List of arguments received from Lua
      foundation::Vector<
        foundation::SharedPointer<sulphur::engine::ScriptableValue>
      > return_values_; //!< List of return values to return to Lua
      ScriptState* script_state_; //!< The script state the function is called from
    };
  }
}