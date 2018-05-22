#pragma once

#if defined(PS_PHYSX)
#include "physics/physx/physx_body.h"
namespace sulphur
{
  namespace physics
  {
    using PhysicsBody = PhysXBody; //!< PhysicsBody implementation to use.
  }
}
#elif defined (PS_BULLET)
#include "physics/bullet/bullet_body.h"

namespace sulphur
{
  namespace physics
  {
    using PhysicsBody = BulletBody; //!< PhysicsBody implementation to use.
  }
}
#else
#error "Unimplemented physics SDK"
#endif