#pragma once
#include "physics/iphysics_hinge_constraint.h"

#include <glm/glm.hpp>
#include "physics/bullet/bullet_include.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletFixedConstraint : public sulphur::physics::IPhysicsHingeConstraint
    * @brief Fixed constraint implementation
    * @author Benjamin Waanders
    */
    class BulletHingeConstraint : public IPhysicsHingeConstraint
    {
    public:
      /**
      * @brief constructor default values, currently up to bullets restrictions
      */
      BulletHingeConstraint(btRigidBody* body_a, btRigidBody* body_b, btDynamicsWorld* world);

      /**
      * @brief the default deconstructor
      */
      virtual ~BulletHingeConstraint();

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
      virtual void SetFrameA(const glm::vec3 & pivot, const glm::vec3 & axis, bool calc_b) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetFrameA
      */
      virtual void SetFrameB(const glm::vec3 & pivot, const glm::vec3 & axis) override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetPivotA
      */
      virtual const glm::vec3 GetPivotA() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetPivotB
      */
      virtual const glm::vec3 GetPivotB() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetAxisA
      */
      virtual const glm::vec3 GetAxisA() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetAxisB
      */
      virtual const glm::vec3 GetAxisB() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetPivotB
      */
      virtual void SetPivotB(const glm::vec3& pivot) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetPivotA
      */
      virtual void SetPivotA(const glm::vec3& pivot) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetAxisA
      */
      virtual void SetAxisA(const glm::vec3& axis) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetAxisB
      */
      virtual void SetAxisB(const glm::vec3& axis) override; 

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetMin
      */
      virtual void SetMin(float min) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetMax
      */
      virtual void SetMax(float max) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetMin
      */
      virtual float GetMin() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetMax
      */
      virtual float GetMax() const override;

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
      btHingeConstraint* bullet_constraint_; //!< The internal constraint
      btDynamicsWorld* world_; //!< pointer to the dynamics world that the constraint lives in
      btRigidBody* body_a_; //!< The A body
      btRigidBody* body_b_; //!< The B body

      btVector3 pivot_a_; //!< Local stored pivot
      btVector3 pivot_b_; //!< Local stored pivot
      btVector3 axis_a_; //!< Local stored axis
      btVector3 axis_b_; //!< Local stored axis

      float limit_min_; //!< Local min range
      float limit_max_; //!< Local max range
    };
  }
}
