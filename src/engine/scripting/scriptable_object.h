#pragma once
#include "engine/scripting/script_system.h"
#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/scriptable_values/scriptable_function.h"
#include <foundation/containers/vector.h>
namespace sulphur 
{
  namespace engine 
  {
    /**
    * @struct ScriptableFunctionRegister
    * @brief Struct containing the data for a function that will be exposed to Lua
    * @author Rodi Lankester
    */
    struct ScriptableFunctionRegister
    {
      const char* name; //!< The name of the function
      ScriptFunction function; //!< The function
    };

    /**
    * @class ScriptableObject
    * @brief Base class for exposing C++ classes to Lua
    * @author Rodi Lankester
    */
    class ScriptableObject
    {
    public:

      /**
      * @brief Set the meta table of a scriptable value
      * @param[in] value (foundation::SharedPointer <ScriptableValue>) The scriptable value
      * @param[in] libname (const char*) The library you want to use the meta table from
      */
      static void SetMetaTable(foundation::SharedPointer<ScriptableValue> value, const char* libname);

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char*) the name
      */
      static const char* Name()
      {
        return "NULL";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (foundation::Vector <FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return {};
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (foundation::Vector <FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return {};
      }

      /**
      * @brief Check if userdata value has metatable libname and return as type T
      * @param[in] libname (const char*) The library you want to check the meta table against
      * @param[in] value (foundation::SharedPointer <ScriptableValue>) The scriptable value
      */
      template <typename T>
      static T* CheckUserData(const char* libname, foundation::SharedPointer<ScriptableValue> value)
      {
        char metaname[128];
        sprintf_s(metaname, "sulphur.%s", libname);
        value->Push();
        return reinterpret_cast<T*>(luaL_checkudata(value->script_system()->lua_state(), -1, metaname));
      }
    };
  }
}