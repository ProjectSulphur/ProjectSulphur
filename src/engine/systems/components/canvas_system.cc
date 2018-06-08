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
      for (int i = 0; i < data_.size(); ++i)
      {
        CanvasData& data = data_.at(i);

        for (eastl::pair<Entity, BaseUIElementData*> pair : data.elements)
        {
          foundation::Memory::Destruct(pair.second);
        }
        data.elements.clear();
        data.base_to_entity.clear();
      }
    }

    static ShaderHandle shader_handle;

    //-------------------------------------------------------------------------
    void CanvasSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      window_ = &app.platform().window();
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      camera_system_ = &world.GetComponent<CameraSystem>();
      transform_system_ = &world.GetComponent<TransformSystem>();
      renderer_ = &app.platform_renderer();

      // TODO (Hilze): Make the no light shaders standard
//#define UI_SYSTEM_NEW_SHADERS
#ifdef UI_SYSTEM_NEW_SHADERS
      auto vertex = AssetSystem::Instance().Load<ShaderProgram>("no_light_vertex");
      auto pixel = AssetSystem::Instance().Load<ShaderProgram>("no_light_pixel");
#else
      auto vertex = AssetSystem::Instance().Load<ShaderProgram>("default_vertex");
      auto pixel = AssetSystem::Instance().Load<ShaderProgram>("default_pixel");
#endif

      Shader* asset = foundation::Memory::Construct<Shader>(vertex,
        ShaderProgramHandle(),
        ShaderProgramHandle(),
        ShaderProgramHandle(),
        pixel
        );

      shader_handle = AssetSystem::Instance().AddAsset<Shader>(asset, "ui_shader");

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

      glm::ivec2 size = window_->GetSize();
      if (false == entity.Has<CameraComponent>())
      {
        CameraComponent camera = entity.Add<CameraComponent>();
        camera.SetOrthographicSize(glm::vec2(size.x, size.y) / 2.0f);
        camera.SetProjectionMode(CameraEnums::ProjectionMode::kOrthographic);
        camera.SetRenderTarget(RenderTarget(RenderTargetType::kTexture2D, glm::vec2(8), TextureFormat::kR8G8B8A8_UNORM));
        camera.SetClearMode(CameraEnums::ClearMode::kColor);
        camera.SetClearColor(foundation::Color::kBlackTransparent);
      }

      //! End section here.

      foundation::Vector<MeshHandle> meshes = GetMeshes();
      for (MeshHandle mesh : meshes)
      {
        mesh->AttachMesh(Mesh::CreateQuad());
      }



      CanvasComponent component = CanvasComponent(camera_system_, *this, entity);

      data_.push_back(CanvasData{
        true,
        CanvasData::UIElementMap(),
        CanvasData::BaseToEntity(),
        meshes,
        0u,
        RenderTarget(RenderTargetType::kTexture2D, glm::vec2(size.x, size.y), TextureFormat::kR8G8B8A8_UNORM),
        entity,
        true
      });
      data_to_entity_[(uint32_t)data_.size() - 1u] = entity;
      entity_to_data_[entity] = (uint32_t)data_.size() - 1u;

      return component;
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::Destroy(ComponentHandleBase handle)
    {
      CanvasComponent* cc = static_cast<CanvasComponent*>(&handle);
      const auto& it = entity_to_data_.find(cc->entity_);
      if (it != entity_to_data_.end())
      {
        uint32_t id = it->second;

        for (auto i = data_to_entity_.find(id); i != data_to_entity_.end(); i++)
        {
          entity_to_data_.at(i->second)--;
        }

        data_.erase(data_.begin() + id);
        entity_to_data_.erase(it);
        data_to_entity_.erase(id);
      }
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::Render()
    {
      for (CanvasData& data : data_)
      {
        CanvasComponent component(camera_system_, *this, data.canvas_id);
        if (GetData(component).visible)
        {
          component.OnRender(*renderer_);
        }
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
    void CanvasSystem::SetCamera(CameraComponent& camera, const RenderTarget& rt)
    {
      camera_system_->set_current_camera(camera);

      renderer_->SetCamera(
        camera.GetTransform().GetWorldPosition(),
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(),
        DepthBuffer(),
        rt
      );
    }

    void CanvasSystem::SetVisible(const CanvasComponent & canvas, bool visible)
    {
      GetData(canvas).visible = visible;
    }

    void CanvasSystem::SetBlendEnabled(const CanvasComponent & canvas, bool enabled)
    {
      auto data = GetData(canvas);
      data.blend_enabled = enabled;
    }

    //-------------------------------------------------------------------------
    CanvasData& CanvasSystem::GetData(CanvasComponent handle)
    {
      return data_.at(entity_to_data_.at(handle.entity_));
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent() :
      system_(nullptr)
    {
    }

    CanvasComponent::CanvasComponent(const CanvasComponent& other) :
      CanvasComponent(other.camera_system_, *other.system_, other.entity_)
    {
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent(System& system, Entity entity) :
      CanvasComponent(nullptr, system, entity)
    {
    }

    CanvasComponent::CanvasComponent(CameraSystem* camera_system, System& system, Entity entity) :
      ComponentHandleBase(0u),
      entity_(entity),
      system_(&system),
      camera_system_(camera_system)
    {
    }

    //-------------------------------------------------------------------------
    void CanvasComponent::Destroy(BaseUIElementComponent element_handle)
    {
      //TODO: Validate handle
      CanvasData& canvas_data = system_->GetData(*this);

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
      CanvasData& canvas_data = system_->GetData(*this);

      // TODO (Hilze): This is currently not possible yet. Figure out how to fix this.
      renderer.ClearRenderTarget(canvas_data.render_target, glm::vec4(0.0f));
      renderer.SetCamera(
        glm::vec3(0.0f),
        glm::mat4(1.0f),
        glm::mat4(1.0f),
        engine::DepthBuffer(),
        canvas_data.render_target
      );

      //renderer->ClearDepthBuffer(canvas_data.canvas_id.Get<CameraComponent>().GetDepthBuffer());
      // TODO (Hilze): Figure out why this clear does not work. Can it only happen once?

      // TODO (Hilze): Add alpha blending.
#ifndef PS_WIN32
      graphics::PipelineState pso;
      pso.blend_state.render_target_blend_states[0] = graphics::RenderTargetBlendState{
        true,
        false,
        graphics::BlendFunc::kSrcAlpha,
        graphics::BlendFunc::kInvSrcAlpha,
        graphics::BlendOp::kAdd,
        graphics::BlendFunc::kOne,
        graphics::BlendFunc::kOne,
        graphics::BlendOp::kAdd,
        graphics::LogicOp::kNoop,
        graphics::ColorWriteEnable::kEnableAll
      };

      pso.depth_stencil_state.depth_enable = false;
#endif

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

#ifndef PS_WIN32
        if (!canvas_data.blend_enabled)
        {
          renderer.SetPipelineState(pso);
        }
#endif
        renderer.Draw();
      }

      // Render the UI to the screen.
      // --------------------------------------------------
      // Setup camera.
      camera_system_->set_current_camera(camera_system_->main_camera());
      renderer.SetCamera(
        glm::vec3(0.0f),
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
      MaterialPass material_pass(shader_handle);
      material_pass.SetTexture(0, canvas_data.render_target.GetTextureResource());
      renderer.SetMaterial(material_pass);
      // Render.
#ifndef PS_WIN32
      renderer.SetPipelineState(pso);
#endif
      renderer.Draw();
    }

    void CanvasComponent::SetVisible(bool visible)
    {
      system_->SetVisible(*this, visible);
    }

    void CanvasComponent::SetBlendEnabled(bool enabled)
    {
      system_->SetBlendEnabled(*this, enabled);
    }

    //-------------------------------------------------------------------------
    BaseUIElementData* CanvasComponent::GetData(Entity entity)
    {
      CanvasData& canvas_data = system_->GetData(*this);
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
    BaseUIElementComponent::BaseUIElementComponent(CanvasSystem* system, CanvasComponent& canvas, Entity self) :
      canvas(canvas),
      system(system),
      self(self)
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
      return MaterialPass(shader_handle);
    }

    //-------------------------------------------------------------------------
    float BaseUIElementData::GetDepth()
    {
      return depth;
    }

    //-------------------------------------------------------------------------
    BaseUIElementData * BaseUIElementComponent::GetData()
    {
      return canvas.GetData(self);
    }
  }
}
