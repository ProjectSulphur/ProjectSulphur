#include "mesh_render_system.h"

#include "engine/assets/asset_system.h"
#include "engine/application/application.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/debug_render_system.h"

#include <graphics/platform/pipeline_state.h>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    MeshRenderSystem::MeshRenderSystem() :
      IComponentSystem<MeshRenderComponent, MeshRenderSystemData>("MeshRenderSystem")
    {
    }
    
    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();

      camera_system_ = &world.GetSystem<CameraSystem>();
      tranform_system_ = &world.GetSystem<TransformSystem>();
      renderer_ = &app.platform_renderer();
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnTerminate()
    {
      // Release all asset handles
      component_data_.data.Clear();
    }

    //------------------------------------------------------------------------------------------------------
    MeshRenderComponent MeshRenderSystem::Create(Entity entity)
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
    void MeshRenderSystem::Destroy(MeshRenderComponent handle)
    {
      //TODO: Validate handle
      // Like this?
      if (component_data_.data.IsValid(handle))
      {
        component_data_.data.Remove(handle);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnPreRender()
    {
      // First iteration: Naive approach
      /*const foundation::Vector<CameraComponent>& cameras = camera_system_->GetCameras();

      for (const auto& component : component_data_.data)
      {
        TransformComponent transform = component.entity.Get<TransformComponent>();

        if (!transform.HasChanged())
        {
          continue;
        }

        for (const CameraComponent& camera : cameras)
        {
          camera_entity_map_.emplace(eastl::make_pair(camera.GetIndex(), foundation::Vector<Entity>()));

        }
      }*/

      //List<Camera, List<Entity>>

      // if(camera's layer mask does not contain the layer the component is on, cull it)


      // Option 1:
      // Create cached a list of object

      // Option 2:
      // Do it on the fly
      // slow


      // TODO: Cull Objects
      // TODO: Sort Objects
      // TODO: Batch Objects

      //SortingLayerEntityMap layer_to_entity = tranform_system_->layer_to_entity_map();

      // compile a list of entities 




      //for each camera
      // Add all entities in a layer, no duplicates
    }

    //------------------------------------------------------------------------------------------------------
    void MeshRenderSystem::OnRender()
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

        // FIXME: For now just do for every component
        for (size_t i = 0; i < component_data_.data.size(); ++i)
        {
          MaterialHandle material;
          if (component_data_.materials->empty() || DebugRenderSystem::force_default_material)
          {
            material = AssetSystem::Instance().GetHandle<Material>("Default_Material");
          }
          else
          {
            material = component_data_.materials[i][0];
          }

          if (!material) // If this material is invalid, show the error material
          {
            material = AssetSystem::Instance().GetHandle<Material>("Error_Material");
          }
          

          for (size_t j = 0; j < material->NumMaterialPasses(); ++j)
          {
            const MaterialPass& pass = material->GetMaterialPass(j);

            renderer_->SetMaterial(pass);
            // TODO: Set scissor rect ( probably part of set pipeline state, but whatever)

            if (DebugRenderSystem::HasOverrides())
            {
              graphics::PipelineState ps = pass.shader()->pipeline_state; // Copy pipeline state

              if (DebugRenderSystem::force_wireframe)
                ps.rasterizer_state.fill_mode = graphics::FillMode::kWireFrame;

              renderer_->SetPipelineState(ps);
            }

            TransformComponent transform = 
              component_data_.entity[i].Get<TransformComponent>();
            renderer_->SetModelMatrix(transform.GetLocalToWorld());

            renderer_->SetMesh(component_data_.mesh[i]);
            renderer_->Draw();
          }

        }
        // END FIXME
      }

      // TODO: Prepare post processing data

      /*for (CameraHandle camera : cameras) {
        camera_system_.SetCurrentCamera(camera);
        renderer_->SetCamera(camera);

        // For each post process material {
        // For each material pass {
        // TODO: Upload GPU data (textures etc. by calling renderer->SetTexture(), SetX(), ...)
        // TODO: Set pipeline state
        // TODO: Set scissor rect ( probably part of set pipeline state, but whatever)
        // TODO: Draw call
        // }
        // }
      }*/

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
    void MeshRenderSystem::SetMesh(MeshRenderComponent handle, const MeshHandle& mesh)
    {
      component_data_.data.
        Get<static_cast<size_t>(MeshRenderComponentElements::kMesh)>(handle) = mesh;
    }

    //------------------------------------------------------------------------------------------------------
    MeshHandle MeshRenderComponent::GetMesh() const
    {
      return system_->GetMesh(*this);
    }
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
  }
}