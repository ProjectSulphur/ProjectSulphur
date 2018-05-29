#pragma once 
#include "engine/networking/editor/editor_listener.h"
#include "engine/application/frame_scheduler.h"

#include <foundation/job/resource.h>
#include <foundation/job/thread_pool.h>
#include <foundation/job/job_graph.h>
#include <functional>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      class ConnectionManager;
      class EditorMessagingSystem;
    }

    /*
    *@class sulphur::engine::Application::ApplicationState : sulphur::engine::IMessageListener
    *@brief class that manages the application state.
    *@author Stan Pepels
    */
    class ApplicationState : public editor::IEditorListener
    {
    public:
      ApplicationState();

      /**
      *@brief constructor.
      *@param[in] system (sulphur::engine::MessagingSystem*) Messaging system to subscribe to for recieving messages.
      */
      void Initialize(editor::EditorMessagingSystem* messaging_system);

      /**
      *@brief Enum detailing the different states of the engine when it's connected with the editor
      */
      enum struct State
      {
        kEdit, //!< state when editing the scene. The callback for this state should diable all gameplay related systems.
        kSimulate, //!< state when simulating the game. The callback for this is state should enable all systems in the engine used for playing the game.
        kRewind, //!< state when using the engine rewind feature. The callback for this state should enble the engine rewind feature.
        kNumStates
      };

      /*
      *@brief Set a callback for an application state
      *@param[in] state (const sulphur::engine::Application::ApplicationState::State&) State to set the callback for.
      *@param[in] callback (eastl::function<void>) Callback to use when in the state passed with this callback.
      */
      void SetCallback(const State& state, std::function<void(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler)> callback);

      /**
      *@see sulphur::engine::IMessageListener
      */
      void OnReceive(editor::EditorMessageID id, const MessagePayload& payload) override;

      /**
      *@brief Runs the callback for the current state
      */
      void Update(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler);
    private:
      State state_; //!< state we're currently in
      editor::EditorMessagingSystem* message_system_;
      std::function<void(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler)> update_functions_[static_cast<size_t>(State::kNumStates)];
    };
  }
}