#pragma once

#if defined(PS_PHYSX)
#include "physics/physx/physx_physics.h"
namespace sulphur
{
  namespace physics
  {
    using PlatformPhysics = PhysXPhysics; //!< Platform specification of the library interface implementation
  }
}
#elif defined (PS_BULLET)
#include "physics/bullet/bullet_physics.h"
namespace sulphur
{
  namespace physics
  {
    using PlatformPhysics = BulletPhysics; //!< Platform specification of the library interface implementation
  }
}
#else
#error "Unimplemented physics SDK"
#endif