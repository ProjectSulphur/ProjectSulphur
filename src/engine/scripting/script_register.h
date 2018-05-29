#pragma once

#include "engine/scripting/script_state.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_object.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"
#include "engine/scripting/scriptable_values/scriptable_table.h"
#include "engine/core/handle_base.h"

#include <foundation/containers/map.h>

namespace sulphur
{
  namespace engine
  {
    class Entity;

    /**
    * @struct sulphur::engine::ScriptClassRegister
    * @brief Used to register classes by type and constructability
    * @tparam (T) The type of class to register, should be a ScriptableObject/Component
    * @tparam (C) Is the class constructable? If false, "New" will not be added to the class metatable
    * @author Daniel Konings
    */
    template <typename T, bool C = false>
    struct ScriptClassRegister
    {
      using type = T;
      static const bool constructable = C;
    };

    /**
    * @class sulphur::engine::ScriptRegister
    * @brief Handles registering classes to the scripting environment
    * @author Rodi Lankester
    */
    class ScriptRegister
    {
    public:

      /**
      * @brief Used to only register "New" to constructable classes
      * @tparam (T) The class to register
      * @tparam (bool) Is the class constructable?
      * @remarks This is necessary because free functions cannot be specialized
      * @author Daniel Konings
      */
      template <typename T, bool C>
      struct ImplNew;

      /**
      * @brief Used to only register "New" to constructable classes
      * @tparam (T) The class to register
      * @remarks Used when the class is constructable
      * @author Daniel Konings
      */
      template <typename T>
      struct ImplNew<T, true>
      {
        /**
        * @brief Used to instantiate classes with ClassName.New() in the script state
        * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from the script state
        * @remarks The arguments are passed on to the ScriptableObject's constructor
        */
        static void New(ScriptableArgs* args)
        {
          void* mem = reinterpret_cast<T*>(lua_newuserdata(args->script_state()->lua_state(), sizeof(T)));
          new(mem) T(args);
          foundation::SharedPointer<ScriptableValue> value = args->script_state()->GetFromStack(-1);
          ScriptableObject::SetMetaTable(value, T::Name());
          args->AddReturn(value);
        }
      };

      /**
      * @brief Used to only register "New" to constructable classes
      * @tparam (T) The class to register
      * @remarks Used when the class is not constructable
      * @author Daniel Konings
      */
      template <typename T>
      struct ImplNew<T, false>
      {
        /**
        * @brief Unimplemented "New" for non-constructable classes
        * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from the script state
        */
        static void New(ScriptableArgs* /*args*/)
        {
          PS_LOG(Error, "Class %s cannot be instantiated", T::Name());
        }
      };

      /**
      * @struct sulphur::engine::ScriptRegister::ImplRegister<Args...>
      * @tparam (Args...) The chain of types to register
      * @brief Used because free functions cannot be specialized under the current standard
      * @remarks This has to be done as there is no template parameter or return type to deduce
      * the template argument from
      */
      template <typename... Args>
      struct ImplRegister;
      
      /**
      * @struct sulphur::engine::ScriptRegister::ImplRegister<Class, Args...>
      * @tparam (Class) The class currently being register
      * @tparam (Args...) The chain of types to register
      * @see sulphur::engine::ScriptRegister::ImplRegister<Args...>
      * @remarks Used when there are still arguments left in the chain
      */
      template <typename Class, typename ... Args>
      struct ImplRegister<Class, Args...>
      {
        /**
        * @brief Registers a class to the script state through the ScriptRegister
        * @param[in] reg (sulphur::engine::ScriptRegister&) The script register to register to
        */
        static void Register(ScriptRegister& reg)
        {
          reg.Register<typename Class::type, Class::constructable>();
          reg.RegisterAll<Args...>();
        }
      };

      /**
      * @tparam (T) The type to check
      * @brief Used to check if T is a sulphur::engine::ComponentHandleBase
      */
      template <typename T>
      using is_component = typename std::enable_if<std::is_base_of<ComponentHandleBase, T>::value && !std::is_same<T, Entity>::value, void>::type;

      /**
      * @tparam (T) The type to check
      * @brief Used to check if T is a sulphur::engine::ScriptableObject
      */
      template <typename T>
      using is_scriptable = typename std::enable_if<std::is_base_of<ScriptableObject, T>::value || std::is_same<T, Entity>::value, void>::type;

      /**
      * @brief Initialize the ScriptRegister for a certain script state
      * @param[in] script_state (sulphur::engine::ScriptState*) The script state
      */
      ScriptRegister(ScriptState* script_state);

      /**
      * @brief Registers ScriptableObjects/Components by a template chain of ClassRegisters
      * @tparam (Args...) The following classes in the argument chain
      * @remarks This is currently untested
      * @todo Daniel: Test this code if it actually unrolls properly
      */
      template <typename ... Args>
      void RegisterAll();

      /**
      * @brief Finish the registration of a template argument chain of ClassRegisters
      * @remarks Registers all types in the chain in a scripting "Types" enumeration
      */
      void RegisterAll();

    protected:

      /**
      * @brief The "New" function, Lua-sided, for all constructable classes
      * @tparam (T) A ScriptableObject this "New" function exists for
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed into the "New" function
      * @remarks The arguments are passed onto the ScriptableObject's Lua constructor
      */
      template <typename T, bool C>
      static void New(ScriptableArgs* args);

      /**
      * @brief Registers a component to the script state
      * @tparam (T) The class to register
      * @tparam (C) Is the class constructable?
      */
      template <typename T, bool C>
      is_component<T> Register();

      /**
      * @brief Registers a ScriptableObject to the script state
      * @tparam (T) The class to register
      * @tparam (C) Is the class constructable?
      */
      template <typename T, bool C>
      is_scriptable<T> Register();

      /**
      * @brief Register all types in a "Types" enumerator
      */
      void RegisterTypes();

      /**
      * @brief Register class T that inherits from sulphur::engine::ScriptableObject
      * to the scripting environment
      * @tparam (T) The class to register
      * @tparam (C) Is the class constructable?
      */
      template <typename T, bool C>
      void RegisterInternal(int id);

    protected:
      ScriptState* script_state_; //!<  The script state the script register is for

      /**
      * @brief temporary list of registered functions
      */
      foundation::Vector<foundation::SharedPointer<ScriptableValue>> registered_values;

      static const int kIdOffset_; //!< The ID offset to skip all component types for different types
      static int current_id_; //!< The current type ID offset to be incremented as a type is registered

      foundation::Map<foundation::String, int> id_map_; //!< A typename to ID mapping
    };

    /**
    * @struct sulphur::engine::ScriptRegister::ImplRegister<>
    * @see sulphur::engine::ScriptRegister::ImplRegister<Args...>
    * @remarks Used when there are no arguments left, register all types to the script state
    */
    template <>
    struct ScriptRegister::ImplRegister<>
    {
      /**
      * @brief Registers the list of "Types" to the script state
      * @param[in] reg (sulphur::engine::ScriptRegister&) The script register to register to
      */
      static void Register(ScriptRegister& reg)
      {
        reg.RegisterTypes();
      }
    };

    //------------------------------------------------------------------------------
    template <typename T, bool C>
    inline ScriptRegister::is_component<T> ScriptRegister::Register()
    {
      size_t id = foundation::type_id<T>();
      RegisterInternal<T, false>(static_cast<int>(id));
      ComponentIDBase* comp = T::ComponentRegister();

      script_state_->RegisterComponentID(id, comp);
    }

    //------------------------------------------------------------------------------
    template <typename T, bool C>
    inline ScriptRegister::is_scriptable<T> ScriptRegister::Register()
    {
      RegisterInternal<T, C>(-1);
    }

    //------------------------------------------------------------------------------
    template <typename T, bool C>
    inline void ScriptRegister::RegisterInternal(int id)
    {
      foundation::Vector<ScriptableFunctionRegister> func_reg = T::FunctionRegister();
      foundation::Vector<ScriptableFunctionRegister> meta_reg = T::MetaRegister();
      foundation::Vector<ScriptableEnumRegister> enum_reg = T::EnumRegister();

      id = id == -1 ? kIdOffset_ + (++current_id_) : id;
      id_map_[T::Name()] = id;

      lua_State* lua_state = script_state_->lua_state();
      const char * libname = T::Name();
      int size = static_cast<int>(func_reg.size());
      char metaname[128];
      sprintf_s(metaname, "sulphur.%s", libname);

      luaL_newmetatable(script_state_->lua_state(), metaname);

      lua_pushstring(lua_state, "__index");
      lua_pushvalue(lua_state, -2);  /* pushes the metatable */
      lua_settable(lua_state, -3);  /* metatable.__index = metatable */

      for (int i = 0; i < meta_reg.size(); ++i)
      {
        foundation::SharedPointer<ScriptableValue> func
          = ScriptUtils::CreateFunction(
            script_state_,
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

      ScriptableFunctionRegister r;
      r.name = "New";
      r.function = ImplNew<T, C>::New;

      func_reg.push_back(r);

      for (int i = 0; i < func_reg.size(); ++i)
      {
        foundation::SharedPointer<ScriptableValue> func
          = ScriptUtils::CreateFunction(
            script_state_,
            func_reg[i].function,
            true);
        registered_values.push_back(func);
        func->Push();
        lua_setfield(lua_state, -2, func_reg[i].name);
      }

      lua_pop(lua_state, -1);  /* remove upvalues */

      for (int i = 0; i < enum_reg.size(); ++i)
      {
        const ScriptableEnumRegister& reg = enum_reg.at(i);
        foundation::SharedPointer<ScriptableTable> e = ScriptUtils::CreateTable(script_state_);

        for (int j = 0; j < reg.values.size(); ++j)
        {
          const ScriptableEnumRegister::Value& val = reg.values.at(j);
          e->Insert(
            val.name,
            val.value
          );
        }

        script_state_->RegisterValue(reg.name, e);
      }

      script_state_->RegisterMetatable(id, sizeof(T), libname);
    }

    //------------------------------------------------------------------------------
    template <typename ... Args>
    inline void ScriptRegister::RegisterAll()
    {
      ImplRegister<Args...>::Register(*this);
    }

    //------------------------------------------------------------------------------
    inline void ScriptRegister::RegisterAll()
    {
      RegisterTypes();
    }
  }
}