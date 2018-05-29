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
#include "physics/bullet/bullet_cylinder_shape.h"
#include "physics/bullet/bullet_cone_shape.h"
#include "physics/bullet/bullet_convex_shape.h"
namespace sulphur
{
  namespace physics
  {
    using PhysicsBoxShape = BulletBoxShape; //!< Platform specific box shape to use.
    using PhysicsSphereShape = BulletSphereShape; //!< Platform specific sphere shape to use.
    using PhysicsCylinderShape = BulletCylinderShape; //!< Platform specific cylinder shape to use.
    using PhysicsCapsuleShape = BulletCapsuleShape; //!< Platform specific capsule shape to use.
    using PhysicsConeShape = BulletConeShape; //!< Platform specific capsule shape to use.
    using PhysicsConvexShape = BulletConvexShape; //!< Platform specific convex hull shape to use.
  }
}
#else
#error "Unimplemented physics SDK"
#endif
