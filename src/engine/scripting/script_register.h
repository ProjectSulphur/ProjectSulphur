#pragma once
#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_object.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::ScriptRegister
    * @brief Handles registering classes to the scripting environment
    * @author Rodi Lankester
    */
    class ScriptRegister
    {
    public:
      /**
      * @brief Initialize the ScriptRegister for a certain script service
      * @param[in] script_service (sulphur::engine::ScriptSystem*) The script service
      */
      ScriptRegister(ScriptSystem* script_service);

      /**
      * @brief Register class T that inherits from sulphur::engine::ScriptableObject
      * to the scripting environment
      */
      template <
        typename T,
        class = typename std::enable_if<std::is_base_of<ScriptableObject, T>::value, T>::type
      >
      void Register();

    protected:
      ScriptSystem * script_service_; //!<  The script service the script register is for

      foundation::Vector<foundation::SharedPointer<ScriptableValue>> registered_values;
      //!< temporary list of registered functions
    };

    //-------------------------------------------------------------------------
    template <typename T, class>
    void ScriptRegister::Register()
    {
      foundation::Vector<ScriptableFunctionRegister> func_reg = T::FunctionRegister();
      foundation::Vector<ScriptableFunctionRegister> meta_reg = T::MetaRegister();
      lua_State* lua_state = script_service_->lua_state();
      const char * libname = T::Name();
      int size = static_cast<int>(func_reg.size());
      char metaname[128];
      sprintf_s(metaname, "sulphur.%s", libname);

      luaL_newmetatable(script_service_->lua_state(), metaname);

      lua_pushstring(lua_state, "__index");
      lua_pushvalue(lua_state, -2);  /* pushes the metatable */
      lua_settable(lua_state, -3);  /* metatable.__index = metatable */

      for (int i = 0; i < meta_reg.size(); ++i)
      {
        foundation::SharedPointer<ScriptableValue> func
          = ScriptUtils::CreateFunction(
            script_service_,
            meta_reg[i].function,
            true);
        registered_values.push_back(func);
        func->Push();
        lua_setfield(lua_state, -2, meta_reg[i].name);
      }

      lua_pop(lua_state, -1);  /* remove upvalues */


                               /* check whether lib already exists */
      luaL_findtable(lua_state, LUA_REGISTRYINDEX, "_LOADED", 16);
      lua_getfield(lua_state, -1, libname);  /* get _LOADED[libname] */

      if (!lua_istable(lua_state, -1))
      {  /* not found? */
        lua_pop(lua_state, 1);  /* remove previous result */
                                /* try global variable (and create one if it does not exist) */
        if (luaL_findtable(lua_state, LUA_GLOBALSINDEX, libname, size) != NULL)
          ScriptUtils::ScriptLog(foundation::Verbosity::kError, "Name conflict for module %s", libname);

        lua_pushvalue(lua_state, -1);
        lua_setfield(lua_state, -3, libname);  /* _LOADED[libname] = new table */
      }

      lua_remove(lua_state, -2);  /* remove _LOADED table */
      lua_insert(lua_state, -1);  /* move library table to below upvalues */


      for (int i = 0; i < func_reg.size(); ++i)
      {
        foundation::SharedPointer<ScriptableValue> func
          = ScriptUtils::CreateFunction(
            script_service_,
            func_reg[i].function,
            true);
        registered_values.push_back(func);
        func->Push();
        lua_setfield(lua_state, -2, func_reg[i].name);
      }

      lua_pop(lua_state, -1);  /* remove upvalues */
    }
  }
}