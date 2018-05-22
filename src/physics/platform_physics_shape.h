#pragma once

#if defined(PS_PHYSX)
#include "physics/physx/physx_box_shape.h"
#include "physics/physx/physx_sphere_shape.h"
namespace sulphur
{
  namespace physics
  {
    using PhysicsBoxShape = PhysXBoxShape; //!< Platform specific box shape to use.
    using PhysicsSphereShape = PhysXSphereShape; //!< Platform specific sphere shape to use.
  }
}
#elif defined (PS_BULLET)
#include "physics/bullet/bullet_box_shape.h"
#include "physics/bullet/bullet_sphere_shape.h"
#include "physics/bullet/bullet_capsule_shape.h"
namespace sulphur
{
  namespace physics
  {
    using PhysicsBoxShape = BulletBoxShape; //!< Platform specific box shape to use.
    using PhysicsSphereShape = BulletSphereShape; //!< Platform specific sphere shape to use.
    using PhysicsCapsuleShape = BulletCapsuleShape; //!< Platform specific capsule shape to use.
  }
}
#else
#error "Unimplemented physics SDK"
#endif
