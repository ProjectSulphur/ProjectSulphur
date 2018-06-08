#include "editor_hook.h"
#include "engine\networking\editor\editor_message_payloads.h"
#include "engine\networking\editor\editor_messaging_system.h"

#include "engine\networking\editor\connection_manager.h"
#include "engine\core\editor\editor_world_provider.h"
#include "engine\core\editor\world_decorator.h"
#include "engine\application\frame_scheduler.h"
#include "engine\networking\editor\listeners\asset_system_listener.h"
#include "engine\networking\editor\listeners\application_listener.h"
#include "engine\assets\asset_system.h" 

#include <foundation\job\job_graph.h>
#include <foundation\job\thread_pool.h>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    foundation::UniquePointer<Platform> Editor::ConstructPlatform(foundation::Resource<foundation::SharedPointer<IRenderer>> renderer)
    {
      connection_ = foundation::Memory::ConstructShared<editor::ConnectionManager>();
      connection_->Initialize(8008);
      foundation::UniquePointer<Platform> result = nullptr;
      while (result == nullptr)
      {
        if (connection_->Connect() == false)
        {
          continue;
        }
        editor::EditorMessageID id;
        MessagePayload payload;
        if (connection_->RetrieveNextMessage(&id, &payload) == true)
        {
          if (id == editor::EditorMessageID::kWindowHandle)
          {
            const editor::WindowHandlePayload& actual_payload =
              payload.AsFormat<editor::WindowHandlePayload>();

            PS_LOG(Debug, "Window handle (%x) received from peer", actual_payload.handle);

            result = foundation::Memory::ConstructUnique<Platform>(*renderer,
              reinterpret_cast<void*>(actual_payload.handle));
          }
        }
      }
      return result;
    }


    //------------------------------------------------------------------------------------------------------
    void Editor::AddEditorServices(SystemSet<IServiceSystemBase>& services)
    {
      services.Create<editor::EditorMessagingSystem>(connection_);
      services.Create<editor::EditorWorldProvider>(
        &services.Get<WorldProviderSystem>(),
        &services.Get<editor::EditorMessagingSystem>()
        );

      messageing_system_ = &services.Get<editor::EditorMessagingSystem>();

    }

    //------------------------------------------------------------------------------------------------------
    void Editor::Release()
    {
      for (editor::IEditorListener* listener : listeners_)
      {
        foundation::Memory::Destruct(listener);
      }
      listeners_.clear();
    }

    //------------------------------------------------------------------------------------------------------
    void Editor::Update(foundation::ThreadPool& thread_pool,
      foundation::JobGraphExt& job_graph,
      foundation::Resource<FrameScheduler>& frame_scheduler)
    {
       states_.Update(thread_pool, job_graph, frame_scheduler);
    }

    //------------------------------------------------------------------------------------------------------
    bool Editor::Connected()
    {
      return true;
    }

    void Editor::Intialize(Application& app, foundation::JobGraph& job_graph)
    {
      foundation::Job editor_update_job = foundation::make_job("editor_update", "", []() {});
      job_graph.Add(std::move(editor_update_job));

      foundation::Job editor_fixed_update_job = foundation::make_job("editor_fixed_update", "", []() {});
      job_graph.Add(std::move(editor_fixed_update_job));

      listeners_.push_back(
        foundation::Memory::Construct<editor::AssetSystemListener>(
          app.GetService<AssetSystem>(),
          app.GetService<editor::EditorMessagingSystem>()
          ));

      listeners_.push_back(
        foundation::Memory::Construct<editor::ApplicationListener>(
          app,
          *messageing_system_
          ));

      states_.Initialize(messageing_system_);
      
      states_.SetCallback(
        ApplicationState::State::kSimulate,
        [](foundation::ThreadPool& thread_pool, foundation::JobGraphExt& job_graph, foundation::Resource<FrameScheduler>& frame_scheduler) {
        while (frame_scheduler->TryStartFixed() == true)
        {
          job_graph.SubmitSubTreeToPool("fixed_update", thread_pool);
          thread_pool.RunAllTasks();
        }
      
        job_graph.SubmitSubTreeToPool("update", thread_pool);
        thread_pool.RunAllTasks();
      });
      
      
      states_.SetCallback(ApplicationState::State::kEdit,
        [](foundation::ThreadPool& thread_pool, foundation::JobGraphExt& job_graph, foundation::Resource<FrameScheduler>& frame_scheduler) {
        while (frame_scheduler->TryStartFixed() == true)
        {
          job_graph.SubmitSubTreeToPool("editor_fixed_update", thread_pool);
          thread_pool.RunAllTasks();
        }
        job_graph.SubmitSubTreeToPool("editor_update", thread_pool);
        thread_pool.RunAllTasks();
      });
      
      states_.SetCallback(
        ApplicationState::State::kRewind,
        [](foundation::ThreadPool& thread_pool, foundation::JobGraphExt& job_graph, foundation::Resource<FrameScheduler>& ) {
        job_graph.SubmitSubTreeToPool("restore_rewind", thread_pool);
        thread_pool.RunAllTasks();
      });
    }

    //------------------------------------------------------------------------------------------------------
    void Editor::SendMessages()
    {
      messageing_system_->DispatchMessages();
    }

    //------------------------------------------------------------------------------------------------------
    void Editor::RecieveMessages()
    {
      messageing_system_->ReceiveMessages();
    }
  }
}