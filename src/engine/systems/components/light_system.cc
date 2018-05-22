#include "light_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    LightComponent::LightComponent() :
      system_(nullptr)
    {
    }

    //------------------------------------------------------------------------------------------------------
    LightComponent::LightComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //------------------------------------------------------------------------------------------------------
    LightSystem::LightSystem() :
      IComponentSystem<LightComponent, LightData>("LightSystem")
    {
    }

    //------------------------------------------------------------------------------------------------------
    void LightSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      renderer_ = &app.platform_renderer();
    }

    //------------------------------------------------------------------------------------------------------
    LightComponent LightSystem::Create(Entity entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      return LightComponent(*this, component_data_.data.Add(
        foundation::Color::kHalfDutchWhite,
        1.0f,
        10.0f,
        30.0f,
        LightType::kDirectionalLight,
        entity));
    }

    //------------------------------------------------------------------------------------------------------
    void LightSystem::Destroy(LightComponent handle)
    {
      // TODO: Validate handle
      component_data_.data.Remove(handle);
    }

    //------------------------------------------------------------------------------------------------------
    void LightSystem::OnUpdate(float)
    {
    }

    //------------------------------------------------------------------------------------------------------
    void LightSystem::OnPreRender()
    {
    }
  }
}