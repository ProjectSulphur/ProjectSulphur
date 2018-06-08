#include "world.h"

#include "engine/core/entity_system.h"
#include "engine/graphics/debug_render_system.h"

#include "engine/systems/components/mesh_render_system.h"
#include "engine/systems/components/skinned_mesh_render_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/light_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/systems/components/collider_system.h"
#include "engine/systems/components/canvas_system.h"
#include "engine/systems/components/network_component_system.h"
#include "engine/systems/components/constraint_system.h"
#include "engine/systems/components/audio_listener_system.h"
#include "engine/systems/components/audio_event_system.h"
#include "engine/systems/components/audio_snapshot_system.h"
#include "engine/systems/components/audio_vca_system.h"
#include "engine/systems/components/audio_bus_system.h"

#include "engine/assets/asset_system.h"

#include <lua-classes/world.lua.cc>


namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ScriptableWorld::ScriptableWorld()
    {

    }

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
    World::World() :
      BaseResource("World")
    {
      DebugRenderSystem::SetupDebugAssets();

      // Components
      components_.Create<TransformSystem>();
      components_.Create<CameraSystem>();
      components_.Create<LightSystem>();
      components_.Create<MeshRenderSystem>();
      components_.Create<SkinnedMeshRenderSystem>();
      components_.Create<RigidBodySystem>();
      components_.Create<ColliderSystem>();
      components_.Create<CanvasSystem>();
      components_.Create<NetworkComponentSystem>();
      components_.Create<ConstraintSystem>();
      components_.Create<AudioEventSystem>();
      components_.Create<AudioListenerSystem>();
      components_.Create<AudioSnapshotSystem>();
      components_.Create<AudioVCASystem>();
      components_.Create<AudioBusSystem>();
      // Self-owned systems
      owners_.Create<EntitySystem>();
      owners_.Create<DebugRenderSystem>();
    }

    //--------------------------------------------------------------------------
    World::~World()
    {
      Terminate();
    }

    //--------------------------------------------------------------------------
    void World::Initialize(Application& app, foundation::JobGraph& job_graph)
    {
      components_.Execute(&IComponentSystem::OnInitialize, app, job_graph);
      owners_.Execute(&IOwnerSystemBase::OnInitialize, app, job_graph);
    }

    //--------------------------------------------------------------------------
    void World::Terminate()
    {
      components_.Execute(&IComponentSystem::OnTerminate);
      owners_.Execute(&IOwnerSystemBase::OnTerminate);
    }

    //--------------------------------------------------------------------------
    WorldProviderSystem* ScriptableWorld::system_ = nullptr;
    Application* ScriptableWorld::app_ = nullptr;

    //--------------------------------------------------------------------------
    ScriptableWorld::ScriptableWorld(World* world) :
      world_(world)
    {
    }

    //--------------------------------------------------------------------------
    void ScriptableWorld::Initialize(
      WorldProviderSystem* system, 
      Application* app)
    {
      system_ = system;
      app_ = app;
    }

    //--------------------------------------------------------------------------
    bool ScriptableWorld::IsValid()
    {
      bool check = (system_ != nullptr && app_ != nullptr);

      if (check == false)
      {
        PS_LOG(Error, "ScriptableWorld is not initialized for use!");
      }

      return check;
    }

    //--------------------------------------------------------------------------
    ScriptableWorld ScriptableWorld::Create()
    {
      if (IsValid() == false)
      {
        return nullptr;
      }

      World& created = system_->GetWorld();

      return ScriptableWorld(&created);
    }

    //--------------------------------------------------------------------------
    Entity ScriptableWorld::CreateEntity() const
    {
      if (world_ == nullptr)
      {
        PS_LOG(Error, 
          "Could not create an entity in a ScriptableWorld, because it is not intialized yet");

        return Entity();
      }

      Entity handle = world_->GetOwner<EntitySystem>().Create();
      return handle;
    }

    //--------------------------------------------------------------------------
    void ScriptableWorld::DestroyEntity(ScriptHandle entity) const
    {
      if (world_ == nullptr)
      {
        PS_LOG(Error,
          "Could not destroy an entity in a ScriptableWorld, because it is not intialized yet");
      }

      Entity* ent = reinterpret_cast<Entity*>(ScriptUtils::As<void*>(entity));

      world_->GetOwner<EntitySystem>().Destroy(*ent);
    }
  }
}