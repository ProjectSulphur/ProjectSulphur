#pragma once
#include "engine/systems/service_system.h"
#include "engine/networking/editor/editor_listener.h"

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }

  namespace engine
  {
    class Application;
    class WorldProviderSystem;

    namespace editor
    {
      class EditorMessagingSystem;
      class WorldDecorator;
      /**
      * @class sulphur::engine::editor::EditorWorldProvider : sulphur::engine::IServiceSystem<EditorWorldProvider>, sulphur::engine::editor::IEditorListener
      * @brief Editor world provider. this class builds upon the world provider system and is only available in the editor build of the engine.
      * It adds an editor listener interface to the main editor world provider.
      * @author Stan Pepels
      */
      class EditorWorldProvider : public IServiceSystem<EditorWorldProvider>, IEditorListener
      {
      public:
        /**
        * @brief makes sure this system is setup to recieve messages from the editor.
        * @param[in] world (sulphur::engine::WorldProviderSystem*) world provider system to use when processing incomming editor messages.
        * @param[in]`messaging_system (sulphur::engine::editor::EditorMessagingSystem*) Messaging system to register to.
        */
        EditorWorldProvider(WorldProviderSystem* world, EditorMessagingSystem* messaging_system);

        /**
        * @brief Destructor
        */
        virtual ~EditorWorldProvider() override;

        /**
        * @see sulphur::engine::editor::IEditorListener::OnReceive
        */
        void OnReceive(EditorMessageID id, const MessagePayload& payload) override;

        /**
        * @see IServiceSystem::OnInitialize
        */
        void OnInitialize(Application& app, foundation::JobGraph& job_graph) final;

        /**
        * @see IServiceSystem::OnTerminate
        */
        void OnTerminate() final;

        /**
        * @brief Creates an empty world using the current world provider system.
        * @param[in] app (sulphur::engine::Application&) Current application instance.
        * @param[in] job_graph (sulphur::foundation::JobGraph&) 
        *            Job graph used to schedule execution of jobs during the frame
        */
        void CreateEmptyWorld(Application& app, foundation::JobGraph& job_graph);

      private:
        WorldDecorator* current_world_; //!< Instance of sulphur::engine::editor::WorldDecorator that extends the current world in the world provider system with editor specific logic.
        WorldProviderSystem* world_provider_system_; //!< world provider system to use when creating worlds.
        EditorMessagingSystem* messaging_system_; //!< messaging system to use when sending messages to the editor.
      };
    }
  }
}