#pragma once
#include "engine/core/component_id.h"

#include "engine/scripting/scriptable_value.h"

#include "engine/scripting/scriptable_values/scriptable_function.h"
#include "engine/scripting/scriptable_values/scriptable_number.h"
#include "engine/scripting/scriptable_values/scriptable_string.h"
#include "engine/scripting/scriptable_values/scriptable_bool.h"
#include "engine/scripting/scriptable_values/scriptable_userdata.h"
#include "engine/scripting/scriptable_values/scriptable_table.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/scriptable_object.h"

#include "engine/scripting/script_state.h"
#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>
#include <foundation/utils/color.h>
#include <foundation/math/ray.h> 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace sulphur
{
  namespace engine 
  {
    class ScriptState;

    /**
    * @class sulphur::engine::ScriptUtils
    * @brief Various functions to help create and modify scriptablevalues
    * @author Rodi Lankester
    */
    class ScriptUtils
    {
    public:

      /**
      * @brief Used to check if a T param is an enumerator
      * @tparam (T) The type to check for
      * @tparam (C) Should it be an enumerator or not?
      * @remarks Used with SFINAE to create the appropriate overload for scriptable value conversion
      */
      template <typename T, bool C>
      using is_enum = typename std::enable_if<std::is_enum<T>::value == C, T>::type;

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
      * @param[in]  state (sulphur::engine::ScriptState*)
      *            The ScriptState to create the SciptableFunction in
      * @param[in] function (sulphur::engine::ScriptFunction)
      *             The function to expose to Lua
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The created ScriptableFunction
      */
      static foundation::SharedPointer<ScriptableValue> CreateFunction(
        ScriptState* state, 
        ScriptFunction function,
        bool is_lib = false
      );

      /**
      * @brief Create a ScriptableTable
      * @param[in] state (sulphur::engine::ScriptState*) 
      *            The ScriptState to create the SciptableObject in
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableTable>) 
      *         The created ScriptableTable
      */
      static foundation::SharedPointer<ScriptableTable> CreateTable(ScriptState* state);

      /**
      * @brief Get a scriptable value as a table
      * @param[in] table (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The value you want to convert to a tabl
            * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableTable>) 
      *         The value as a ScriptableTable (nullptr if the value was not a table)
      */
      static foundation::SharedPointer<ScriptableTable> GetTable(foundation::SharedPointer<ScriptableValue> table);

      /**
      * @brief Instantiates a script handle by type ID and data
      * @param[in] state (sulphur::engine::ScriptState*) The state to create the handle in
      * @param[in] id (int) The type ID of the metatable that should be retrieved
      * @param[in] handle (void*) The handle to remap to a scriptable value
      * @return (sulphur::engine::ScriptHandle) The scriptable handle to pass to the script state
      */
      static ScriptHandle InstantiateType(ScriptState* state, int id, void* handle);

      /**
      * @brief Get the type of a ScriptableValue
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The ScriptableValue
      * @return (sulphur::engine::ScriptableValueType) the type of the ScriptableValue
      */
      static ScriptableValueType GetType(foundation::SharedPointer<ScriptableValue> value);

    protected:
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
      static T AsImpl(foundation::SharedPointer<ScriptableValue> value)
      {
        return T();
      }

    public:

      /**
      * @brief Called when the T parameter is not an enumerator
      * @tparam (T) The value to convert to
      * @param[in] value (sulphur::foundation::SharedPointer<sulphur::foundation::ScriptableValue>) The value to convert
      * @return (T) The converted value
      */
      template <typename T>
      static T As(foundation::SharedPointer<ScriptableValue> value, is_enum<T, false>* = nullptr)
      {
        return AsImpl<T>(value);
      }

      /**
      * @brief Called when the T parameter is an enumerator
      * @tparam (T) The value to convert to
      * @param[in] value (sulphur::foundation::SharedPointer<sulphur::foundation::ScriptableValue>) The value to convert
      * @return (T) The converted value
      */
      template <typename T>
      static T As(foundation::SharedPointer<ScriptableValue> value, is_enum<T, true>* = nullptr)
      {
        return static_cast<T>(AsImpl<int>(value));
      }

      /**
      * @brief Instantiate a ScriptableValue with value NIL
      * @param[in] state (sulphur::engine::ScriptState*) The state to instantiate the ScriptableValue in
      */
      static foundation::SharedPointer<ScriptableValue> Instantiate(ScriptState* state);

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
      * @param[in] state (sulphur::engine::ScriptState*) 
      *            The state to instantiate the ScriptableValue in
      * @param[in] value (T) The value to instantiate the ScriptableValue with
      */
      template<typename T>
      static foundation::SharedPointer<ScriptableValue> Instantiate(
        ScriptState* state,
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
      * @param[in] state (sulphur::engine::ScriptState*)
      *            The state the ScriptableValue is in
      * @param[in] scriptable_value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The ScriptableValue to get the value from
      * @param[in] value (T) The value to set the ScriptableValue to
      */
      template<typename T>
      static void Set(
        ScriptState* script_state, 
        foundation::SharedPointer<ScriptableValue> scriptable_value, 
        T value
      );
    };


    //------------------------------------------------------------------------------

    template <>
    inline ScriptHandle ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return value;
    }

    //------------------------------------------------------------------------------

    template <>
    inline lua_Number ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      lua_Number res = lua_tonumber(value->script_state()->lua_state(), -1);
      lua_pop(value->script_state()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline float ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<float>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------


    template <>
    inline int ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<int>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline unsigned int ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<unsigned int>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline int8_t ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<int8_t>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline int16_t ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<int16_t>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline uint8_t ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<uint8_t>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline uint16_t ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return static_cast<uint16_t>(AsImpl<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template <>
    inline bool ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      bool res = lua_toboolean(value->script_state()->lua_state(), -1) != 0;
      lua_pop(value->script_state()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline const char* ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      const char * res = lua_tostring(value->script_state()->lua_state(), -1);
      lua_pop(value->script_state()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::String ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      const char * res = lua_tostring(value->script_state()->lua_state(), -1);
      lua_pop(value->script_state()->lua_state(), 1);
      return foundation::String(res);
    }

    //------------------------------------------------------------------------------

    template <>
    inline void* ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();
      void * res = lua_touserdata(value->script_state()->lua_state(), -1);
      lua_pop(value->script_state()->lua_state(), 1);
      return res;
    }

    //------------------------------------------------------------------------------

    template <>
    inline ScriptableCallback ScriptUtils::AsImpl<ScriptableCallback>(foundation::SharedPointer<ScriptableValue> value)
    {
      ScriptableCallback callback = ScriptableCallback();
      callback.FromValue(value);
      return callback;
    }

    //------------------------------------------------------------------------------

    template <>
    inline glm::vec2 ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::GetTable(value);

      if (table == nullptr || table->GetSize() != 2)
      {
        return glm::vec2(0.0f, 0.0f);
      }

      if (table->Contains("x") == true)
      {
        return glm::vec2(table->Get<float>("x"), table->Get<float>("y"));
      }

      return glm::vec2(table->Get<float>(0), table->Get<float>(1));
    }

    //------------------------------------------------------------------------------

    template <>
    inline glm::vec3 ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::GetTable(value);

      if (table == nullptr || table->GetSize() != 3)
      {
        return glm::vec3(0.0f, 0.0f, 0.0f);
      }

      if (table->Contains("x") == true)
      {
        return glm::vec3(
          table->Get<float>("x"), 
          table->Get<float>("y"),
          table->Get<float>("z"));
      }

      return glm::vec3(
        table->Get<float>(0), 
        table->Get<float>(1),
        table->Get<float>(2));
    }

    //------------------------------------------------------------------------------

    template <>
    inline glm::vec4 ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::GetTable(value);

      if (table == nullptr || table->GetSize() != 4)
      {
        return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
      }

      if (table->Contains("x") == true)
      {
        return glm::vec4(
          table->Get<float>("x"),
          table->Get<float>("y"),
          table->Get<float>("z"),
          table->Get<float>("w"));
      }

      return glm::vec4(
        table->Get<float>(0),
        table->Get<float>(1),
        table->Get<float>(2),
        table->Get<float>(3));
    }

    //------------------------------------------------------------------------------

    template <>
    inline glm::quat ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      glm::vec4 q = AsImpl<glm::vec4>(value);
      return glm::quat(q.w, q.x, q.y, q.z);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::Color ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      return AsImpl<glm::vec4>(value);
    }

    //------------------------------------------------------------------------------
    
    template <>
    inline glm::mat3x3 ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      glm::mat3x3 identity = glm::mat3x3(1.0f);

      foundation::SharedPointer<ScriptableTable> t1 = ScriptUtils::GetTable(value);

      if (t1 == nullptr || t1->GetSize() != 3)
      {
        return identity;
      }

      for (int r = 0; r < 3; ++r)
      {
        foundation::SharedPointer<ScriptableTable> t2 = ScriptUtils::GetTable(t1->Get(r));

        if (t2 == nullptr || t2->GetSize() != 3)
        {
          return identity;
        }

        for (int c = 0; c < 3; ++c)
        {
          identity[r][c] = t2->Get<float>(c);
        }
      }

      return identity;
    }

    //------------------------------------------------------------------------------

    template <>
    inline glm::mat4x4 ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      glm::mat4x4 identity = glm::mat4x4(1.0f);

      foundation::SharedPointer<ScriptableTable> t1 = ScriptUtils::GetTable(value);

      if (t1 == nullptr || t1->GetSize() != 4)
      {
        return identity;
      }

      for (int r = 0; r < 4; ++r)
      {
        foundation::SharedPointer<ScriptableTable> t2 = ScriptUtils::GetTable(t1->Get(r));

        if (t2 == nullptr || t2->GetSize() != 4)
        {
          return identity;
        }

        for (int c = 0; c < 4; ++c)
        {
          identity[r][c] = t2->Get<float>(c);
        }
      }

      return identity;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::Ray ScriptUtils::AsImpl(foundation::SharedPointer<ScriptableValue> value)
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::GetTable(value);
      foundation::Ray ray;

      if (table->Contains("direction") == true)
      {
        ray.origin = table->Get<glm::vec3>("origin");
        ray.direction = table->Get<glm::vec3>("direction");
      }
      else
      {
        ray.origin = table->Get<glm::vec3>(0);
        ray.direction = table->Get<glm::vec3>(1);
      }

      return ray;
    }


    //------------------------------------------------------------------------------
    template <typename T>
    inline T ScriptableTable::Get(int key)
    {
      foundation::SharedPointer<ScriptableValue> result
        = Get(ScriptUtils::Instantiate(script_state_, key + 1));

      return ScriptUtils::As<T>(result);
    }

    //------------------------------------------------------------------------------
    template <typename T>
    inline T ScriptableTable::Get(const char* key)
    {
      foundation::SharedPointer<ScriptableValue> result
        = Get(ScriptUtils::Instantiate(script_state_, key));

      return ScriptUtils::As<T>(result);
    }

    //------------------------------------------------------------------------------
    template <typename Value>
    inline void ScriptableTable::Insert(const char* key, Value value)
    {
      Insert(ScriptUtils::Instantiate(script_state_, key), ScriptUtils::Instantiate(script_state_, value));
    }

    //------------------------------------------------------------------------------
    template <typename Value>
    inline void ScriptableTable::Insert(int key, Value value)
    {
      Insert(ScriptUtils::Instantiate(script_state_, key + 1), ScriptUtils::Instantiate(script_state_, value));
    }

    //------------------------------------------------------------------------------

    template <typename T>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* state, 
      T value
    )
    {
      size_t size = sizeof(T);
      void* mem = lua_newuserdata(state->lua_state(), size);
      memcpy(mem, &value, size);

      foundation::SharedPointer<ScriptableValue> ud = state->GetFromStack(-1);
      ScriptableObject::SetMetaTable(ud, T::Name());

      return ud;
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<
      foundation::SharedPointer<ScriptableTable>>(
        ScriptState* /**state*/,
        foundation::SharedPointer<ScriptableTable> value
        )
    {
      return eastl::static_pointer_cast<ScriptableTable, ScriptableValue>(value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<lua_Number>(
      ScriptState* state, 
      lua_Number value
      )
    {
      return foundation::Memory::ConstructShared<ScriptableNumber>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<float>(
      ScriptState* state, 
      float value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<uint8_t>(
      ScriptState* state,
      uint8_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<uint16_t>(
      ScriptState* state,
      uint16_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<uint32_t>(
      ScriptState* state,
      uint32_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<uint64_t>(
      ScriptState* state,
      uint64_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<int8_t>(
      ScriptState* state,
      int8_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<int16_t>(
      ScriptState* state,
      int16_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<int32_t>(
      ScriptState* state,
      int32_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------
    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<int64_t>(
      ScriptState* state,
      int64_t value
      )
    {
      return Instantiate<lua_Number>(state, static_cast<lua_Number>(value));
    }

    //------------------------------------------------------------------------------

    template<>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* state,
      const char* value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableString>(state, value);
    }

    //------------------------------------------------------------------------------

    template<>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* state,
      char* value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableString>(state, value);
    }

    //------------------------------------------------------------------------------

    template<>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* state,
      foundation::String value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableString>(state, value.c_str());
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* state, 
      bool value
    )
    {
      return foundation::Memory::ConstructShared<ScriptableBool>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<void*>(
      ScriptState* state, 
      void* value
      )
    {
      return foundation::Memory::ConstructShared<ScriptableUserData>(state, value);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(
      ScriptState* /**state*/,
      foundation::SharedPointer<ScriptableValue> value
    )
    {
      return value;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::vec2>(
      ScriptState* state,
      glm::vec2 value
      )
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::CreateTable(state);
      table->Insert("x", value.x);
      table->Insert("y", value.y);

      return table;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::vec3>(
      ScriptState* state,
      glm::vec3 value
      )
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::CreateTable(state);
      table->Insert("x", value.x);
      table->Insert("y", value.y);
      table->Insert("z", value.z);

      return table;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<foundation::Ray>(
      ScriptState* state,
      foundation::Ray value
      )
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::CreateTable(state);
      table->Insert("origin", value.origin);
      table->Insert("direction", value.direction);

      return table;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::vec4>(
      ScriptState* state,
      glm::vec4 value
      )
    {
      foundation::SharedPointer<ScriptableTable> table = ScriptUtils::CreateTable(state);
      table->Insert("x", value.x);
      table->Insert("y", value.y);
      table->Insert("z", value.z);
      table->Insert("w", value.w);

      return table;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::quat>(
      ScriptState* state,
      glm::quat value
      )
    {
      return Instantiate<glm::vec4>(state, glm::vec4(value.x, value.y, value.z, value.w));
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<foundation::Color>(
      ScriptState* state,
      foundation::Color value
      )
    {
      return Instantiate<glm::vec4>(state, value.v);
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::mat3x3>(
      ScriptState* state,
      glm::mat3x3 value
      )
    {
      foundation::SharedPointer<ScriptableTable> rows = ScriptUtils::CreateTable(state);

      for (int r = 0; r < 3; ++r)
      {
        foundation::SharedPointer<ScriptableTable> column = ScriptUtils::CreateTable(state);
        
        for (int c = 0; c < 3; ++c)
        {
          column->Insert(c, value[r][c]);
        }

        rows->Insert(r, column);
      }

      return rows;
    }

    //------------------------------------------------------------------------------

    template <>
    inline foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate<glm::mat4x4>(
      ScriptState* state,
      glm::mat4x4 value
      )
    {
      foundation::SharedPointer<ScriptableTable> rows = ScriptUtils::CreateTable(state);

      for (int r = 0; r < 4; ++r)
      {
        foundation::SharedPointer<ScriptableTable> column = ScriptUtils::CreateTable(state);

        for (int c = 0; c < 4; ++c)
        {
          column->Insert(c, value[r][c]);
        }

        rows->Insert(r, column);
      }

      return rows;
    }

    //------------------------------------------------------------------------------

    template<typename T>
    inline void ScriptUtils::Set(
      ScriptState* script_state,
      foundation::SharedPointer<ScriptableValue> scriptable_value, 
      T value)
    {
      foundation::SharedPointer<ScriptableValue> temp 
        = ScriptUtils::Instantiate<T>(script_state, value);     
      if (scriptable_value->global() == true)
      {
        temp->Push();
        lua_setglobal(scriptable_value->script_state()->lua_state(), scriptable_value->key());
      }
      else
      {
        lua_unref(scriptable_value->script_state()->lua_state(), scriptable_value->ref());
        temp->Push();
        luaL_ref(scriptable_value->script_state()->lua_state(), LUA_REGISTRYINDEX);
      }
    }
    /// @endcond
  }
}
