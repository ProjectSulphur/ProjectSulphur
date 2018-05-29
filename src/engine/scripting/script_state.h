#pragma once

#include <foundation/memory/memory.h>
#include <foundation/containers/map.h>
#include <foundation/containers/string.h>
#include <foundation/utils/type_set.h>

struct lua_State;

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }
  namespace engine
  {
    class ScriptableValue;
    class ScriptableCallback;
    class ScriptableArgs;
    class ComponentIDBase;
  }
}

namespace sulphur
{
  namespace engine
  {
    class Application;

    /**
    * @class sulphur::engine::ScriptState
    * @brief Handles the state of the scripting environment
    * @author Rodi Lankester
    */
    class ScriptState
    {
    public:

      /**
      * @struct sulphur::engine::ScriptState::Metatable
      * @brief Used to store metatable information per type ID
      * @author Daniel Konings
      */
      struct Metatable
      {
        size_t size; //!< The size of a registered scriptable object in its underlying C++ POD
        foundation::String name; //!< The name of the metatable the scriptable object is registerd to
      };

      /**
      * @brief Default constructor
      */
      ScriptState();

      /**
      * @brief Initializes the scripting environment
      * @param[in] app (sulphur::engine::Application*) The application this script state belongs to
      * @returns (bool) Did the ScriptState initialize properly?
      */
      bool Initialize(Application* app);

      /**
      * @brief Shuts down the scripting environment
      */
      void Shutdown();

      /**
      * @brief Registers all callbacks in the loaded scripts
      */
      void RegisterCallbacks();

      /**
      * @brief Called when the application is initialized
      * @param[in] project_dir (const foundation::String&) The project directory set from the application
      */
      void OnInitialize(const foundation::String& project_dir);

      /**
      * @brief Updates the scripting environment
      * @param[in] delta_time (float) The delta time of the previous frame
      */
      void Update(float delta_time);

      /**
      * @brief Updates the scripting environment on a fixed interval
      */
      void FixedUpdate();

      /**
      * @brief Force the scripting environment to run garbage collection
      */
      void CollectGarbage();

      /**
      * @brief Compiles and runs a script
      * @param[in] data (const char*) The script data
      * @param[in] size (size_t) The size of the script data
      * @param[in] context (const char*) The context/name of the script
      * @returns (bool) Did the script compile and run without errors
      */
      bool CompileAndRun(
        const char* data,
        size_t size,
        const char* context
      );

      /**
      * @brief Register a ScriptableValue to the global namespace of the scripting environment
      * @param[in] name (const char*) The name of the global variable
      * @param[in] value (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The ScriptableValue to register
      */
      void RegisterValue(const char* name, foundation::SharedPointer<ScriptableValue> value);
      /**
      * @brief Get a ScriptableValue from the global namespace of the scripting environment
      * @param[in] name (cont char*) The name of the ScriptableValue in the global namespace
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The scriptable value
      */
      foundation::SharedPointer<ScriptableValue> GetValue(const char* name);

      /**
      * @brief Get a ScriptableValue from the stack
      * @param[in] idx (int) The index of the ScriptableValue on the stack
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::ScriptableValue>) The scriptable value
      */
      foundation::SharedPointer<ScriptableValue> GetFromStack(int idx);

      /**
      * @brief Registeres a metatable by type ID
      * @param[in] id (int) The ID to register the metatable name of
      * @param[in] size (size_t) The size of the metatable
      * @param[in] name (sulphur::foundation::String) The name of the table to register
      */
      void RegisterMetatable(int id, size_t size, const foundation::String& name);

      void RegisterComponentID(foundation::typeid_t id, ComponentIDBase* base);

      ComponentIDBase* GetComponentIDRegister(foundation::typeid_t id);

      /**
      * @brief Retrieves a metatable by ID
      * @param[in] id (int) The id to retrieve the metatable of
      * @return (sulphur::engine::ScriptState::Metatable) The metatable
      */
      Metatable GetMetatable(int id);

      /**
      * @brief Get the lua state
      * @return (lua_State*) The lua state
      */
      lua_State* lua_state()
      {
        return lua_state_;
      }

      /**
      * @return (sulphur::engine::Application*) The application this script state belongs to
      */
      Application* app()
      {
        return app_;
      }

      ~ScriptState();

    protected:
      /**
      * @brief Include function for lua to include other lua files
      * @param[in] (ScriptableArgs*) The arguments passed from Lua
      */
      static void Include(ScriptableArgs* args);

      foundation::Map<
        foundation::String,
        foundation::SharedPointer<ScriptableValue>
      > globals_; //!< list of globals in Lua

      lua_State* lua_state_; //!< The Lua state

      foundation::SharedPointer<ScriptableCallback> on_initialize_; //!< OnInitialize global in main.lua
      foundation::SharedPointer<ScriptableCallback> update_; //!< Update global in main.lua
      foundation::SharedPointer<ScriptableCallback> fixed_update_; //!< FixedUpdate global in main.lua

      foundation::SharedPointer<ScriptableValue> include_; //!< the include function for scripting

      foundation::Map<int, Metatable> metatables_; //!< The registered meta tables
      foundation::Map<foundation::typeid_t, ComponentIDBase*> component_ids_;

      Application* app_; //!< The application this script state belongs to
    };
  }
}