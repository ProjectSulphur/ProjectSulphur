#include "application_state.h"
#include "engine\networking\editor\editor_messaging_system.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    ApplicationState::ApplicationState() :
      state_(State::kEdit),
      message_system_(nullptr)
    {
    }

    //------------------------------------------------------------------------------------------------------
    void ApplicationState::Initialize(editor::EditorMessagingSystem* messaging_system)
    {
      message_system_ = messaging_system;
      message_system_->Subscribe(this, editor::EditorMessageID::kStartedPlaying);
      message_system_->Subscribe(this, editor::EditorMessageID::kStoppedPlaying);
      message_system_->Subscribe(this, editor::EditorMessageID::kStartRewinding);
      message_system_->Subscribe(this, editor::EditorMessageID::kStopRewinding);
    }

    //------------------------------------------------------------------------------------------------------
    void ApplicationState::SetCallback(const State& state, std::function<void(foundation::ThreadPool& thread_pool,
      foundation::JobGraphExt& job_graph,
      foundation::Resource<FrameScheduler>& frame_scheduler)> callback)
    {
      update_functions_[static_cast<size_t>(state)] = callback;
    }

    //------------------------------------------------------------------------------------------------------
    void ApplicationState::OnReceive(editor::EditorMessageID id, const MessagePayload&)
    {
      switch (id)
      {
      case editor::EditorMessageID::kStartedPlaying:
        state_ = State::kSimulate;
        break;
      case editor::EditorMessageID::kStoppedPlaying:
        state_ = State::kEdit;
        break;
      case editor::EditorMessageID::kStartRewinding:
        state_ = State::kRewind;
        break;
      case editor::EditorMessageID::kStopRewinding:
        state_ = State::kEdit;
        break;
      default:
        break;
      }
    }

    //------------------------------------------------------------------------------------------------------
    void ApplicationState::Update(foundation::ThreadPool& thread_pool,
      foundation::JobGraphExt& job_graph,
      foundation::Resource<FrameScheduler>& frame_scheduler)
    {
      update_functions_[static_cast<size_t>(state_)](thread_pool, job_graph, frame_scheduler);
    }
  }
}