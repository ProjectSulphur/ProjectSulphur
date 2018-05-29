#pragma once
#include <lua.hpp>
#include <foundation/memory/memory.h>
#include "engine/scripting/scriptable_value.h"

namespace sulphur
{
  namespace engine
  {
    class ScriptUtils;
    class ScriptableTable;

    /**
    * @brief Shorter name containing all needed information for
    * foundation::SharedPointer<ScriptableTable>
    */
    using ScriptTableHandle = foundation::SharedPointer<ScriptableTable>;

    /**
    * @class sulphur::engine::ScriptableTable : public sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable table
    * @author Rodi Lankester
    */
    class ScriptableTable : public ScriptableValue
    {
      friend class sulphur::foundation::Memory;

    public:
      /**
      * @brief Get a value from the table with the given key
      * @param[in] key (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The key at which the value is located
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The value of the key in the table
      */
      foundation::SharedPointer<ScriptableValue> Get(foundation::SharedPointer<ScriptableValue> key);

      /**
      * @brief Get a value from the table with the given indexed key
      * @param[in] key (int) The key at which the value is located
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The value of the key in the table
      * @remarks Counts from 0, not 1 like Lua
      */
      foundation::SharedPointer<ScriptableValue> Get(int key);

      /**
      * @brief Get a value from the table with the given string key
      * @param[in] key (const char*) The string key at which the value is located
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The value of the key in the table
      */
      foundation::SharedPointer<ScriptableValue> Get(const char* key);
      
      /**
      * @brief Get a value from the table with the given key
      * @tparam[in] key (const char*)
      *            The key at which the value is located
      * @return (T)
      *           The value of the key in the table
      * @remarks The implementation of this function is in script_utils.h
      *          This is done to avoid a circular dependency
      */
      template<typename T>
      T Get(const char* key);

      /**
      * @brief Get a value from the table with the given key
      * @tparam[in] key (int)
      *            The key at which the value is located
      * @return (T)
      *           The value of the key in the table
      * @remarks The implementation of this function is in script_utils.h
      *          This is done to avoid a circular dependency
      * @remarks Counts from 0, not 1 like Lua
      */
      template<typename T>
      T Get(int key);


      /**
      * @brief Check if the table contains a given key
      * @param[in] key (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The key
      * @return (bool)
      *           Does it contain the value
      */
      bool Contains(foundation::SharedPointer<ScriptableValue> key);

      /**
      * @brief Check if the table contains a given key
      * @param[in] key (const char*)
      *            The key
      * @return (bool)
      *           Does it contain the value
      */
      bool Contains(const char* key);
      
      /**
      * @brief Check if the table contains a given key
      * @param[in] key (int)
      *            The key
      * @return (bool)
      *           Does it contain the value
      */
      bool Contains(int key);

      /**
      * @brief Insert a key value pair into the table
      * @param[in] key (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *            The key
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>)
      *           The value
      */
      void Insert(foundation::SharedPointer<ScriptableValue> key, foundation::SharedPointer<ScriptableValue> value);
      /**
      * @brief Insert a key value pair into the table
      * @tparam[in] key (const char*)
      *            The key
      * @tparam[in] value (Value)
      *           The value
      * @remarks The implementation of this function is in script_utils.h
      *          This is done to avoid a circular dependency
      */
      template <typename Value>
      void Insert(const char* key, Value value);

      /**
      * @brief Insert a key value pair into the table
      * @tparam[in] key (int)
      *            The key
      * @tparam[in] value (Value)
      *           The value
      * @remarks The implementation of this function is in script_utils.h
      *          This is done to avoid a circular dependency
      *          Counts from 0, not 1 like Lua
      */
      template <typename Value>
      void Insert(int key, Value value);

      /**
      * @brief Get the amount of elements in the table
      * @return (int) The amount of elements in the table
      */
      int GetSize();

    protected:
      /**
      * @brief Create a ScriptableObject
      * @param[in] lua_state() (lua_State*) the Lua state to create the ScriptableObject in
      */
      ScriptableTable(ScriptState* script_state, bool is_lib = false);
    };
  }
}