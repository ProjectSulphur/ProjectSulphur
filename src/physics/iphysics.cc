#include "physics/iphysics.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    const glm::vec3 IPhysics::kDefaultGravity = glm::vec3{ 0.0f, -10.0f, 0.0f };

    //-------------------------------------------------------------------------
    const size_t IPhysics::kManifoldBufferLimit = 2048;
  }
}
