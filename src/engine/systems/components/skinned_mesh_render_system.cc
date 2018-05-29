#include "skinned_mesh_render_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"

#include "engine/graphics/irenderer.h"
#include "engine/graphics/debug_render_system.h"
#include "engine/graphics/animation_state.h"
#include "engine/application/application.h"
#include "engine/assets/asset_system.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>
#include <graphics/platform/pipeline_state.h>

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
          nullptr,
          true,
          true
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
        component_data_.animation_state[i]->Update();
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
          camera.GetViewMatrix(),
          camera.GetProjectionMatrix(),
          camera.GetDepthBuffer(),
          camera.GetRenderTarget());

        for (size_t i = 0; i < component_data_.data.size(); ++i)
        {
          TransformComponent transform = component_data_.entity[i].Get<TransformComponent>();
          
          renderer_->SetModelMatrix(transform.GetLocalToWorld());
          renderer_->SetMesh(component_data_.mesh[i]);
          renderer_->SetBoneMatrices(component_data_.animation_state[i]->GetTransformedBones());
        
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
              const SubMeshOffset offset = component_data_.mesh[i]->GetSubmesh(static_cast<uint>(j));
        
              renderer_->Draw(offset.size, offset.offset);
            }
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::SetMesh(SkinnedMeshRenderComponent handle, const MeshHandle& new_mesh)
    {
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
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(handle);

      mesh = new_mesh;
      size_t num_submeshes = mesh->GetSubmeshCount();
      
      // Resize the materials vector to be of the same size as the
      // number of submeshes, while making sure that it is filled 
      // with valid materials.
      foundation::Vector<MaterialHandle>& materials =
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(handle);

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
    const MeshHandle& SkinnedMeshRenderSystem::GetMesh(SkinnedMeshRenderComponent handle)
    {
      return component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::SetMaterial(SkinnedMeshRenderComponent handle, const MaterialHandle& material, size_t index)
    {
      foundation::Vector<MaterialHandle>& materials =
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(handle);

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
    MaterialHandle SkinnedMeshRenderSystem::GetMaterial(SkinnedMeshRenderComponent handle, size_t index)
    {
      foundation::Vector<MaterialHandle>& materials = 
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(handle);

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
    void SkinnedMeshRenderSystem::SetMaterials(SkinnedMeshRenderComponent handle, const foundation::Vector<MaterialHandle>& new_materials)
    {
      foundation::Vector<MaterialHandle>& materials =
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(handle);

      MeshHandle mesh = component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMesh)>(handle);
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
    const foundation::Vector<MaterialHandle>& SkinnedMeshRenderSystem::GetMaterials(SkinnedMeshRenderComponent handle)
    {
      return 
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kMaterials)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::SetAnimationState(SkinnedMeshRenderComponent handle, AnimationState* animation_state)
    {
      component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kAnimationState)>(handle) =
        animation_state;
    }

    //------------------------------------------------------------------------------------------------------
    AnimationState* SkinnedMeshRenderSystem::GetAnimationState(SkinnedMeshRenderComponent handle)
    {
      return 
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kAnimationState)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::SetVisible(SkinnedMeshRenderComponent handle, bool visible)
    {
      bool& is_visible = 
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kVisible)>(handle);

      is_visible = visible;
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderSystem::IsVisible(SkinnedMeshRenderComponent handle)
    {
      return 
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kVisible)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderSystem::SetCastsShadows(SkinnedMeshRenderComponent handle, bool casts_shadow)
    {
      bool& is_casting_shadows =
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kCastsShadows)>(handle);

      is_casting_shadows = casts_shadow;
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderSystem::CastsShadows(SkinnedMeshRenderComponent handle)
    {
      return
        component_data_.data.Get<static_cast<size_t>(SkinnedMeshRenderComponentElements::kCastsShadows)>(handle);
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
    void SkinnedMeshRenderComponent::SetMesh(const MeshHandle& mesh)
    {
      system_->SetMesh(*this, mesh);
    }

    //------------------------------------------------------------------------------------------------------
    const MeshHandle& SkinnedMeshRenderComponent::GetMesh()
    {
      return system_->GetMesh(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMaterial(const MaterialHandle& material, size_t index)
    {
      system_->SetMaterial(*this, material, index);
    }

    //------------------------------------------------------------------------------------------------------
    MaterialHandle SkinnedMeshRenderComponent::GetMaterial(size_t index)
    {
      return system_->GetMaterial(*this, index);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetMaterials(const foundation::Vector<MaterialHandle>& materials)
    {
      system_->SetMaterials(*this, materials);
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<MaterialHandle>& SkinnedMeshRenderComponent::GetMaterials()
    {
      return system_->GetMaterials(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetAnimationState(AnimationState* animation_state)
    {
      system_->SetAnimationState(*this, animation_state);
    }

    //------------------------------------------------------------------------------------------------------
    AnimationState* SkinnedMeshRenderComponent::GetAnimationState()
    {
      return system_->GetAnimationState(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetVisible(bool visible)
    {
      system_->SetVisible(*this, visible);
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::IsVisible()
    {
      return system_->IsVisible(*this);
    }

    //------------------------------------------------------------------------------------------------------
    void SkinnedMeshRenderComponent::SetCastsShadows(bool casts_shadows)
    {
      system_->SetCastsShadows(*this, casts_shadows);
    }

    //------------------------------------------------------------------------------------------------------
    bool SkinnedMeshRenderComponent::CastsShadows()
    {
      return system_->CastsShadows(*this);
    }
  }
}
