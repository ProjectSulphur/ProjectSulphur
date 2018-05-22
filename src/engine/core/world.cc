#include "world.h"

#include "engine/core/entity_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/light_system.h"
#include "engine/systems/components/debug_render_system.h"
#include "engine/systems/components/mesh_render_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/systems/components/collider_system.h"
#include "engine/systems/components/canvas_system.h"
#include "engine/networking/messaging_system.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    WorldProviderSystem::WorldProviderSystem() :
      IServiceSystem("WorldProviderSystem"),
      current_world_(nullptr)
    {
    }

    //--------------------------------------------------------------------------
    WorldProviderSystem::~WorldProviderSystem()
    {
      OnTerminate();
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::CreateEmptyWorld(Application& app, foundation::JobGraph& job_graph)
    {
      OnTerminate();
      current_world_ = foundation::Memory::Construct<World>();
      current_world_->Initialize(app, job_graph);
    }

    //--------------------------------------------------------------------------
    World& WorldProviderSystem::GetWorld()
    {
      assert(current_world_ != nullptr && "No world currently exists");
      return *current_world_;
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      // @note (Maarten) at the moment this service system is initialized after every other
      //                 service so that systems in this world can use those services properly
      //                 If anyone decides to change that they should manually create a new world
      CreateEmptyWorld(app, job_graph);
    }
    
    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnTerminate()
    {
      if (current_world_ != nullptr)
      {
        foundation::Memory::Destruct(current_world_);
        current_world_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnFixedUpdate()
    {
      GetWorld().FixedUpdate();
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnUpdate(float delta_time)
    {
      GetWorld().Update(delta_time);
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnPreRender()
    {
      GetWorld().PreRender();
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnRender()
    {
      GetWorld().Render();
    }

    //--------------------------------------------------------------------------
    void WorldProviderSystem::OnPostRender()
    {
      GetWorld().PostRender();
    }

    //--------------------------------------------------------------------------
    World::World() :
      BaseResource("World")
    {
      DebugRenderSystem::SetupDebugAssets();

      // Entity-system
      systems_.Create<EntitySystem>();

      // Components
      systems_.Create<TransformSystem>();
      systems_.Create<CameraSystem>();
      systems_.Create<LightSystem>();
      systems_.Create<DebugRenderSystem>();
      systems_.Create<MeshRenderSystem>();
      systems_.Create<RigidBodySystem>();
      systems_.Create<BoxColliderSystem>();
      systems_.Create<SphereColliderSystem>();
      systems_.Create<CapsuleColliderSystem>();
      systems_.Create<CanvasSystem>();
    }

    //--------------------------------------------------------------------------
    World::~World()
    {
      Terminate();
    }

    //--------------------------------------------------------------------------
    void World::Initialize(Application& app, foundation::JobGraph& job_graph)
    {
      systems_.Execute(&ISystemBase::OnInitialize, app, job_graph);
    }

    //--------------------------------------------------------------------------
    void World::Terminate()
    {
      systems_.Execute(&ISystemBase::OnTerminate);
    }

    //--------------------------------------------------------------------------
    void World::FixedUpdate()
    {
      systems_.Execute(&ISystemBase::OnFixedUpdate);
    }

    //--------------------------------------------------------------------------
    void World::Update(float delta_time)
    {
      systems_.Execute(&ISystemBase::OnUpdate, delta_time);
    }

    //--------------------------------------------------------------------------
    void World::PreRender()
    {
      systems_.Execute(&ISystemBase::OnPreRender);
    }

    //--------------------------------------------------------------------------
    void World::Render()
    {
      systems_.Execute(&ISystemBase::OnRender);
    }

    //--------------------------------------------------------------------------
    void World::PostRender()
    {
      systems_.Execute(&ISystemBase::OnPostRender);
    }
  }
}