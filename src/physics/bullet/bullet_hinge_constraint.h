#pragma once
#include "physics/iphysics_hinge_constraint.h"
#include "physics/platform_physics_body.h"

class btDynamicsWorld;
class btHingeConstraint;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletHingeConstraint : public sulphur::physics::IPhysicsHingeConstraint
    * @brief Hinge constraint implementation.
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class BulletHingeConstraint : public IPhysicsHingeConstraint
    {
    public:
      
      /**
      * @brief Constructor.
      * @param[in] body_a (sulphur::physics::PhysicsBody*) The main body.
      * @param[in] world (btDynamicsWorld*) The dynamics world this constraint will live in.
      */
      BulletHingeConstraint(PhysicsBody* body_a, btDynamicsWorld* world);

      /**
      * @brief Destructor.
      */
      ~BulletHingeConstraint();

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
      * @see sulphur::physics::IPhysicsFixedConstraint::SetFrameA
      */
      void SetFrameA(const glm::mat4x4& transform) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetFrameB
      */
      void SetFrameB(const glm::mat4x4& transform) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetFrameA
      */
      glm::mat4x4 GetFrameA() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetFrameB
      */
      glm::mat4x4 GetFrameB() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetPivotA
      */
      glm::vec3 GetPivotA() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetPivotB
      */
      glm::vec3 GetPivotB() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetAxisA
      */
      glm::vec3 GetAxisA() const override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::GetAxisB
      */
      glm::vec3 GetAxisB() const override;

      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetPivotB
      */
      void SetPivotB(const glm::vec3& pivot) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetPivotA
      */
      void SetPivotA(const glm::vec3& pivot) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetAxisA
      */
      void SetAxisA(const glm::vec3& axis) override;
      /**
      * @see sulphur::physics::IPhysicsFixedConstraint::SetAxisB
      */
      void SetAxisB(const glm::vec3& axis) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::SetMinimumAngle
      */
      void SetMinimumAngle(float angle) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::SetMaximumAngle
      */
      void SetMaximumAngle(float angle) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetHingeAngle
      */
      float GetHingeAngle() const override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetMinimumAngle
      */
      float GetMinimumAngle() const override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetMaximumAngle
      */
      float GetMaximumAngle() const override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::SetSoftness
      */
      void SetSoftness(float softness) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetSoftness
      */
      float GetSoftness() const override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::SetBiasFactor
      */
      void SetBiasFactor(float bias) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetBiasFactor
      */
      float GetBiasFactor() const override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::SetRelaxationFactor
      */
      void SetRelaxationFactor(float relaxation) override;

      /**
      * @see sulphur::physics::IPhysicsHingeConstraint::GetRelaxationFactor
      */
      float GetRelaxationFactor() const override;

    private:
      btHingeConstraint* bullet_constraint_; //!< The internal constraint
      btDynamicsWorld* world_; //!< pointer to the dynamics world that the constraint lives in
      PhysicsBody* body_a_; //!< The A body
      PhysicsBody* body_b_; //!< The B body
    };
  }
}
