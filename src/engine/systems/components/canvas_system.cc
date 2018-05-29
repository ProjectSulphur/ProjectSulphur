#include "engine/systems/components/canvas_system.h"

#include "engine/core/entity_system.h"
#include "engine/application/application.h"
#include "engine/systems/components/camera_system.h"
#include "engine/assets/asset_system.h"
#include "engine/assets/shader.h"

#include "engine/systems/components/transform_system.h"
#include "engine/graphics/irenderer.h"
#include "engine/application/window.h"

#include <foundation/pipeline-assets/shader.h>
#include <foundation/memory/memory.h>
#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    CanvasSystem::CanvasSystem() :
      IComponentSystem("CanvasSystem")
    {
    }

    void CanvasSystem::OnTerminate()
    {
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        CanvasComponent component(camera_system_, *this, component_data_.data.GetSparseFromDataIndex(i));
        CanvasDataRef data = component_data_.data.GetObjectAsStruct<CanvasDataRef>(component);

        for (eastl::pair<Entity, BaseUIElementData*> pair : data.elements)
        {
          foundation::Memory::Destruct(pair.second);
        }
        data.elements.clear();
        data.base_to_entity.clear();
      }
      component_data_.data.Clear();
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      window_ = &app.platform().window();
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      camera_system_ = &world.GetComponent<CameraSystem>();
      transform_system_ = &world.GetComponent<TransformSystem>();
      renderer_ = &app.platform_renderer();

      const auto render = [](CanvasSystem& canvas_system)
      {
        canvas_system.Render();
      };

      foundation::Job render_job = make_job("canvassystem_render", "render",
                                            render, bind_write(*this));
      render_job.set_blocker("meshrendersystem_apply_post_processing");
      job_graph.Add(std::move(render_job));
    }

    //-------------------------------------------------------------------------
    CanvasComponent CanvasSystem::Create(Entity& entity)
    {
      if (false == entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      // TODO (Hilze): Stop hard coding the camera.
      //! Start section here.

      if (false == entity.Has<CameraComponent>())
      {
        glm::ivec2 size = window_->GetSize();
        CameraComponent camera = entity.Add<CameraComponent>();
        camera.SetOrthographicSize(glm::vec2(size.x, size.y) / 2.0f);
        camera.SetProjectionMode(CameraEnums::ProjectionMode::kOrthographic);
        camera.SetRenderTarget(RenderTarget(RenderTargetType::kTexture2D, glm::vec2(size.x, size.y), TextureFormat::kR8G8B8A8_UNORM));
        camera.SetClearMode(CameraEnums::ClearMode::kColor);
      }

      //! End section here.

      foundation::Vector<MeshHandle> meshes = GetMeshes();
      for (MeshHandle mesh : meshes)
      {
        mesh->AttachMesh(Mesh::CreateQuad());
      }

      CanvasComponent component = CanvasComponent(camera_system_, *this, component_data_.data.Add(
        CanvasData::UIElementMap(),
        CanvasData::BaseToEntity(),
        meshes,
        0u,
        entity.Get<CameraComponent>().GetRenderTarget(),
        entity
      ));

      return component;
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::Destroy(ComponentHandleBase handle)
    {
      component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::Render()
    {
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        CanvasComponent component(camera_system_, *this, component_data_.data.GetSparseFromDataIndex(i));
        component.OnRender(*renderer_);
      }
    }

    //-------------------------------------------------------------------------
    foundation::Vector<MeshHandle> CanvasSystem::GetMeshes()
    {
      foundation::Vector<MeshHandle> meshes(kMaxMeshCount);
      
      for (uint8_t i = 0; i < kMaxMeshCount; ++i)
      {
        meshes.at(i) = AssetSystem::Instance().AddAsset(foundation::Memory::Construct<Mesh>(),
          "__ui_element_" + foundation::to_string(current_mesh_count_ + i) + "__"
        );
        meshes.at(i)->MarkDynamic(true);
      }
      
      current_mesh_count_ += kMaxMeshCount;
      
      return meshes;
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::SetCamera(CameraComponent& camera)
    {
      camera_system_->set_current_camera(camera);

      renderer_->SetCamera(
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(),
        camera.GetDepthBuffer(),
        camera.GetRenderTarget()
      );
    }

    //-------------------------------------------------------------------------
    CanvasDataRef CanvasSystem::GetData(CanvasComponent handle)
    {
      return component_data_.data.GetObjectAsStruct<CanvasDataRef>(handle);
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent(System& system, size_t handle) :
      CanvasComponent(nullptr, system, handle)
    {
    }

    CanvasComponent::CanvasComponent(CameraSystem* camera_system, System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system),
      camera_system_(camera_system)
    {
    }

    //-------------------------------------------------------------------------
    void CanvasComponent::Destroy(BaseUIElementComponent element_handle)
    {
      //TODO: Validate handle
      CanvasDataRef canvas_data = system_->GetData(*this);

      Entity entity = canvas_data.base_to_entity.at(element_handle);
      canvas_data.base_to_entity.erase(element_handle);
      
      foundation::Memory::Destruct(canvas_data.elements.at(entity));
      canvas_data.elements.erase(entity);
    }

    //-------------------------------------------------------------------------
    bool ElementSortFunction(BaseUIElementData* lhs, BaseUIElementData* rhs)
    { 
      return lhs->GetDepth() < rhs->GetDepth();
    }

    //-------------------------------------------------------------------------
    void CanvasComponent::OnRender(IRenderer& renderer)
    {
      CanvasDataRef canvas_data = system_->GetData(*this);

      // TODO (Hilze): This is currently not possible yet. Figure out how to fix this.
      CameraComponent camera = canvas_data.canvas_id.Get<CameraComponent>();
      system_->SetCamera(camera);

      //renderer->ClearDepthBuffer(canvas_data.canvas_id.Get<CameraComponent>().GetDepthBuffer());
      // TODO (Hilze): Figure out why this clear does not work. Can it only happen once?

      // TODO (Hilze): Add alpha blending.
      /*graphics::PipelineState pso;
      pso.blend_state.render_target_blend_states[0] = graphics::RenderTargetBlendState{
        true,
        false,
        graphics::BlendFunc::kSrcAlpha,
        graphics::BlendFunc::kInvSrcAlpha,
        graphics::BlendOp::kAdd,
        graphics::BlendFunc::kZero,
        graphics::BlendFunc::kOne,
        graphics::BlendOp::kAdd,
        graphics::LogicOp::kNoop,
        graphics::ColorWriteEnable::kEnableAll
      };

      renderer.SetPipelineState(pso);*/

      // Do a simple depth sort.
      foundation::Vector<BaseUIElementData*> depth_sort(canvas_data.elements.size());
      int idx = 0;
      for (eastl::pair<Entity, BaseUIElementData*> it : canvas_data.elements)
      {
        depth_sort.at(idx++) = it.second;
      }
      std::sort(depth_sort.begin(), depth_sort.end(), ElementSortFunction);

      // Add each element to the mesh.
      for (BaseUIElementData* element : depth_sort)
      {
        Rect bounds = element->GetGlobalBounds();

        // Create the mesh that is going to be renderered.
        element->meshes.at(element->mesh_index)->Clear();
        element->meshes.at(element->mesh_index)->AttachMesh(element->GetMesh(bounds));
        renderer.SetMesh(element->meshes.at(element->mesh_index));
        if (++element->mesh_index >= element->meshes.size())
        {
          element->mesh_index = 0;
        }

        // Set the mesh, model matrix, material and render it.
        renderer.SetModelMatrix(glm::mat4(1.0f));
        
        renderer.SetMaterial(element->GetMaterial());

        renderer.Draw();
      }

      // Render the UI to the screen.
      // --------------------------------------------------
      // Setup camera.
      camera_system_->set_current_camera(camera_system_->main_camera());
      renderer.SetCamera(
        glm::mat4(1.0f),
        glm::mat4(1.0f),
        camera_system_->main_camera().GetDepthBuffer(),
        camera_system_->main_camera().GetRenderTarget()
      );
      // Setup screen quad.
      renderer.SetMesh(canvas_data.meshes.at(canvas_data.mesh_index));
      if (++canvas_data.mesh_index >= canvas_data.meshes.size())
      {
        canvas_data.mesh_index = 0;
      }
      renderer.SetModelMatrix(
        glm::translate(
          glm::scale(
            glm::mat4(1.0f), 
            glm::vec3(2.0f, 2.0f, 1.0f)
          ), 
          glm::vec3(-0.5f, -0.5f, -0.5f)
        )
      );
      // Setup texture.
      MaterialPass material_pass(AssetSystem::Instance().GetHandle<Shader>("Default_Shader"));
      material_pass.SetTexture(0, canvas_data.render_target.GetTextureResource());
      renderer.SetMaterial(material_pass);
      // Render.
      renderer.Draw();
    }

    //-------------------------------------------------------------------------
    BaseUIElementData* CanvasComponent::GetData(BaseUIElementComponent element_handle)
    {
      CanvasDataRef canvas_data = system_->GetData(*this);
      Entity entity = canvas_data.base_to_entity.at(element_handle);
      return canvas_data.elements.at(entity);
    }

    foundation::Vector<MeshHandle> CanvasComponent::GetMeshes()
    {
      return system_->GetMeshes();
    }

    //-------------------------------------------------------------------------
    void CanvasComponent::SetupComponent(Entity& entity, BaseUIElementComponent& component)
    {
      if (false == entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }
      component.handle = entity.handle;
    }

    
    //-------------------------------------------------------------------------
    BaseUIElementComponent::BaseUIElementComponent(CanvasSystem* system, CanvasComponent& canvas) :
      canvas(canvas),
      system(system)
    {
    }

    //-------------------------------------------------------------------------
    void BaseUIElementComponent::SetDepth(const float & depth)
    {
      GetData()->depth = depth;
    }

    //-------------------------------------------------------------------------
    float BaseUIElementComponent::GetDepth()
    {
      return GetData()->depth;
    }

    //-------------------------------------------------------------------------
    Rect BaseUIElementData::GetGlobalBounds(const bool &)
    {
      TransformComponent transform = element_id.Get<TransformComponent>();

      glm::vec3 position = transform.GetWorldPosition();
      glm::vec3 scale    = transform.GetWorldScale();
      position          -= scale * 0.5f;

      return Rect(position.x, position.y, scale.x, scale.y);
    }

    //-------------------------------------------------------------------------
    Mesh BaseUIElementData::GetMesh(const Rect&)
    {
      return Mesh::CreateCircle();
    }

    //-------------------------------------------------------------------------
    MaterialPass BaseUIElementData::GetMaterial()
    {
      return MaterialPass(AssetSystem::Instance().GetHandle<Shader>("Default_Shader"));
    }

    //-------------------------------------------------------------------------
    float BaseUIElementData::GetDepth()
    {
      return depth;
    }

    //-------------------------------------------------------------------------
    BaseUIElementData * BaseUIElementComponent::GetData()
    {
      return canvas.GetData(*this);
    }
  }
}
