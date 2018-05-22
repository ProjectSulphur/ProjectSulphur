#pragma once
#include "engine/systems/service_system.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/map.h>
#include <foundation/containers/string.h>

#include <lua.hpp>

namespace sulphur 
{
  namespace engine 
  {
    class Application;
    class ScriptableValue;
    class ScriptableCallback;

    /**
    * @class sulphur::engine::ScriptSystem
    * @brief Handles the state of the scripting environment
    * @author Rodi Lankester
    */
    class ScriptSystem : public IServiceSystem<ScriptSystem>
    {
    public:
      /**
      * @brief Default constructor.
      */
      ScriptSystem();

      /**
      * @see IServiceSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @see IServiceSystem::OnTerminate
      */
      void OnTerminate() override;
      /**
      * @see IServiceSystem::OnUpdate
      */
      void OnUpdate(float delta_time) override;
      /**
      * @see IServiceSystem::OnFixedUpdate
      */
      void OnFixedUpdate() override;

      /**
      * @brief Registers all callbacks in the loaded scripts
      */
      void RegisterCallbacks();

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
      * @param[in] value (foundation::SharedPointer <ScriptableValue>) The ScriptableValue to register
      */
      void RegisterValue(const char* name, foundation::SharedPointer<ScriptableValue> value);
      /**
      * @brief Get a ScriptableValue from the global namespace of the scripting environment
      * @param[in] name (cont char*) The name of the ScriptableValue in the global namespace
      * @return (foundation::SharedPointer <ScriptableValue>) The scriptable value
      */
      foundation::SharedPointer<ScriptableValue> GetValue(const char* name);

      /**
      * @brief Get a ScriptableValue from the stack
      * @param[in] idx (int) The index of the ScriptableValue on the stack
      * @return (foundation::SharedPointer <ScriptableValue>) The scriptable value
      */
      foundation::SharedPointer<ScriptableValue> GetFromStack(int idx);

      /**
      * @brief Get the lua state
      * @return (lua_State*) The lua state
      */
      lua_State* lua_state()
      {
        return lua_state_;
      }
      /**
      * @brief Get the application that this system was initialized with
      * @return (Application&) The current application
      */
      Application& application()
      {
        return *application_;
      }

    protected:
      foundation::Map<
        foundation::String,
        foundation::SharedPointer<ScriptableValue>
      > globals_; //!< list of globals in Lua

      lua_State* lua_state_; //!< The Lua state
      Application* application_;

      foundation::SharedPointer<ScriptableCallback> update_; //!< temporary update from Lua global 
      foundation::SharedPointer<ScriptableCallback> fixed_update_; //!< temporary update from Lua global 
    };
  }
}