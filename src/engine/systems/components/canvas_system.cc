#include "canvas_system.h"

#include "engine/core/entity_system.h"
#include "engine/application/application.h"
#include "engine/systems/components/camera_system.h"
#include "engine/assets/asset_system.h"
#include "engine/assets/shader.h"

#include <foundation/pipeline-assets/material.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/memory/memory.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    CanvasSystem::CanvasSystem() :
      IComponentSystem<CanvasComponent, CanvasData>("CanvasSystem")
    {
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      camera_system_ = &world.GetSystem<CameraSystem>();
      transform_system_ = &world.GetSystem<TransformSystem>();
      renderer_ = &app.platform_renderer();
    }

    //-------------------------------------------------------------------------
    CanvasComponent CanvasSystem::Create(Entity entity)
    {
      if (false == entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      // TODO (Hilze): Stop hard coding the camera.
      //! Start section here.
      //data.render_target = RenderTarget(RenderTargetType::kTexture2D, glm::vec2(1920.0f, 1080.0f), TextureFormat::kR8G8B8A8_UNORM);
      
      if (false == entity.Has<CameraComponent>())
      {
        CameraComponent camera = entity.Add<CameraComponent>();
        camera.SetOrthographicSize(glm::vec2(1920.0f, 1080.0f) / 2.0f);
        camera.SetProjectionMode(ProjectionMode::kOrthographic);
        //camera.SetRenderTarget(RenderTarget());
        //camera.SetClearMode(ClearMode::kDepthOnly);
      }

      //! End section here.

      CanvasComponent component = CanvasComponent(*this, component_data_.data.Add(
        CanvasData::UIElementMap(),
        CanvasData::BaseToEntity(),
        entity.Get<CameraComponent>().GetRenderTarget(),
        entity
      ));

      return component;
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::Destroy(CanvasComponent handle)
    {
      component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::OnRender()
    {
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        CanvasComponent component(*this, component_data_.data.GetSparseFromDataIndex(i));
        component.OnRender(*renderer_);
      }
    }

    //-------------------------------------------------------------------------
    void CanvasSystem::SetCamera(CameraComponent camera)
    {
      camera_system_->set_current_camera(camera);
    }

    //-------------------------------------------------------------------------
    CanvasDataRef CanvasSystem::GetData(CanvasComponent handle)
    {
      return CanvasDataRef(
        component_data_.data.Get<static_cast<size_t>(CanvasDataElements::kElements)>(handle),
        component_data_.data.Get<static_cast<size_t>(CanvasDataElements::kBaseToElement)>(handle),
        component_data_.data.Get<static_cast<size_t>(CanvasDataElements::kRenderTarget)>(handle),
        component_data_.data.Get<static_cast<size_t>(CanvasDataElements::kCanvasId)>(handle)
      );
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    CanvasComponent::CanvasComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    ImageUIElementComponent CanvasComponent::CreateImage(Entity & entity)
    {
      return Create<ImageUIElementComponent, ImageUIElementData>(entity);
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

      renderer.SetCamera(
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(),
        camera.GetDepthBuffer(),
        camera.GetRenderTarget());

      //renderer->ClearDepthBuffer(canvas_data.canvas_id.Get<CameraComponent>().GetDepthBuffer());
      // TODO (Hilze): Figure out why this clear does not work. Can it only happen once?

      // TODO (Hilze): Add alpha blending.
      /*PipelineState pso;
      pso.blend_state.render_target_blend_states[0] = RenderTargetBlendState{
        true,
        false,
        BlendFunc::kSrcAlpha,
        BlendFunc::kInvSrcAlpha,
        BlendOp::kAdd,
        BlendFunc::kZero,
        BlendFunc::kOne,
        BlendOp::kAdd
      };

      renderer->SetPipelineState(pso);*/

      foundation::Vector<BaseUIElementData*> depth_sort(canvas_data.elements.size());
      int idx = 0;
      for (eastl::pair<Entity, BaseUIElementData*> it : canvas_data.elements)
      {
        depth_sort.at(idx++) = it.second;
      }

      std::sort(depth_sort.begin(), depth_sort.end(), ElementSortFunction);

      for (BaseUIElementData* element : depth_sort)
      {
        Rect bounds = element->GetGlobalBounds();

        // # Please make sure that you do not release resources that are still in use. #screw_the_100_character_mark
        foundation::String name = "ui_mesh_" + foundation::to_string(rand());
        MeshHandle mesh_handle = AssetSystem::Instance().AddAsset(
          foundation::Memory::Construct<Mesh>(element->GetMesh(bounds)),
          name
        );

        renderer.SetModelMatrix(glm::mat4(1.0f));
        
        renderer.SetMaterial(element->GetMaterial());

        renderer.SetMesh(mesh_handle);
        
        renderer.Draw();
      }
    }

    //-------------------------------------------------------------------------
    BaseUIElementData * CanvasComponent::GetData(BaseUIElementComponent element_handle)
    {
      CanvasDataRef canvas_data = system_->GetData(*this);
      Entity entity = canvas_data.base_to_entity.at(element_handle);
      return canvas_data.elements.at(entity);
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetColor(const foundation::Color & color)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->color = color;
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetTexture(const TextureHandle & texture)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->texture = texture;
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetTextureRect(const Rect & texture_rect)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->texture_rect = texture_rect;
    }

    //-------------------------------------------------------------------------
    BaseUIElementComponent::BaseUIElementComponent(Application& app, CanvasComponent& canvas) :
      canvas(canvas)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      system = &world.GetSystem<CanvasSystem>();
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
    Mesh BaseUIElementData::GetMesh(const Rect &)
    {
      return Mesh();
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

    //-------------------------------------------------------------------------
    Rect ImageUIElementData::GetGlobalBounds(const bool & clipped)
    {
      return BaseUIElementData::GetGlobalBounds(clipped);
    }

    //-------------------------------------------------------------------------
    Mesh ImageUIElementData::GetMesh(const Rect & clipped_bounds)
    {
      // TODO (Hilze): Do something with the clipped bounds.

      glm::vec2 min = clipped_bounds.Min();
      glm::vec2 max = clipped_bounds.Max();

      glm::vec2 tex_min = texture_rect.Min();
      glm::vec2 tex_max = texture_rect.Max();

      Mesh mesh;

      mesh.SetVertices({
        glm::vec3(min.x, max.y, 0.0f),
        glm::vec3(min.x, min.y, 0.0f),
        glm::vec3(max.x, min.y, 0.0f),
        glm::vec3(max.x, max.y, 0.0f)
      });
      mesh.SetNormals({
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      });
      mesh.SetUVs({
        glm::vec2(tex_min.x, tex_max.y),
        glm::vec2(tex_min.x, tex_min.y),
        glm::vec2(tex_max.x, tex_min.y),
        glm::vec2(tex_max.x, tex_max.y)
      });

      mesh.SetIndices({
        0, 1, 2,
        0, 2, 3
      });

      //glm::vec3 scale = glm::vec3(max - min, 1.0f);
      //glm::vec3 center = glm::vec3((min + max) / 2.0f, 1.0f);

      return mesh;// .CreateQuad().TransformMesh(center, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), scale);
    }

    //-------------------------------------------------------------------------
    MaterialPass ImageUIElementData::GetMaterial()
    {
      MaterialPass pass = BaseUIElementData::GetMaterial();
      pass.SetTexture(0, texture);
      return pass;
    }
  }
}