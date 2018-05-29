#include "mesh_render_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"

#include "engine/application/application.h"
#include "engine/assets/asset_system.h"
#include "engine/graphics/debug_render_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/assets/scriptable_asset_system.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>
#include <graphics/platform/pipeline_state.h>

#include <lua-classes/mesh_render_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    MeshRenderSystem::MeshRenderSystem() :
      IComponentSystem("MeshRenderSystem")
    {
    }
    
    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();

      camera_system_ = &world.GetComponent<CameraSystem>();
      tranform_system_ = &world.GetComponent<TransformSystem>();
      renderer_ = &app.platform_renderer();

      { // MeshRenderSystem Render Meshes Job, starts after DebugRenderSystem Render Job
        const auto function = [](MeshRenderSystem& system)
        {
          system.RenderMeshes();
        };

        foundation::Job job = foundation::make_job(
          "meshrendersystem_render",
          "render",
          function,
          bind_write(*this)
        );

        job.set_blocker("debugrendersystem_render");
        job_graph.Add(std::move(job));
      } 
      
      { // MeshRenderSystem Apply Post Processing Job, starts after SkinnedMeshRenderSystem Render Job
        const auto function = [](MeshRenderSystem& system)
        {
          system.ApplyPostProcessing();
        };

        foundation::Job job = foundation::make_job(
          "meshrendersystem_apply_post_processing",
          "render",
          function,
          bind_write(*this)
        );

        job.set_blocker("skinnedmeshrendersystem_render_meshes");
        job_graph.Add(std::move(job));
      }
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnTerminate()
    {
      // Release all asset handles
      component_data_.data.Clear();
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderSystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      return MeshRenderComponent(*this, component_data_.data.Add(
        MeshHandle(),
        foundation::Vector<MaterialHandle>(),
        true,
        true,
        true,
        entity));
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::Destroy(ComponentHandleBase handle)
    {
      if (component_data_.data.IsValid(handle))
      {
        component_data_.data.Remove(handle);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::RenderMeshes()
    {
      if (!camera_system_->main_camera().IsValid())
      {
        PS_LOG(Warning, "Unable to render without a main camera!");
        return;
      }

      // TODO: Set per frame data ?
      foundation::Vector<CameraComponent> cameras = camera_system_->GetCameras();

      // IF I CAN FIGURE OUT WHAT MESHES RENDER ON THE SAME CAMERA, I CAN MODIFY THE MVP AND USE THE SAME STATE
      for (CameraComponent& camera : cameras)
      {
        camera_system_->set_current_camera(camera);
        renderer_->SetCamera(
          camera.GetViewMatrix(),
          camera.GetProjectionMatrix(),
          camera.GetDepthBuffer(),
          camera.GetRenderTarget());

        // TODO: Render batches for this camera
        // -> Use Material passes

        for (size_t i = 0; i < component_data_.data.size(); ++i)
        {
          TransformComponent transform =
            component_data_.entity[i].Get<TransformComponent>();

          // Layer culling
          if (camera.GetLayerMask().ContainsLayer(transform.GetSortingLayer()) == false)
          {
            continue;
          }

          // Frustum culling
          const foundation::Sphere bounding_sphere = component_data_.mesh[i]->bounding_sphere().
            Transform(transform.GetWorldPosition(), transform.GetWorldScale());
          if(camera.GetFrustum().Intersects(bounding_sphere) == false)
          {
            continue;
          }

          renderer_->SetModelMatrix(transform.GetLocalToWorld());

          renderer_->SetMesh(component_data_.mesh[i]);

          foundation::Vector<MaterialHandle>& materials = component_data_.materials[i];
          size_t submesh_count = component_data_.mesh[i]->GetSubmeshCount();
          
          // Make sure we render with a material
          UpdateMaterials(materials, submesh_count);

          for (size_t j = 0; j < materials.size(); ++j)
          {
            MaterialHandle material = materials[j];

            if (DebugRenderSystem::force_default_material)
            {
              material = AssetSystem::Instance().GetHandle<Material>("Default_Material");
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
    void MeshRenderSystem::ApplyPostProcessing()
    {
      foundation::Vector<CameraComponent> cameras = camera_system_->GetCameras();

      for (CameraComponent& camera : cameras)
      {
        camera_system_->set_current_camera(camera);

        PostProcessMaterialHandle material = camera.GetPostProcessMaterial();

        if (!material)
        {
          continue;
        }

        for (size_t j = 0; j < material->NumComputePasses(); ++j)
        {
          const ComputePass& pass = material->GetComputePass(j);
          const TextureHandle& tex = material->GetUAV(0);
          uint width = tex->width();
          uint height = tex->height();

          renderer_->SetComputePass(pass);
          const glm::vec3 group_size = pass.GetWorkGroupSize();
          uint groups_x = static_cast<uint>(ceil(width / group_size.x));
          uint groups_y = static_cast<uint>(ceil(height / group_size.y));
          renderer_->Dispatch(
            graphics::ComputeQueueType::kPostProcessing,
            groups_x,
            groups_y,
            1u
          );
        }
      }

      // Reset current camera to main
      camera_system_->set_current_camera(camera_system_->main_camera());
    }
    
    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent::MeshRenderComponent() :
      system_(nullptr)
    {
    }
    
    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent::MeshRenderComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetMesh(const MeshHandle& mesh)
    {
      system_->SetMesh(*this, mesh);
      return *this;
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetMesh(ScriptableAsset* mesh)
    {
      if (mesh->type() != ScriptableAsset::AssetTypes::kMesh)
      {
        return *this;
      }

      BaseAssetHandle* h = mesh->GetHandle();
      MeshHandle* m = static_cast<MeshHandle*>(h);
      system_->SetMesh(*this, *m);
      system_->SetMaterial(*this, AssetSystem::Instance().GetHandle<Material>("Default_Material"));
      return *this;
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetModel(ScriptableAsset* model)
    {
      if (model->type() != ScriptableAsset::AssetTypes::kModel)
      {
        return *this;
      }

      BaseAssetHandle* h = model->GetHandle();
      ModelHandle m = *static_cast<ModelHandle*>(h);
      system_->SetMesh(*this, m->mesh_handle());
      system_->SetMaterials(*this, m->material_handles());
      return *this;
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::SetMesh(MeshRenderComponent handle, const MeshHandle& mesh)
    {
      component_data_.data.
        Get<static_cast<size_t>(MeshRenderComponentElements::kMesh)>(handle) = mesh;
    }

    //------------------------------------------------------------------------------------------------------
    MeshHandle MeshRenderComponent::GetMesh(MeshHandle*) const
    {
      return system_->GetMesh(*this);
    }

    //------------------------------------------------------------------------------------------------------
    MeshHandle MeshRenderSystem::GetMesh(MeshRenderComponent handle) const
    {
      return component_data_.data.
        Get<static_cast<size_t>(MeshRenderComponentElements::kMesh)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetMaterial(
      const MaterialHandle& material,
      size_t index)
    {
      system_->SetMaterial(*this, material, index);
      return *this;
    }
    void MeshRenderSystem::SetMaterial(
      MeshRenderComponent handle,
      const MaterialHandle& material,
      size_t index)
    {
      foundation::Vector<MaterialHandle>& material_list = 
        component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kMaterials)>(handle);

      if (material_list.size() <= index)
      {
        material_list.resize(index + 1);
      }

      material_list[index] = material;
    }

    //------------------------------------------------------------------------------------------------------
    MaterialHandle MeshRenderComponent::GetMaterial(size_t index) const
    {
      return system_->GetMaterial(*this, index);
    }
    MaterialHandle MeshRenderSystem::GetMaterial(MeshRenderComponent handle, size_t index) const
    {
      foundation::Vector<MaterialHandle>& material_list =
        component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kMaterials)>(handle);

      assert(material_list.size() > index);

      return material_list[index];
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetMaterials(
      const foundation::Vector<MaterialHandle>& materials)
    {
      system_->SetMaterials(*this, materials);
      return *this;
    }
    void MeshRenderSystem::SetMaterials(
      MeshRenderComponent handle,
      const foundation::Vector<MaterialHandle>& materials)
    {
      component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kMaterials)>(handle) = materials;
    }

    //------------------------------------------------------------------------------------------------------
    foundation::Vector<MaterialHandle> MeshRenderComponent::GetMaterials() const
    {
      return system_->GetMaterials(*this);
    }
    foundation::Vector<MaterialHandle> MeshRenderSystem::GetMaterials(
      MeshRenderComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kMaterials)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetVisible(bool value)
    {
      system_->SetVisible(*this, value);
      return *this;
    }
    void MeshRenderSystem::SetVisible(MeshRenderComponent handle, bool value)
    {
      component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kVisible)>(handle) = value;
    }

    //------------------------------------------------------------------------------------------------------
    bool MeshRenderComponent::IsVisible() const
    {
      return system_->IsVisible(*this);
    }
    bool MeshRenderSystem::IsVisible(MeshRenderComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kVisible)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderComponent::SetCastShadows(bool value)
    {
      system_->SetCastShadows(*this, value);
      return *this;
    }
    void MeshRenderSystem::SetCastShadows(MeshRenderComponent handle, bool value)
    {
      component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kCastShadows)>(handle) = value;
    }

    //------------------------------------------------------------------------------------------------------
    bool MeshRenderComponent::CastsShadows() const
    {
      return system_->CastsShadows(*this);
    }
    bool MeshRenderSystem::CastsShadows(MeshRenderComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>
        (MeshRenderComponentElements::kCastShadows)>(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::UpdateMaterials(foundation::Vector<MaterialHandle>& materials, size_t expected_count)
    {
      if (materials.size() != expected_count) // If no material was set, apply the default one
        materials.resize(expected_count, AssetSystem::Instance().GetHandle<Material>("Default_Material"));

      for (MaterialHandle& material : materials)
      {
        if (!material) // If this material is invalid, show the error material
          material = AssetSystem::Instance().GetHandle<Material>("Error_Material");
      }
    }
  }
}
