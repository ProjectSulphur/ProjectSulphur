#include "application.h"

#include "engine/application/window.h"
#include "engine/application/frame_scheduler.h"
#include "engine/networking/messaging_system.h"

#include "engine/assets/asset_system.h"
#include "engine/scripting/script_system.h"
#include "engine/physics/physics_system.h"
#ifdef PS_EDITOR
#include "engine/networking/messages.h"
#endif

#include <graphics/platform/renderer_loader.h>

#include <foundation/job/thread_pool.h>
#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>
#include <foundation/utils/timer.h>
#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Application::Application() :
      BaseResource("Application"),
      platform_(nullptr),
      renderer_("Renderer", nullptr),
      physics_("Physics", nullptr)
    {
      foundation::Memory::Initialize(2ul * 1024ul * 1024ul * 1024ul);

      foundation::Vector<graphics::RendererType> supported_renderers_ =
        graphics::RendererLoader::GetSupportedRenderers();

      assert(!supported_renderers_.empty() && "This device is not supported or renderers could not be build.");

      // Create external systems
      renderer_ = foundation::SharedPointerResource<IRenderer>(
        "Renderer",
        graphics::RendererLoader::CreateRenderer(supported_renderers_[0])
        );
      physics_ = foundation::SharedPointerResource<physics::PlatformPhysics>(
        "Physics",
        foundation::Memory::ConstructShared<physics::PlatformPhysics>()
        );

      // Create internal systems
      services_.Create<MessagingSystem>();
      services_.Create<PhysicsSystem>();
      services_.Create<ScriptSystem>();
      // @note (Maarten) This service should probably always be the last that is initialized so 
      //                 accidental references to other services from within the world are valid
      services_.Create<WorldProviderSystem>();
    }

    void Application::OnInitialize(foundation::JobGraph&)
    {
      OnInitialize();
    }

    //--------------------------------------------------------------------------
    void Application::Run()
    {
      // Initialize platform layer
      while (platform_ == nullptr)
      {
#if PS_EDITOR
        // Retrieve window handle from server (editor)
        MessageID id;
        MessagePayload payload;
        if (services_.Get<MessagingSystem>().connection_.RetrieveNextMessage(&id, &payload) == true)
        {
          if (id == MessageID::kWindowHandle)
          {
            const WindowHandlePayload& actual_payload = payload.AsFormat<WindowHandlePayload>();

            PS_LOG(Debug, "Window handle (%x) received from peer", actual_payload.handle);

            platform_ = foundation::Memory::ConstructUnique<Platform>(reinterpret_cast<void*>(actual_payload.handle));
          }
        }
#else
        platform_ = foundation::Memory::ConstructUnique<Platform>(nullptr);
#endif
      }

      // Initialize job-system
      foundation::ThreadPool thread_pool;
      foundation::JobGraphExt job_graph;

      foundation::Timer timer = foundation::Timer();
      timer.Start();

      foundation::Resource<FrameScheduler> update_scheduler(
        "UpdateScheduler",
        FrameScheduler(50)
      );

      // Create jobs to group the different phases
      job_graph.Add(foundation::make_job("fixed_update", "", []() {}));
      job_graph.Add(foundation::make_job("update", "", []() {}));
      job_graph.Add(foundation::make_job("render", "", []() {}));

      // Create a set of tasks to perform the old updates
      const auto legacy_fixed_update = [](SystemSet<IServiceSystemBase>& services)
      {
        services.Execute(&ISystemBase::OnFixedUpdate);
      };

      job_graph.Add(make_job(
        "legacy_fixed_update",
        "fixed_update",
        legacy_fixed_update,
        bind_write(services_)
      ));

      const auto render_update  = [](foundation::SharedPointer<IRenderer> renderer) { renderer->OnUpdate(); };
      const auto service_update = [](SystemSet<IServiceSystemBase>& services) { services.Execute(&ISystemBase::OnUpdate, foundation::Frame::delta_time());};

      auto render_update_job = make_job("render_update", "update", render_update, bind_write(renderer_));
      job_graph.Add(std::move(render_update_job));

      auto service_update_job = make_job("service_update", "update", service_update, bind_write(services_));
      service_update_job.set_blocker("render_update");
      job_graph.Add(std::move(service_update_job));

      const auto services_onprerender  = [](SystemSet<IServiceSystemBase>& services) { services.Execute(&ISystemBase::OnPreRender);};
      const auto renderer_startframe   = [](foundation::SharedPointer<IRenderer>& renderer) {renderer->StartFrame();};
      const auto services_onrender     = [](SystemSet<IServiceSystemBase>& services) {services.Execute(&ISystemBase::OnRender);};
      const auto renderer_endframe     = [](foundation::SharedPointer<IRenderer>& renderer) {renderer->EndFrame();};
      const auto services_onpostrender = [](SystemSet<IServiceSystemBase>& services) {services.Execute(&ISystemBase::OnPostRender);};

      auto services_onprerender_job = make_job("services_onprerender", "render", services_onprerender, bind_write(services_));
      job_graph.Add(std::move(services_onprerender_job));

      auto renderer_startframe_job = make_job("renderer_startframe", "render", renderer_startframe, bind_write(renderer_));
      renderer_startframe_job.set_blocker("services_onprerender");
      job_graph.Add(std::move(renderer_startframe_job));

      auto services_onrender_job = make_job("services_onrender", "render", services_onrender, bind_write(services_));
      services_onrender_job.set_blocker("renderer_startframe");
      job_graph.Add(std::move(services_onrender_job));

      auto renderer_endframe_job = make_job("renderer_endframe", "render", renderer_endframe, bind_write(renderer_));
      renderer_endframe_job.set_blocker("services_onrender");
      job_graph.Add(std::move(renderer_endframe_job));

      auto services_onpostrender_job = make_job("services_onpostrender", "render", services_onpostrender, bind_write(services_));
      services_onpostrender_job.set_blocker("renderer_endframe");
      job_graph.Add(std::move(services_onpostrender_job));
            
      // Initialization
      // 1. Initialize the renderer
      const Window& window = platform_->window();
      (*renderer_)->OnInitialize(window.GetWindowHandle(), window.size());
      PS_LOG(Debug, "Renderer intiliazation successful!");

      // 2. Initialize physics
      physics_->get()->Initialize();
      PS_LOG(Debug, "Physics intiliazation successful!");

      // 3. Initialize internal systems
      services_.Execute(&ISystemBase::OnInitialize, *this, job_graph);
      PS_LOG(Debug, "Services initialized successfully!");

      // 4. Initialize the application
      OnInitialize(job_graph);

      // Validate the job graph
      PS_LOG_IF(!job_graph.Validate(), Error, "Data contention detected");

      while (platform_->ShouldExit() == false)
      {
        // If we can not start a frame right now we will buzy wait until the next
        if (update_scheduler->TryStartFrame(timer.GetDeltaSeconds()) == false)
        {
          continue;
        }

        while (update_scheduler->TryStartFixed() == true)
        {
          job_graph.SubmitSubTreeToPool("fixed_update", thread_pool);
          thread_pool.RunAllTasks();
        }

        services_.Get<MessagingSystem>().ReceiveMessages();

        job_graph.SubmitSubTreeToPool("update", thread_pool);
        thread_pool.RunAllTasks();

        job_graph.SubmitSubTreeToPool("render", thread_pool);
        thread_pool.RunAllTasks();

        platform_->ProcessEvents();

        services_.Get<MessagingSystem>().DispatchMessages();
      }

      // Termination
      // 1. Shutdown application
      OnTerminate();

      // 2. Shutdown local services
      services_.Execute(&ISystemBase::OnTerminate);

      // 3. Shutdown physics
      (*physics_)->Destroy();

      // 4. Shutdown asset system and release GPU resources
      AssetSystem::ReleaseGPUHandles();
      
      // 5. Shutdown renderer
      (*renderer_)->OnDestroy();

      // 6. Shutdown AssetSystem
      AssetSystem::Shutdown();

      // 7. Shutdown platform-layer
      platform_ = nullptr;
    }
  }
}
