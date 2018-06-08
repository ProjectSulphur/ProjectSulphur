#pragma once
#include "physics/iphysics_constraint.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsFixedConstraint : sulphur::physics::IPhysicsConstraint
    * @brief Constraint that keeps attached bodies in place relative to one another.
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class IPhysicsFixedConstraint : public IPhysicsConstraint
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetType
      */
      ConstraintTypes GetType() const override { return IPhysicsConstraint::ConstraintTypes::kFixed; }
    };
  }
}