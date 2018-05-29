#include "light_system.h"

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
      IComponentSystem("LightSystem")
    {
    }

    //------------------------------------------------------------------------------------------------------
    void LightSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      renderer_ = &app.platform_renderer();
    }
  }
}