#pragma once

#include "engine/scripting/script_state.h"
#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/scriptable_values/scriptable_function.h"

#include <foundation/containers/vector.h>
#include <lua.hpp>

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_CLASS
* @brief Used to annotate a Lua class that can go through "lua-parser"
*/
#define SCRIPT_CLASS(...)

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_NAME
* @param x The name of the Lua class to register the class under
* @brief Used to annotate the name of a Lua class that can go through "lua-parser"
* @remarks Also defines the different tables used for value registry using ScriptRegister
*/
#define SCRIPT_NAME(x)                                                              \
static const char* Name()                                                           \
{                                                                                   \
  return #x;                                                                        \
}                                                                                   \
static sulphur::foundation::Vector<ScriptableFunctionRegister> FunctionRegister();  \
static sulphur::foundation::Vector<ScriptableFunctionRegister> MetaRegister();      \
static sulphur::foundation::Vector<ScriptableEnumRegister> EnumRegister();         

#define SCRIPT_COMPONENT()\
static sulphur::engine::ComponentIDBase* ComponentRegister();

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_ENUM
* @brief Used to annotate an enum of a Lua class that can go through "lua-parser"
*/
#define SCRIPT_ENUM(...)

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_FUNC
* @param static [Optional] Add "static" as a parameter if the function is statically accessible
* @brief Used to annotate a function of a Lua class that can go through "lua-parser"
*/
#define SCRIPT_FUNC(...) __VA_ARGS__

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_FUNC_EX
* @brief Used to annotate a function of a base class to paste them in all derived classes for lua-parser
*/
#define SCRIPT_FUNC_EX(...)

/**
* @file engine/scripting/scriptable_object.h
* @define SCRIPT_EXPAND
* @brief Used to annotate that all expansion functions should be expanded in the class this macro is used in
*/
#define SCRIPT_EXPAND(...)

namespace sulphur 
{
  namespace engine 
  {
    class ComponentIDBase;

    /**
    * @brief lua-parser doesn't really like templated classes and we only care about the "value"
    * portion, so we typedef it.
    */
    using ScriptHandle = foundation::SharedPointer<ScriptableValue>;

    /**
    * @struct sulphur::engine::ScriptableFunctionRegister
    * @brief Struct containing the data for a function that will be exposed to Lua
    * @author Rodi Lankester
    */
    struct ScriptableFunctionRegister
    {
      const char * name; //!< The name of the function
      ScriptFunction function; //!< The function
    };

    /**
    * @struct sulphur::engine::ScriptableEnumRegister
    * @brief All data used to register an enum to the ScriptState
    * @author Daniel Konings
    */
    struct ScriptableEnumRegister
    {
      /**
      * @struct sulphur::engine::ScriptableEnumRegister::Value
      * @brief A structure representing an enumerator value
      * @author Daniel Konings
      */
      struct Value
      {
        const char* name; //!< The name/key of the enumerator value
        int value; //!< The actual underlying value
      };

      const char* name; //!< The name of the enumerator
      foundation::Vector<Value> values; //!< The values contain within the enumerator
    };

    /**
    * @class sulphur::engine::ScriptableObject
    * @brief Base class for exposing C++ classes to Lua
    * @author Rodi Lankester
    */
    class ScriptableObject
    {
    public:

      /**
      * @brief Set the meta table of a scriptable value
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The scriptable value
      * @param[in] libname (const char*) The library you want to use the meta table from
      */
      static void SetMetaTable(foundation::SharedPointer<ScriptableValue> value, const char* libname);

      /**
      * @brief Check if userdata value has metatable libname and return as type T
      * @param[in] libname (const char*) The library you want to check the meta table against
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The scriptable value
      */
      template <typename T>
      static T* CheckUserData(const char* libname, foundation::SharedPointer<ScriptableValue> value);
    };

    //------------------------------------------------------------------------------
    template <typename T>
    inline T* ScriptableObject::CheckUserData(
      const char* libname, 
      foundation::SharedPointer<ScriptableValue> value)
    {
      char metaname[128];
      sprintf_s(metaname, "sulphur.%s", libname);
      value->Push();
      return reinterpret_cast<T*>(luaL_checkudata(value->script_state()->lua_state(), -1, metaname));
    }
  }
}