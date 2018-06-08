#pragma once

#include "physics/platform_physics_body.h"

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsContraint
    * @brief Base interface for all physics constraints
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class IPhysicsConstraint
    {
    public:

      /**
      * @brief The different constraint types that are supported.
      */
      enum struct ConstraintTypes
      {
        kFixed,
        kHinge,
        kSlider
      };

      /**
      * @brief Virtual destructor.
      */
      virtual ~IPhysicsConstraint() = default;

      /**
      * @brief Returns the type of this constraint.
      * @return (sulphur::physics::ConstraintTypes) The type of this constraint.
      */
      virtual ConstraintTypes GetType() const = 0;

      /**
      * @brief Sets the force limit of the constraint before it breaks.
      * @param[in] limit (float) The force limit, or 0 for an unbreakable constraint.
      * @remarks The Bullet implementation does not distinguish between torque and force.
      */
      virtual void SetForceLimit(float limit) = 0;
      
      /**
      * @brief Sets the torque limit of the constraint before it breaks.
      * @param[in] limit (float) The torque limit, or 0 for an unbreakable constraint.
      * @remarks The Bullet implementation does not distinguish between torque and force.
      */
      virtual void SetTorqueLimit(float limit) = 0;

      /**
      * @brief Returns the force limit of the constraint.
      * @return (float) The maximum force the constraint can take before it breaks.
      */
      virtual float GetForceLimit() const = 0;

      /**
      * @brief Returns the torque limit of the constraint.
      * @return (float) The maximum torque the constraint can take before it breaks.
      */
      virtual float GetTorqueLimit() const = 0;

      /**
      * @brief Sets whether a constraint is active in the world. Can be used to re-enable broken constraints.
      * @param[in] enabled (bool) Whether the constraint should be active or not.
      */
      virtual void SetEnabled(bool enabled) = 0;

      /**
      * @brief Returns whether the constraint is active or not. Broken constraints are considered inactive.
      * @return (bool) True if the constraint is active.
      */
      virtual bool IsEnabled() const = 0;

      /**
      * @brief Returns the main body this constraint is attached to.
      * @return (sulphur::physics::PhysicsBody*) The body that owns this constraint.
      */
      virtual PhysicsBody* GetBodyA() = 0;

      /**
      * @brief Attaches a second body to this constraint.
      * @param[in] body (sulphur::physics::PhysicsBody*) The body to attach, or 0 to detach the previous body.
      */
      virtual void SetBodyB(PhysicsBody* body) = 0;

      /**
      * @brief Returns the second body this constraint is connected with.
      * @return (sulphur::physics::PhysicsBody*) The secondary body attached to this constraint, or null if there is none.
      */
      virtual PhysicsBody* GetBodyB() = 0;

      /**
      * @brief Sets the reference frame of the owning body relative to the constraint.
      * @param[in] transform (const glm::mat4x4&) The relative transform.
      */
      virtual void SetFrameA(const glm::mat4x4& transform) = 0;

      /**
      * @brief Returns the reference frame of the owning body relative to the constraint.
      * @return (glm::mat4x4) The relative transform.
      */
      virtual glm::mat4x4 GetFrameA() const = 0;

      /**
      * @brief Sets the reference frame of the attached body relative to the constraint.
      * @param[in] transform (const glm::mat4x4&) The relative transform.
      */
      virtual void SetFrameB(const glm::mat4x4& transform) = 0;

      /**
      * @brief Returns the reference frame of the attached body relative to the constraint.
      * @return (glm::mat4x4) The relative transform.
      */
      virtual glm::mat4x4 GetFrameB() const = 0;
    };
  }
}
