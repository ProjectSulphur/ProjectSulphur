#pragma once
#include "engine/core/world.h"

#include "engine/systems/component_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/transform_system.h"

#include "engine/assets/texture.h"
#include "engine/assets/mesh.h"
#include "engine/assets/render_target.h"
#include "engine/graphics/irenderer.h"

#include <foundation/utils/color.h>

#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    class Entity;
    class CanvasSystem;
    class CanvasComponent;

    class Rect
    {
    public:
      Rect() : position_(0.0f), size_(0.0f) {};
      Rect(glm::vec2 position, glm::vec2 size) :
        position_(position), size_(size) {};
      Rect(float x, float y, float w, float h) :
        position_(x, y), size_(w, h) {};

      static Rect FromMinMax(glm::vec2 min, glm::vec2 max)
      {
        return Rect(min, max - min);
      }

      glm::vec2 get_position() const
      {
        return position_;
      }
      glm::vec2 get_size() const
      {
        return size_;
      }
      glm::vec2 Min() const
      {
        return position_;
      }
      glm::vec2 Max() const
      {
        return position_ + size_;
      }

      Rect Clip(const Rect& other)
      {
        return Clip(*this, other);
      }
      static Rect Clip(const Rect& lhs, const Rect& rhs)
      {
        glm::vec2 min = glm::max(lhs.Min(), rhs.Min());
        glm::vec2 max = glm::min(lhs.Max(), rhs.Max());
        return FromMinMax(min, max);
      }

    private:
      glm::vec2 position_;
      glm::vec2 size_;
    };

    struct BaseUIElementData
    {
      Entity element_id;
      virtual ~BaseUIElementData() {};
      float depth = 0.0f;

      // TODO (Hilze): Find out if there is a better way than this.
      virtual Rect GetGlobalBounds(const bool& clipped = true);
      virtual Mesh GetMesh(const Rect& clipped_bounds);
      virtual MaterialPass GetMaterial();
      virtual float GetDepth();
    };

    class BaseUIElementComponent : public ComponentHandleBase
    {
    public:
      using System = CanvasSystem;

    public:
      BaseUIElementComponent(Application& app, CanvasComponent& canvas);

      void SetDepth(const float& depth);
      float GetDepth();

    protected:
      friend class CanvasComponent;

      BaseUIElementData* GetData();

      CanvasComponent& canvas;
      System* system;

    };

    struct ImageUIElementData : public BaseUIElementData
    {
      TextureHandle texture;
      foundation::Color color = foundation::Color::kWhite;
      Rect texture_rect = Rect(glm::vec2(0.0f), glm::vec2(1.0f));
      virtual Rect GetGlobalBounds(const bool& clipped = true) override;
      virtual Mesh GetMesh(const Rect& clipped_bounds) override;
      virtual MaterialPass GetMaterial() override;
    };

    class ImageUIElementComponent : public BaseUIElementComponent
    {
    public:
      ImageUIElementComponent(Application& app, CanvasComponent& canvas) :
        BaseUIElementComponent(app, canvas) {};

      void SetColor(const foundation::Color& color);
      void SetTexture(const TextureHandle& texture);
      void SetTextureRect(const Rect& texture_rect);

    };

    class CanvasComponent : public ComponentHandleBase
    {
    public:
      using System = CanvasSystem;
      
    public:
      CanvasComponent();
      CanvasComponent(System& system, size_t handle);

      ImageUIElementComponent CreateImage(Entity& entity);
      
      void Destroy(BaseUIElementComponent handle);
      void OnRender(IRenderer& renderer);

    protected:
      friend class BaseUIElementComponent;
      BaseUIElementData* GetData(BaseUIElementComponent handle);

    private:
      template<typename H, typename D>
      H Create(Entity& entity);

      Application* application_;
      System* system_;
      CameraSystem* camera_system_;

    };


    enum struct CanvasDataElements
    {
      kElements,
      kBaseToElement,
      kRenderTarget,
      kCanvasId
    };

    struct CanvasData
    {
      using UIElementMap = foundation::Map<Entity, BaseUIElementData*>;
      using BaseToEntity = foundation::Map<BaseUIElementComponent, Entity>;
      using ComponentSystemData = SystemData<
        UIElementMap,
        BaseToEntity,
        RenderTarget,
        Entity>;

      CanvasData() :
        data((void**)&elements)
      {
      }

      UIElementMap elements;
      BaseToEntity base_to_entity;
      RenderTarget render_target;
      Entity       canvas_id;

      ComponentSystemData data;
    };

    struct CanvasDataRef
    {
      CanvasDataRef(
        CanvasData::UIElementMap& elements,
        CanvasData::BaseToEntity& base_to_entity,
        RenderTarget& render_target,
        Entity& canvas_id
      ) :
        elements(elements),
        base_to_entity(base_to_entity),
        render_target(render_target),
        canvas_id(canvas_id)
      {}
    
      CanvasData::UIElementMap& elements;
      CanvasData::BaseToEntity& base_to_entity;
      RenderTarget& render_target;
      Entity& canvas_id;
    };

    class CanvasSystem : public IComponentSystem<CanvasComponent, CanvasData>
    {
    private:
      friend class CanvasComponent;

    public:
      CanvasSystem();

      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      CanvasComponent Create(Entity entity) override;
      void Destroy(CanvasComponent handle) override;
      void OnRender() override;

      void SetCamera(CameraComponent camera);

    protected:
      CanvasDataRef GetData(CanvasComponent handle);

    private:
      CameraSystem* camera_system_;
      TransformSystem* transform_system_;
      IRenderer* renderer_;

    };

    template<typename C, typename D>
    inline C CanvasComponent::Create(Entity& entity)
    {
      if (false == entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }
      CanvasDataRef canvas_data = system_->GetData(*this);

      C component(*application_, *this);
      component.handle = entity.handle;

      D* data = foundation::Memory::Construct<D>(); // Yo Hilze, you never destroy this one on system terminate
      data->element_id = entity;

      canvas_data.elements.insert(eastl::make_pair(entity, data));
      canvas_data.base_to_entity.insert(eastl::make_pair(component, entity));

      return component;
    }
  }
}