#pragma once

#if defined(PS_PHYSX)
namespace sulphur
{
  namespace physics
  {
  }
}
#elif defined (PS_BULLET)
#include "physics/bullet/bullet_fixed_constraint.h"
#include "physics/bullet/bullet_hinge_constraint.h"

namespace sulphur
{
  namespace physics
  {
    using FixedConstraint = BulletFixedConstraint; //!< Platform specific fixed constraint.
    using HingeConstraint = BulletHingeConstraint; //!< Platform specific fixed constraint.
  }
}
#else
#error "Unimplemented physics SDK"
#endif
