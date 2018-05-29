#include "application.h"

#include "engine/application/frame_scheduler.h"
#include "engine/assets/asset_system.h"
#include "engine/physics/physics_system.h"
#include "engine/scripting/script_system.h"
#include "engine/networking/network_system.h"
#include "engine/rewinder/rewind_system.h"
#include "engine/audio/audio_system.h"
#include "engine/application/hooks.h"

#include "engine/networking/messaging_system.h"
#include <physics/platform_physics.h>

#include <graphics/platform/renderer_loader.h>

#include <foundation/job/thread_pool.h>
#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>
#include <foundation/utils/timer.h>
#include <functional>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Application::Application() :
      BaseResource("Application"),
      platform_(nullptr),
      renderer_("renderer", nullptr),
      physics_("platform_physics", nullptr)
    {
      foundation::Memory::Initialize(2ul * 1024ul * 1024ul * 1024ul);
      editor_hook_ = foundation::Memory::Construct<EditorHook>();

      platform_ = editor_hook_->ConstructPlatform(renderer_);
      if (platform_ == nullptr)
      {
        platform_ = foundation::Memory::ConstructUnique<Platform>(*renderer_, nullptr);
      }

      // Create external systems
      foundation::Vector<graphics::RendererType> supported_renderers_ =
        graphics::RendererLoader::GetSupportedRenderers();
      PS_LOG_IF(supported_renderers_.empty(), Fatal,
        "This device is not supported or renderers could not be build.");

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
      services_.Create<AssetSystem>();
      services_.Create<PhysicsSystem>();
      services_.Create<NetworkSystem>();
      services_.Create<AudioSystem>();
      // @note (Maarten) This service should probably always be the last that is initialized so 
      //                 accidental references to other services from within the world are valid
      services_.Create<WorldProviderSystem>();
      // @note (Raymi) This one depends on the systems in the world to work. So it needs to be after the world
      services_.Create<RewindSystem>();
      // @note (Daniel) Makes sure all systems before it are initialized, so that scripting has everything accessible
      services_.Create<ScriptSystem>();

      editor_hook_->AddEditorServices(services_);
    }

    //------------------------------------------------------------------------------------------------------
    void Application::OnInitialize(foundation::JobGraph&)
    {
      OnInitialize();
    }

    //--------------------------------------------------------------------------
    void Application::Run(int argc, char** argv)
    {
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
      foundation::Job fixed_update_job = foundation::make_job("fixed_update", "", []() {});
      job_graph.Add(std::move(fixed_update_job));

      foundation::Job update_job = foundation::make_job("update", "", []() {});
      update_job.set_blocker("fixed_update");
      job_graph.Add(std::move(update_job));

      foundation::Job restore_rewind_job = foundation::make_job("restore_rewind", "", []() {});
      job_graph.Add(std::move(restore_rewind_job));

      foundation::Resource<RewindSystem*> rewinder = foundation::Resource<RewindSystem*>(
        "Rewinder",
        (&services_.Get<RewindSystem>())
        );

      foundation::Job render_job = foundation::make_job("render", "", []() {});
      render_job.set_blocker("update");
      job_graph.Add(std::move(render_job));

      const auto render_update = [](foundation::SharedPointer<IRenderer> renderer)
      {
        renderer->OnUpdate();
      };
      const auto renderer_startframe = [](foundation::SharedPointer<IRenderer>& renderer)
      {
        renderer->StartFrame();
      };
      const auto renderer_endframe = [](foundation::SharedPointer<IRenderer>& renderer)
      {
        renderer->EndFrame();
      };

      foundation::Job render_update_job = make_job("render_update", "update",
        render_update, bind_write(renderer_));
      job_graph.Add(std::move(render_update_job));

      foundation::Job renderer_startframe_job = make_job("renderer_startframe", "render",
        renderer_startframe, bind_write(renderer_));
      job_graph.Add(std::move(renderer_startframe_job));

      foundation::Job renderer_endframe_job = make_job("renderer_endframe", "render",
        renderer_endframe, bind_write(renderer_));
      renderer_endframe_job.set_blocker("camerasystem_copy_to_screen");
      job_graph.Add(std::move(renderer_endframe_job));

      // Initialization
      // 0. Save the project directory

      if (argc > 1 && argv != nullptr)
      {
        project_directory_ = argv[1];
      }
      else
      {
        project_directory_ = "";
      }

      // 1. Initialize the renderer
      const Window& window = platform_->window();
      (*renderer_.Get())->OnInitialize(window.GetNativeWindowHandle(), window.GetSize());
      PS_LOG(Debug, "Renderer intiliazation successful!");

      // 2. Initialize physics
      (*physics_.Get())->Initialize();
      PS_LOG(Debug, "Physics intiliazation successful!");

      // 3. Initialize internal systems
      services_.Execute(&IServiceSystemBase::OnInitialize, *this, job_graph);
      PS_LOG(Debug, "Services initialized successfully!");


      // 4. Initialize the application
      OnInitialize(job_graph);

      // 5. Intialize the editor hook
      editor_hook_->Intialize(*this, job_graph);

      // @note (Daniel) Makes sure every single thing is initialized before starting the scripting environment
      // @note (Stan) When the editor is hooked in the application state is initialized in edit mode. 
      //              Thus scripting is not required to be started yet as it will not run in this state.
      if (editor_hook_->Connected() == false)
      {
        services_.Get<ScriptSystem>().Start(project_directory_.path());
      }

      // Validate the job graph
      PS_LOG_IF(!job_graph.Validate(), Error, "Data contention detected");

      // Setup the correct engine logic update. If editor is hooked in the update uses a state machine to swap between edit, simulate and rewind update
      // If the editor is not hooked in the regular update logic is used to run the engine. 
      std::function<void(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler)> update;

      if (editor_hook_->Connected() == true)
      {
        update = [this](foundation::ThreadPool& thread_pool,
          foundation::JobGraphExt& job_graph,
          foundation::Resource < FrameScheduler>& frame_scheduler)
        {
          editor_hook_->Update(thread_pool, job_graph, frame_scheduler);
        };
      }
      else
      {
        update = [rewinder](foundation::ThreadPool& thread_pool,
          foundation::JobGraphExt& job_graph,
          foundation::Resource<FrameScheduler>& frame_scheduler)
        {
          if ((*rewinder)->IsRewinding() == false)
          {
            while (frame_scheduler->TryStartFixed() == true)
            {
              job_graph.SubmitSubTreeToPool("fixed_update", thread_pool);
              thread_pool.RunAllTasks();
            }

            job_graph.SubmitSubTreeToPool("update", thread_pool);
            thread_pool.RunAllTasks();
            if ((*rewinder)->active() == true)
            {
              job_graph.SubmitSubTreeToPool("store_rewind", thread_pool);
              thread_pool.RunAllTasks();
            }
          }
          else if ((*rewinder)->IsRewinding())
          {
            job_graph.SubmitSubTreeToPool("restore_rewind", thread_pool);
            thread_pool.RunAllTasks();
          }
        };
      }

      while (platform_->ShouldExit() == false)
      {
        // If we can not start a frame right now we will buzy wait until the next
        if (update_scheduler->TryStartFrame(timer.GetDeltaSeconds()) == false)
        {
          continue;
        }
        services_.Get<MessagingSystem>().ReceiveMessages();
        editor_hook_->RecieveMessages();

        update(thread_pool, job_graph, update_scheduler);

        job_graph.SubmitSubTreeToPool("render", thread_pool);
        thread_pool.RunAllTasks();

        platform_->ProcessEvents();
        editor_hook_->SendMessages();
      }

      // Termination
      // 1. Shutdown application
      OnTerminate();

      // 2. release all editor listeners
      editor_hook_->Release();

      // 3. Notify local services
      services_.Execute(&IServiceSystemBase::OnTerminate);

      // 4. Shutdown physics
      (*physics_.Get())->Destroy();

      // 5. Shutdown renderer
      (*renderer_.Get())->OnDestroy();

      // 6. Shutdown local services
      services_.Execute(&IServiceSystemBase::OnShutdown);

      // 7. Shutdown platform-layer
      platform_ = nullptr;
    }

    //------------------------------------------------------------------------------------------------------
    IRenderer& Application::platform_renderer()
    {
      return *(*renderer_.Get()).get();
    }

    //------------------------------------------------------------------------------------------------------
    const IRenderer& Application::platform_renderer() const
    {
      return *(*renderer_.Get()).get();
    }

    //------------------------------------------------------------------------------------------------------
    physics::PlatformPhysics& Application::platform_physics()
    {
      return *(*physics_.Get()).get();
    }

    //------------------------------------------------------------------------------------------------------
    const physics::PlatformPhysics& Application::platform_physics() const
    {
      return *(*physics_.Get()).get();
    }

    //------------------------------------------------------------------------------------------------------
    Platform& Application::platform()
    {
      return *(platform_);
    }

    //------------------------------------------------------------------------------------------------------
    const Platform& Application::platform() const
    {
      return *(platform_);
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Path& Application::project_directory() const
    {
      return project_directory_;
    }

    //------------------------------------------------------------------------------------------------------
    IEditorHook* Application::editor_hook()
    {
      return editor_hook_;
    }

    //------------------------------------------------------------------------------------------------------
    void Application::SetProjectDirectory(foundation::Path& directory_path)
    {
      // Todo: unload all world resources
      // Todo: load the initial world.


      ScriptSystem& script_system = GetService<ScriptSystem>();
      script_system.OnTerminate();
      script_system.InitializeScriptState(*this);
      script_system.RegisterClasses(*this);
      script_system.LoadMain(*this);

      if (editor_hook_->Connected() == false)
      {
        script_system.Start(project_directory_.path());
      }
    }
  }
}
