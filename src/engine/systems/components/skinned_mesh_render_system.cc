#include "skinned_mesh_render_system.h"

#include "engine/core/entity_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"

#include "engine/graphics/irenderer.h"
#include "engine/graphics/debug_render_system.h"
#include "engine/application/application.h"
#include "engine/assets/asset_system.h"
#include "engine/assets/mesh.h"
#include "engine/assets/model.h"
#include "engine/assets/material.h"
#include "engine/assets/skeleton.h"

#include <foundation/utils/frame.h>
#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>
#include <graphics/platform/pipeline_state.h>
#include <foundation/memory/memory.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <lua-classes/skinned_mesh_render_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    SkinnedMeshRenderSystem::SkinnedMeshRenderSystem() :
      IComponentSystem("SkinnedMeshRenderSystem")
    {
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();

      camera_system_ = &(world.GetComponent<CameraSystem>());
      tranform_system_ = &(world.GetComponent<TransformSystem>());
      renderer_ = &(app.platform_renderer());

      { // SkinnedMeshRenderSystem Update Animation States Job, starts after MeshRenderSystem Render Meshes job
        const auto function = [](SkinnedMeshRenderSystem& system)
        {
          system.UpdateAnimationStates();
        };

        foundation::Job job = foundation::make_job(
          "skinnedmeshrendersystem_update_animation_states",
          "render",
          function,
          bind_write(*this)
        );

        job.set_blocker("meshrendersystem_render");
        job_graph.Add(std::move(job));
      }

      { // SkinnedMeshRenderSystem Render Meshes Job, starts after Animation States Update job
        const auto function = [](SkinnedMeshRenderSystem& system)
        {
          system.RenderMeshes();
        };

        foundation::Job job = foundation::make_job(
          "skinnedmeshrendersystem_render_meshes",
          "render",
          function,
          bind_write(*this)
        );

        job.set_blocker("skinnedmeshrendersystem_update_animation_states");
        job_graph.Add(std::move(job));
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::OnTerminate()
    {
      // Release all asset handles
      component_data_.data.Clear();
    }

    //------------------------------------------------------------------------------------------------------
    SkinnedMeshRenderComponent SkinnedMeshRenderSystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      return SkinnedMeshRenderComponent(
        *this,
        component_data_.data.Add(
          entity,
          MeshHandle(),
          foundation::Vector<MaterialHandle>(),
          true,
          true,
          SkeletonHandle(),
          AnimationHandle(),
          0.0f,
          0.0f,
          false,
          1.0f,
          foundation::Vector<glm::mat4>()
        )
      );
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::Destroy(ComponentHandleBase handle)
    {
      if (component_data_.data.IsValid(handle))
      {
        component_data_.data.Remove(handle);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::UpdateAnimationStates()
    {
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        if (component_data_.is_playing[i] == true && component_data_.playback_speed[i] > 0.0f)
        {
          component_data_.global_playback_time_in_seconds[i] += 
            foundation::Frame::delta_time() * component_data_.playback_speed[i];

          SkeletonHandle skeleton = component_data_.skeleton[i];
          AnimationHandle animation = component_data_.animation[i];

          if (skeleton.IsValid() && animation.IsValid())
          {
            component_data_.local_playback_time_in_ticks[i] =
              std::fmodf(
                animation->ticks_per_second() * component_data_.global_playback_time_in_seconds[i],
                animation->duration()
              );

            component_data_.bone_matrices[i].resize(skeleton->bones().size());

            CalculateBoneTransform(
              i, 
              skeleton->root_node_index(), 
              component_data_.entity[i].Get<TransformComponent>().GetLocalToWorld()
            );
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::RenderMeshes()
    {
      if (!camera_system_->main_camera().IsValid())
      {
        PS_LOG(Warning, "Unable to render without a main camera!");
        return;
      }

      foundation::Vector<CameraComponent> cameras = camera_system_->GetCameras();

      for (CameraComponent& camera : cameras)
      {
        camera_system_->set_current_camera(camera);

        renderer_->SetCamera(
          camera.GetTransform().GetWorldPosition(),
          camera.GetViewMatrix(),
          camera.GetProjectionMatrix(),
          camera.GetDepthBuffer(),
          camera.GetRenderTarget());

        for (size_t i = 0; i < component_data_.data.size(); ++i)
        {
          TransformComponent transform = component_data_.entity[i].Get<TransformComponent>();
          
          renderer_->SetModelMatrix(transform.GetLocalToWorld());
          renderer_->SetMesh(component_data_.mesh[i]);
          renderer_->SetBoneMatrices(component_data_.bone_matrices[i]);
        
          foundation::Vector<MaterialHandle>& materials = component_data_.materials[i];
          size_t submesh_count = component_data_.mesh[i]->GetSubmeshCount();
        
          if (materials.size() != submesh_count)
          {
            materials.resize(submesh_count);
          }
        
          for (size_t j = 0; j < materials.size(); ++j)
          {
            MaterialHandle material;
        
            if (component_data_.materials->empty() || DebugRenderSystem::force_default_material)
            {
              material = AssetSystem::Instance().GetHandle<Material>("Default_Material");
            }
            else
            {
              material = materials[j];
            }
        
            if (!material) // If this material is invalid, show the error material
            {
              material = AssetSystem::Instance().GetHandle<Material>("Error_Material");
            }
        
            for (size_t k = 0; k < material->NumMaterialPasses(); ++k)
            {
              // Set the material
              const MaterialPass& pass = material->GetMaterialPass(k);
              renderer_->SetMaterial(pass);
        
              // Override pipeline state
              if (DebugRenderSystem::HasOverrides())
              {
                graphics::PipelineState ps = pass.shader()->pipeline_state; // Copy pipeline state
        
                if (DebugRenderSystem::force_wireframe)
                  ps.rasterizer_state.fill_mode = graphics::FillMode::kWireFrame;
        
                renderer_->SetPipelineState(ps);
              }
        
              // Render the mesh
              const SubMeshOffset offset = 
                component_data_.mesh[i]->GetSubmesh(static_cast<uint>(j));
        
              renderer_->Draw(offset.size, offset.offset);
            }
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    SkinnedMeshRenderSystemData& SkinnedMeshRenderSystem::data()
    {
      return component_data_;
    }

    //------------------------------------------------------------------------------------------------------
    SkinnedMeshRenderComponent::SkinnedMeshRenderComponent() :
      system_(nullptr)
    {

    }

    //------------------------------------------------------------------------------------------------------
    SkinnedMeshRenderComponent::SkinnedMeshRenderComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMesh(const MeshHandle& new_mesh)
    {
      SkinnedMeshRenderSystemData& data = system_->data();

      if (!new_mesh) // Is the new mesh valid?
      {
        PS_LOG(
          Warning,
          "[SkinnedMeshRenderSystem] Tried setting a new Mesh on a "
          "SkinnedMeshRenderComponent, but the new Mesh is invalid."
          "Instead of assigning the invalid Mesh, nothing changes."
        );

        // No, it is not valid. Quit the function right here, right now.
        return;
      }

      // Assign the new mesh
      MeshHandle& mesh =
        data.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(*this);

      mesh = new_mesh;
      size_t num_submeshes = mesh->GetSubmeshCount();

      // Resize the materials vector to be of the same size as the
      // number of submeshes, while making sure that it is filled 
      // with valid materials.
      foundation::Vector<MaterialHandle>& materials = 
        data.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(*this);

      materials.resize(num_submeshes);

      for (int i = 0; i < num_submeshes; ++i)
      {
        if (!materials[i])
        {
          materials[i] = AssetSystem::Instance().GetHandle<Material>("Error_Material");
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMesh(ScriptableAsset* mesh)
    {
      if (mesh->type() != ScriptableAsset::AssetTypes::kMesh)
      {
        return;
      }

      BaseAssetHandle* h = mesh->GetHandle();
      MeshHandle* m = static_cast<MeshHandle*>(h);
      SetMesh(*m);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetModel(const ModelHandle& model)
    {
      SetMesh(model->mesh_handle());
      SetMaterials(model->material_handles());
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetModel(ScriptableAsset* model)
    {
      if (model->type() != ScriptableAsset::AssetTypes::kModel)
      {
        return;
      }

      BaseAssetHandle* h = model->GetHandle();
      ModelHandle* m = static_cast<ModelHandle*>(h);
      SetModel(*m);
    }

    //------------------------------------------------------------------------------------------------------
    const MeshHandle& SkinnedMeshRenderComponent::GetMesh() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMaterial(const MaterialHandle& material, size_t index)
    {
      foundation::Vector<MaterialHandle>& materials = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(*this);

      if (materials.size() > index)
      {
        if (!material) // Is the new material valid?
        {
          PS_LOG(
            Warning,
            "[SkinnedMeshRenderSystem] Tried setting a new Material on a "
            "SkinnedMeshRenderComponent at index %i, but the new Material "
            "is invalid. Instead of assigning the invalid Material, it has "
            "been replaced with the default error Material.",
            index
          );

          // No it is not. Replace it with the default error Material.
          materials[index] = AssetSystem::Instance().GetHandle<Material>("Error_Material");
        }
        else
        {
          materials[index] = material;
        }
      }
      else
      {
        PS_LOG(
          Warning,
          "[SkinnedMeshRenderSystem] Tried setting a Material at index %i "
          "but the current Mesh assigned to the SkinnedMeshRenderComponent"
          " has less submeshes. Doing nothing instead.",
          index
        );
      }
    }

    //------------------------------------------------------------------------------------------------------
    MaterialHandle SkinnedMeshRenderComponent::GetMaterial(size_t index) const
    {
      foundation::Vector<MaterialHandle>& materials =
        system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(*this);

      if (materials.size() < index)
      {
        return materials[index];
      }
      else
      {
        PS_LOG(
          Warning,
          "[SkinnedMeshRenderSystem] Tried retrieving a Material from a "
          "SkinnedMeshRenderComponent at index %i, but that index is "
          "larger than the number of materials that reside in this "
          "component. Returning an empty (invalid) handle instead.",
          index
        );
        return MaterialHandle();
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMaterials(
      const foundation::Vector<MaterialHandle>& new_materials
    )
    {
      foundation::Vector<MaterialHandle>& materials = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(*this);

      MeshHandle mesh = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(*this);

      size_t num_submeshes = mesh->GetSubmeshCount();
      size_t num_new_materials = new_materials.size();

      if (num_submeshes > num_new_materials)
      {
        PS_LOG(
          Warning,
          "[SkinnedMeshRenderSystem] Tried setting a new set of Materials on a"
          " SkinnedMeshRenderComponent, but the new set of Materials has less "
          "Materials in it when compared with the number of submeshes in the "
          "Mesh assigned to this component. To compensate for that, Default "
          "Materials have been added instead."
        );

        for (size_t i = 0; i < num_submeshes; ++i)
        {
          if (i < num_new_materials)
          {
            materials[i] = new_materials[i];
          }
          else
          {
            materials[i] = AssetSystem::Instance().GetHandle<Material>("Default_Material");
          }
        }
      }
      else
      {
        materials = new_materials;

        for (int i = 0; i > num_new_materials; i++)
        {
          if (!materials[i]) // Is the newly set material valid?
          {
            PS_LOG(
              Warning,
              "[SkinnedMeshRenderSystem] Tried setting a new set of Materials on a"
              " SkinnedMeshRenderComponent, but the Material at index %i is invalid"
              ". Instead of assigning the invalid Material, it has been replaced "
              "with the default error Material.",
              i
            );

            // No it is not, so replace it with the default Error Material
            materials[i] = AssetSystem::Instance().GetHandle<Material>("Error_Material");
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<MaterialHandle>& SkinnedMeshRenderComponent::GetMaterials() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetVisible(bool visible)
    {
      bool& is_visible = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kVisible)>(*this);

      is_visible = visible;
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::IsVisible() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kVisible)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetCastsShadows(bool casts_shadows)
    {
      bool& casting_shadows = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kCastsShadows)>(*this);

      casting_shadows = casts_shadows;
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::CastsShadows() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kCastsShadows)>(*this);
    }
    
    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetAnimation(AnimationHandle animation_handle)
    {
      AnimationHandle& animation = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kAnimation)>(*this);

      animation = animation_handle;
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetAnimation(ScriptableAsset* animation)
    {
      if (animation->type() != ScriptableAsset::AssetTypes::kAnimation)
      {
        return;
      }

      BaseAssetHandle* h = animation->GetHandle();
      AnimationHandle* m = static_cast<AnimationHandle*>(h);

      SetAnimation(*m);
    }

    //------------------------------------------------------------------------------------------------------
    AnimationHandle SkinnedMeshRenderComponent::GetAnimation() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kAnimation)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetSkeleton(SkeletonHandle skeleton_handle)
    {
      SkeletonHandle& skeleton = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kSkeleton)>(*this);

      skeleton = skeleton_handle;
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetSkeleton(ScriptableAsset* skeleton)
    {
      if (skeleton->type() != ScriptableAsset::AssetTypes::kSkeleton)
      {
        return;
      }

      BaseAssetHandle* h = skeleton->GetHandle();
      SkeletonHandle* m = static_cast<SkeletonHandle*>(h);

      SetSkeleton(*m);
    }

    //------------------------------------------------------------------------------------------------------
    SkeletonHandle SkinnedMeshRenderComponent::GetSkeleton() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kSkeleton)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::Play()
    {
      SetPlaying(true, true);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::Stop()
    {
      SetPlaying(false, true);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::Pause()
    {
      SetPlaying(false, false);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::Resume()
    {
      SetPlaying(true, false);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetPlaying(bool playing, bool reset_playback_time)
    {
      bool& is_playing = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kIsPlaying)>(*this);

      is_playing = playing;

      if (reset_playback_time == true)
      {
        SetPlaybackTime(0.0f);
      }
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::IsPlaying() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kIsPlaying)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::TogglePlaying(bool reset_playback_time)
    {
      SetPlaying(!IsPlaying(), reset_playback_time);

      return IsPlaying();
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetPlaybackTime(float playback_time)
    {
      float& time = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kGlobalPlaybackTime)>(*this);

      time = playback_time;
    }

    //------------------------------------------------------------------------------------------------------
    float SkinnedMeshRenderComponent::GetPlaybackTime() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kGlobalPlaybackTime)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetPlaybackSpeed(float playback_speed)
    {
      float& speed = system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kPlaybackSpeed)>(*this);

      speed = playback_speed;
    }

    //------------------------------------------------------------------------------------------------------
    float SkinnedMeshRenderComponent::GetPlaybackSpeed() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kPlaybackSpeed)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<glm::mat4>& SkinnedMeshRenderComponent::GetBoneMatrices() const
    {
      return system_->data().data.
        Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kBoneMatrices)>(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::CalculateBoneTransform(
      unsigned int component_index,
      unsigned int node_index,
      const glm::mat4& parent_transform
    )
    {
      const SkeletalNode& node = component_data_.skeleton[component_index]->node_at(node_index);

      glm::mat4 node_transform = node.transform;

      unsigned int animation_channel_index = UINT_MAX;
      GetChannelByNodeName(component_index, node.name, &animation_channel_index);

      if (animation_channel_index != UINT_MAX)
      {
        // process the animation channel
        // set node transform to be result of interpolated animation channel
        const AnimationChannel& channel = component_data_.
          animation[component_index]->animation_channels()[animation_channel_index];

        float& local_playback_time = component_data_.local_playback_time_in_ticks[component_index];

        glm::vec3 position = ProcessKeyframes(
          local_playback_time,
          channel.position_keys
        );
        glm::quat rotation = ProcessKeyframes(
          local_playback_time,
          channel.rotation_keys
        );
        glm::vec3 scale = ProcessKeyframes(
          local_playback_time,
          channel.scale_keys
        );

        glm::mat4 position_matrix = glm::translate(position);
        glm::mat4 rotation_matrix = glm::toMat4(rotation);
        glm::mat4 scaling_matrix = glm::scale(scale);

        node_transform = position_matrix * rotation_matrix * scaling_matrix;
      }

      glm::mat4 global_transform = parent_transform * node_transform;

      SkeletonHandle skeleton = component_data_.skeleton[component_index];

      if (skeleton->bone_names().find(node.name) != skeleton->bone_names().end())
      {
        foundation::Vector<glm::mat4>& bone_matrices = 
          component_data_.bone_matrices[component_index];

        bone_matrices[component_data_.skeleton[component_index]->bone_names().at(node.name)] =
          glm::inverse(component_data_.skeleton[component_index]->root_node().transform) *
          global_transform *
          skeleton->bones()[skeleton->bone_names().at(node.name)].offset;
      }

      for (size_t i = 0; i < node.children.size(); i++)
      {
        CalculateBoneTransform(component_index, node.children[i], global_transform);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::GetChannelByNodeName(
      unsigned int component_index,
      const foundation::String& node_name,
      unsigned int* out_channel_index) const
    {
      AnimationHandle animation = component_data_.animation[component_index];

      for (int i = 0; i < animation->animation_channels().size(); ++i)
      {
        if (animation->animation_channels()[i].bone_name == node_name)
        {
          *out_channel_index = i;
          break;
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    glm::vec3 SkinnedMeshRenderSystem::ProcessKeyframes(
      float playback_time,
      const foundation::Vector<Vector3Keyframe>& keyframes) const
    {
      if (keyframes.size() == 0)
      {
        return glm::vec3(1.0f, 1.0f, 1.0f);
      }
      
      if (keyframes.size() == 1)
      {
        return keyframes[0].value;
      }

      unsigned int current_key_index = UINT_MAX;
      unsigned int next_key_index = UINT_MAX;

      for (int i = static_cast<int>(keyframes.size()) - 1; i >= 0; i--)
      {
        if (keyframes[i].time < playback_time)
        {
          current_key_index = static_cast<unsigned int>(i);
          break;
        }
      }

      if (current_key_index == UINT_MAX)
      {
        return keyframes[0].value;
      }

      next_key_index = static_cast<unsigned int>(fmod(current_key_index + 1, keyframes.size()));

      float delta_time = keyframes[next_key_index].time - keyframes[current_key_index].time;
      float factor = (playback_time - keyframes[current_key_index].time) / delta_time;

      const glm::vec3& start = keyframes[current_key_index].value;
      const glm::vec3& end = keyframes[next_key_index].value;
      glm::vec3 to_end = end - start;

      return start + (to_end * factor);
    }

    //------------------------------------------------------------------------------------------------------
    glm::quat SkinnedMeshRenderSystem::ProcessKeyframes(
      float playback_time,
      const foundation::Vector<QuaternionKeyframe>& keyframes) const
    {
      if (keyframes.size() == 0)
      {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
      }

      if (keyframes.size() == 1)
      {
        return keyframes[0].value;
      }

      unsigned int current_key_index = UINT_MAX;
      unsigned int next_key_index = UINT_MAX;

      for (int i = static_cast<int>(keyframes.size()) - 1; i >= 0; i--)
      {
        if (keyframes[i].time < playback_time)
        {
          current_key_index = static_cast<unsigned int>(i);
          break;
        }
      }

      if (current_key_index == UINT_MAX)
      {
        return keyframes[0].value;
      }

      next_key_index = static_cast<unsigned int>(fmod(current_key_index + 1, keyframes.size()));

      float delta_time = keyframes[next_key_index].time - keyframes[current_key_index].time;
      float factor = (playback_time - keyframes[current_key_index].time) / delta_time;

      const glm::quat& start = keyframes[current_key_index].value;
      const glm::quat& end = keyframes[next_key_index].value;

      return glm::slerp(start, end, factor);
    }
  }
}
