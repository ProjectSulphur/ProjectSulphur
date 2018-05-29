#pragma once
#include "physics/iphysics_fixed_constraint.h"

#include <glm/glm.hpp>
#include "physics/bullet/bullet_include.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletFixedConstraint : public sulphur::physics::IPhysicsFixedConstraint
    * @brief Fixed constraint implementation
    * @author Benjamin Waanders
    */
    class BulletFixedConstraint : public IPhysicsFixedConstraint
    {
    public:
      /**
      * @brief constructor default values, currently up to bullets restrictions
      */
      BulletFixedConstraint(btRigidBody* body_a, btRigidBody* body_b, btDynamicsWorld* world);

      /**
      * @brief The default deconstructor
      */
      virtual ~BulletFixedConstraint();

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetTorqueLimit
      */
      virtual void SetTorqueLimit(float limit) override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetForceLimit
      */
      virtual void SetForceLimit(float limit) override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetForceLimit
      */
      virtual float GetForceLimit() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetTorqueLimit
      */
      virtual float GetTorqueLimit() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetBodyA
      */
      virtual PhysicsBody * GetBodyA() override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetBodyB
      */
      virtual PhysicsBody * GetBodyB() override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetFrameA
      */
      virtual void SetFrameA(const glm::mat4x4 & transform) override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetFrameB
      */
      virtual void SetFrameB(const glm::mat4x4 & transform) override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetFrameA
      */
      virtual const glm::mat4x4 GetFrameA() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetFrameB
      */
      virtual const glm::mat4x4 GetFrameB() const override;

    private:
      btFixedConstraint* bullet_constraint_; //!< The internal constraint
      btRigidBody* body_a_; //!< The A body
      btRigidBody* body_b_; //!< The B body
      btDynamicsWorld* world_; //!< pointer to the dynamics world that the constraint lives in

      btTransform frame_a_;
      btTransform frame_b_;
    };
  }
}
