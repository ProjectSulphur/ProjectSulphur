#pragma once
#include "engine/systems/service_system.h"

#include "engine/scripting/script_state.h"
#include "engine/scripting/script_register.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::ScriptSystem : sulphur::engine::IServiceSystem <sulpur::engine::ScriptSystem>
    * @brief A service that acts as a manager for the script state
    * @author Maarten ten Velden
    */
    class ScriptSystem : public IServiceSystem<ScriptSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      ScriptSystem();

      /**
      * @see sulphur::engine::IServiceSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @brief Initializes the script state. This function can be used reintitialze the script state when the application is allready running.
      * @param[in] app (sulphur::engine::Application) Current running application instance.
      */
      void InitializeScriptState(Application& app);


      /**
      * @see sulphur::engine::IServiceSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Registers all exposed classes to the scripting environment 
      * @param[in] app (sulphur::engine::Application&) The application this system was created from
      * @param[in] job_graph (sulphur::foundation::JobGraph&) The job graph of the application
      * @remarks Static interfaces are also initialized here (e.g. sulphur::engine::ScriptableWorld)
      */
      void RegisterClasses(Application& app);

      /**
      * @brief Loads "main.lua" from the current project directory
      * @param[in] app (sulphur::engine::Application&) The application this system was created from
      */
      void LoadMain(Application& app);

      /**
      * @brief "Starts" the scripting application
      * @param[in] project_dir (const sulphur::foundation::String&) The current project directory
      */
      void Start(const foundation::String& project_dir);

      /**
      * @brief Get a reference to the internal script state
      * @return (sulphur::engine::ScriptState) The active script state
      */
      ScriptState* script_state()
      {
        return &script_state_;
      }

    private:
      ScriptState script_state_; //!< The script state corresponding to this ScriptSystem
     
      /**
      * @brief The register used to register all exposed classes to the scripting environment
      */
      ScriptRegister register_;

      foundation::Resource<ScriptState*> script_resource_; //!< The resource for the system to use
    };
  }
}