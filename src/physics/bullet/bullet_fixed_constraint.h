#pragma once
#include "physics/iphysics_fixed_constraint.h"
#include "physics/platform_physics_body.h"

class btDynamicsWorld;
class btGeneric6DofSpring2Constraint;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletFixedConstraint : public sulphur::physics::IPhysicsFixedConstraint
    * @brief Fixed constraint implementation
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class BulletFixedConstraint : public IPhysicsFixedConstraint
    {
    public:

      /**
      * @brief Constructor.
      * @param[in] body_a (sulphur::physics::PhysicsBody*) The main body.
      * @param[in] world (btDynamicsWorld*) The dynamics world this constraint will live in.
      */
      BulletFixedConstraint(PhysicsBody* body_a, btDynamicsWorld* world);

      /**
      * @brief Destructor.
      */
      ~BulletFixedConstraint();

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetTorqueLimit
      */
      void SetTorqueLimit(float limit) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetForceLimit
      */
      void SetForceLimit(float limit) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetForceLimit
      */
      float GetForceLimit() const override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetTorqueLimit
      */
      float GetTorqueLimit() const override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetEnabled
      */
      void SetEnabled(bool enabled) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::IsEnabled
      */
      bool IsEnabled() const override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetBodyA
      */
      PhysicsBody* GetBodyA() override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetBodyB
      */
      void SetBodyB(PhysicsBody* body_b) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetBodyB
      */
      PhysicsBody* GetBodyB() override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetFrameA
      */
      void SetFrameA(const glm::mat4x4& transform) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::SetFrameB
      */
      void SetFrameB(const glm::mat4x4& transform) override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetFrameA
      */
      glm::mat4x4 GetFrameA() const override;

      /**
      * @see sulphur::physics::IPhysicsConstraint::GetFrameB
      */
      glm::mat4x4 GetFrameB() const override;

    private:
      btGeneric6DofSpring2Constraint* bullet_constraint_; //!< The internal constraint.
      PhysicsBody* body_a_; //!< The A body.
      PhysicsBody* body_b_; //!< The B body.
      btDynamicsWorld* world_; //!< Pointer to the dynamics world that the constraint lives in.
    };
  }
}
