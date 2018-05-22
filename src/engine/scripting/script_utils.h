#pragma once
#include "engine/scripting/scriptable_value.h"

#include "engine/scripting/scriptable_values/scriptable_function.h"
#include "engine/scripting/scriptable_values/scriptable_number.h"
#include "engine/scripting/scriptable_values/scriptable_string.h"
#include "engine/scripting/scriptable_values/scriptable_table.h"
#include "engine/scripting/scriptable_values/scriptable_bool.h"
#include "engine/scripting/scriptable_values/scriptable_userdata.h"

#include "engine/scripting/script_system.h"
#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>


namespace sulphur
{
  namespace engine 
  {
    class ScriptSystem;
    /**
    * @class sulphur::engine::ScriptUtils
    * @brief Various functions to help create and modify scriptablevalues
    * @author Rodi Lankester
    */
    class ScriptUtils
    {
    public:
      /**
      * @brief Log function specific to calls from the scripting environment
      * @param[in] verbosity (sulphur::foundation::Verbosity) the verbosity of the message logged
      * @param[in] format (sulphur::foundation::String) The format of the message 
      * @param[in] args (Args&&...) Arguments for format specification
      */
      template<typename ...Args>
      static void ScriptLog(foundation::Verbosity verbosity, foundation::String format, Args&&... args)
      {
        foundation::Logger<
          foundation::LoggingChannel::kScripting,
          foundation::DefaultFormat,
          foundation::DefaultTarget>::Log(
            verbosity,
            format,
            args...
          );
      };

      /**
      * @brief Create a ScriptableFunction
      * @param[in]  state (sulphur::engine::ScriptSystem*)
      *            The ScriptSystem to create the SciptableFunction in
      * @param[in] function (sulphur::engine::ScriptFunction)
      *             The function to expose to Lua
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The created ScriptableFunction
      */
      static foundation::SharedPointer<ScriptableValue> CreateFunction(
        ScriptSystem* state, 
        ScriptFunction function,
        bool is_lib = false
      );

      /**
      * @brief Create a ScriptableTable
      * @param[in] state (sulphur::engine::ScriptSystem*) 
      *            The ScriptSystem to create the SciptableObject in
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) 
      *         The created ScriptableTable
      */
      static foundation::SharedPointer<ScriptableValue> CreateTable(ScriptSystem* state);

      /**
      * @brief Set key to value inside a ScriptableValue with type kObject
      * @param[in] object (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) 
      *            The object 
      * @param[in] key (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) 
      *            The key
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) 
      *            The value
      */
      static void SetObjectValue(
        foundation::SharedPointer<ScriptableValue> object,
        foundation::SharedPointer<ScriptableValue> key,
        foundation::SharedPointer<ScriptableValue> value
      );

      /**
      * @brief Convert a ScriptableObject to a map
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      * The ScriptableObject to convert to a map
      * @return (sulphur::foundation::Map 
      *          <sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>, 
                  sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>>) 
      *         The map
      */
      static foundation::Map<
        foundation::SharedPointer<ScriptableValue>,
        foundation::SharedPointer<ScriptableValue>
      > GetObject(foundation::SharedPointer<ScriptableValue> value);

      /**
      * @brief Get the type of a ScriptableValue
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The ScriptableValue
      * @return (sulphur::engine::ScriptableValueType) the type of the ScriptableValue
      */
      static ScriptableValueType GetType(foundation::SharedPointer<ScriptableValue> value);

      /**
      * @brief Gets the value of a ScriptableValue as type T
      * @details Accepts the following for T:
      * @details - lua_Number
      * @details - float
      * @details - int
      * @details - unsigned int
      * @details - bool
      * @details - const char*
      * @details - void*
      * @details Other types will return the default of that type
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) 
      *            The ScriptableValue to get the value from
      * @return (T) The ScriptableValue's value
      */
      template <typename T>
      static T As(foundation::SharedPointer<ScriptableValue> value);

      /**
      * @brief Instantiate a ScriptableValue with value NIL
      * @param[in] state (sulphur::engine::ScriptSystem*) The state to instantiate the ScriptableValue in
      */
      static foundation::SharedPointer<ScriptableValue> Instantiate(ScriptSystem* state);

      /**
      * @brief Instantiate a ScriptableValue with type T value
      * @details Accepts the following for T:
      * @details - lua_Number
      * @details - float
      * @details - int 
      * @details - unsigned int
      * @details - bool
      * @details - const char*
      * @details - void*
      * @details Other types will be interpreted as NIL
      * @param[in] state (sulphur::engine::ScriptSystem*) 
      *            The state to instantiate the ScriptableValue in
      * @param[in] value (T) The value to instantiate the ScriptableValue with
      */
      template<typename T>
      static foundation::SharedPointer<ScriptableValue> Instantiate(
        ScriptSystem* state,
        T value
      ); 

      /**
      * @brief Set a ScriptableValue to a value of type T
      * @details Accepts the following for T:
      * @details - lua_Number
      * @details - float
      * @details - int
      * @details - unsigned int
      * @details - bool
      * @details - const char*
      * @details - void*
      * @details Other types will be interpreted as NIL
      * @param[in] state (sulphur::engine::ScriptSystem*)
      *            The state the ScriptableValue is in
      * @param[in] scriptable_value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The ScriptableValue to get the value from
      * @param[in] value (T) The value to set the ScriptableValue to
      */
      template<typename T>
      static void Set(
        ScriptSystem* script_system, 
        foundation::SharedPointer<ScriptableValue> scriptable_value, 
        T value
      );
    };


    /// @cond
    //------------------------------------------------------------------------------

    template <typename T>
    inline T ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      return T();
    }

    //------------------------------------------------------------------------------

    template <>
    inline lua_Number ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      lua_Number res = lua_tonumber(value->script_system()->lua_state(), -1);
      lua_pop(value->script_system()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline float ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<float>(As<lua_Number>(value));
    }

    //------------------------------------------------------------------------------


    template <>
    inline int ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<int>(As<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline unsigned int ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<unsigned int>(As<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline bool ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      bool res = lua_toboolean(value->script_system()->lua_state(), -1) != 0;
      lua_pop(value->script_system()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline const char* ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      const char * res = lua_tostring(value->script_system()->lua_state(), -1);
      lua_pop(value->script_system()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline void* ScriptUtils::As(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      void * res = lua_touserdata(value->script_system()->lua_state(), -1);
      lua_pop(value->script_system()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <typename T>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptSystem* state, 
      T
    )
    {
      return foundation::Memory::ConstructShared<ScriptableValue>(state);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<lua_Number>(
      ScriptSystem* state, 
      lua_Number value
      )
    {
      return foundation::Memory::ConstructShared<ScriptableNumber>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<float>(
      ScriptSystem* state, 
      float value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<int>(
      ScriptSystem* state, 
      int value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<size_t>(
      ScriptSystem* state,
      size_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<unsigned int>(
      ScriptSystem* state, 
      unsigned int value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template<>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptSystem* state,
      const char* value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableString>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptSystem* state, 
      bool value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableBool>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<void*>(
      ScriptSystem* state, 
      void* value
      )
    {
      return foundation::Memory::ConstructShared<ScriptableUserData>(state, value);
    }

    //------------------------------------------------------------------------------

    template<typename T>
    inline void ScriptUtils::Set(
      ScriptSystem* script_system,
      foundation::SharedPointer<ScriptableValue> scriptable_value, 
      T value)
    {
      foundation::SharedPointer<ScriptableValue> temp 
        = ScriptUtils::Instantiate<T>(script_system, value);     
      if (scriptable_value->global() == true)
      {
        temp->Push();
        lua_setglobal(scriptable_value->script_system()->lua_state(), scriptable_value->key());
      }
      else
      {
        lua_unref(scriptable_value->script_system()->lua_state(), scriptable_value->ref());
        temp->Push();
        luaL_ref(scriptable_value->script_system()->lua_state(), LUA_REGISTRYINDEX);
      }
    }
    /// @endcond
  }
}
