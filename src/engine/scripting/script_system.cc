#include "script_system.h"

#include "engine/scripting/script_register.h"
#include "engine/core/script_debug.h"
#include "engine/systems/components/mesh_render_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/systems/components/collider_system.h"
#include "engine/systems/components/network_component_system.h"
#include "engine/systems/components/audio_event_system.h"
#include "engine/systems/components/audio_listener_system.h"
#include "engine/systems/components/constraint_system.h" 

#include "engine/audio/audio_system.h"

#include "engine/physics/physics_system.h"
#include "engine/input/input.h"
#include "engine/networking/network_system.h"
#include "engine/assets/scriptable_asset_system.h"
#include "engine/utilities/scriptable_imgui.h"

#include "engine/assets/asset_system.h"
#include "engine/assets/script.h"

#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>
#include <foundation/utils/frame.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    ScriptSystem::ScriptSystem() :
      IServiceSystem<ScriptSystem>("ScriptSystem"),
      script_state_(),
      register_(nullptr),
      script_resource_("ScriptState")
    {

    }

    //-------------------------------------------------------------------------
    void ScriptSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      InitializeScriptState(app);

      const auto fixed_update = [](ScriptState* script_state)
      {
        script_state->FixedUpdate();
      };
      
      job_graph.Add(foundation::make_job("scriptsystem_fixedupdate", "fixed_update",
        fixed_update,
        foundation::bind_write(script_resource_)
      ));
      
      const auto update = [](ScriptState* script_state)
      {
        script_state->Update(foundation::Frame::delta_time());
      };
      
      job_graph.Add(foundation::make_job("scriptsystem_update", "update",
        update,
        foundation::bind_write(script_resource_)
      ));    

      RegisterClasses(app);
      LoadMain(app);
    }

    void ScriptSystem::InitializeScriptState(Application& app)
    {
      script_resource_ = foundation::Resource<ScriptState*>("ScriptState", &script_state_);
      script_state_.Initialize(&app);

      register_ = ScriptRegister(&script_state_);
    }

    //-------------------------------------------------------------------------
    void ScriptSystem::RegisterClasses(Application& app)
    {
      register_.RegisterAll<
        ScriptClassRegister<ScriptDebug>,
        ScriptClassRegister<ScriptableWorld>,
        ScriptClassRegister<Entity>,
        ScriptClassRegister<CameraComponent>,
        ScriptClassRegister<TransformComponent>,
        ScriptClassRegister<MeshRenderComponent>,
        ScriptClassRegister<RigidBodyComponent>,
        ScriptClassRegister<BoxColliderComponent>,
        ScriptClassRegister<SphereColliderComponent>,
        ScriptClassRegister<CylinderColliderComponent>,
        ScriptClassRegister<CapsuleColliderComponent>,
        ScriptClassRegister<ConeColliderComponent>,
        ScriptClassRegister<AudioEventComponent>,
        ScriptClassRegister<AudioListenerComponent>,
        ScriptClassRegister<CameraEnums>,
        ScriptClassRegister<ScriptableInput>,
        ScriptClassRegister<ButtonEnumWrapper>,
        ScriptClassRegister<ScriptableNetworking>,
        ScriptClassRegister<NetworkComponent>,
        ScriptClassRegister<ScriptableNetworkPlayer>,
        ScriptClassRegister<ScriptableImGuiInputText>,
        ScriptClassRegister<ScriptableImGui>,
        ScriptClassRegister<ScriptableAsset>,
        ScriptClassRegister<ScriptableAssetSystem>,
        ScriptClassRegister<ScriptableAudio>,
        ScriptClassRegister<PhysicsSystem>,
        ScriptClassRegister<FixedConstraintComponent>,
        ScriptClassRegister<HingeConstraintComponent>
      >();

      ScriptableWorld::Initialize(&app.GetService<WorldProviderSystem>(), &app);
      ScriptableInput::Initialize(&app.platform().input());
    }

    //-------------------------------------------------------------------------
    void ScriptSystem::LoadMain(Application& app)
    {
      AssetHandle<Script> script = AssetSystem::Instance().Load<Script>("main");
      foundation::Vector<char> bin = script->binary;

      script_state_.CompileAndRun(bin.data(), bin.size(), "main.lua");
      script_state_.RegisterCallbacks();
    }

    //-------------------------------------------------------------------------
    void ScriptSystem::OnTerminate()
    {
      script_state_.Shutdown();
    }

    //-------------------------------------------------------------------------
    void ScriptSystem::Start(const foundation::String& project_dir)
    {
      script_state_.OnInitialize(project_dir);
    }

  }
}
