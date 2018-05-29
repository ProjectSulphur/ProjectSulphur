#pragma once
#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

namespace sulphur 
{
  namespace engine 
  {
    class ScriptState;
    /**
    * @brief An enumerator for all the types that exist in Lua
    */
    enum struct ScriptableValueType
    {
      kNil = 0,
      kBoolean = 1,
      kLightUserData = 2,
      kNumber = 3,
      kString = 4,
      kObject = 5,
      kFunction = 6,
      kUserdata = 7,
      kThread = 8,
      kVector = 9,
      kMatrix = 10,
      kQuaternion = 11
    };

    /**
    * @class sulphur::engine::ScriptableValue
    * @brief Used to hold a reference to a scriptable value
    * @author Rodi Lankester
    */
    class ScriptableValue
    {
      friend class sulphur::engine::ScriptState;
      friend class sulphur::foundation::Memory;
    public:
      /**
      * @brief Destructor for ScriptableValue
      */
      virtual ~ScriptableValue();  

      /**
      * @brief Push the scriptable value to the top of the lua stack
      */
      virtual void Push();

      /**
      * @brief Get the type of the value from Lua
      * @return (sulphur::engine::ScriptableValueType) The type of the ScriptableValue
      */
      ScriptableValueType GetType();

      /**
      * @brief Get the script state the object was created in
      * @return (sulphur::engine::ScriptState) The script staate
      */
      ScriptState* script_state();

      /**
      * @brief Get the reference of the position of the scriptable value in the scripting state
      * @returns (int) The reference
      */
      int ref()
      {
        return ref_;
      }

      /**
      * @brief Get the key of the scriptable value
      * @returns (const char*) key
      */
      const char* key()
      {
        return key_;
      }

      /**
      * @brief Get if the scriptable value is a global
      * @returns (bool) Is this value a global
      */
      const bool global()
      {
        return global_;
      }
    protected:
      /**
      * @brief Construct a scriptable value from the Lua stack
      * @param[in] lua_state() (lua_State*) the Lua state to create the value in
      * @param[in] key (const char*) the name of the global of which create the value from
      * @param[in] is_lib (bool) Does the scriptable value belong to a library 
      */
      ScriptableValue(ScriptState* script_state, const char* key, bool is_lib = false);
      /**
      * @brief Construct a scriptable value from the Lua stack
      * @param[in] lua_state() (lua_State*) the Lua state to create the value in
      * @param[in] idx (int) the stack index to create the value from
      */
      ScriptableValue(ScriptState* script_state, int idx, bool is_lib = false);
      /**
      * @brief Construct an empty scriptable value (ScriptableValueType::kNil)
      * @param[in] lua_state() (lua_State*) the Lua state to create the value in
      */
      ScriptableValue(ScriptState* script_state, bool is_lib = false);

      
      ScriptState* script_state_; //!< The script state this ScriptableValue was created in

      int ref_; //!< The reference to the position of the ScriptableValue in the Lua registry
      const char * key_; //!< The key to the global variable in lua if this value is a global
      bool  global_; //!< Is this ScriptableValue a global 
      bool is_lib_; //!< Does this value belong to a library
    };

  }
}