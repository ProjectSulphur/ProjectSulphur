#pragma once

#include "physics/platform_physics_body.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::PhysicsContraint
    * @brief Base interface for all physics constraints
    * @author Benjamin Waanders
    */
    class IPhysicsConstraint
    {
    public:

      /**
      * @brief Virtual destructor.
      */
      virtual ~IPhysicsConstraint() = default;

      /**
      * @brief torque limit setter
      */
      virtual void SetTorqueLimit(const float limit) = 0;
      /**
      * @brief force limit setter
      */
      virtual void SetForceLimit(const float limit) = 0;

      /**
      * @brief force_limit getter
      */
      virtual float GetForceLimit() const = 0;
      /**
      * @brief Torque limit getter
      */
      virtual float GetTorqueLimit() const = 0;

      /**
      * @brief Getter for Body A
      */
      virtual PhysicsBody* GetBodyA() = 0;

      /**
      * @brief Getter for Body B
      */
      virtual PhysicsBody* GetBodyB() = 0;

      /**
      * @brief Sets the frame relative to the A body of the constraint
      * @param[in] transform (const glm::mat4x4&)
      */
      virtual void SetFrameA(const glm::mat4x4& transform) = 0;

      /**
      * @brief Sets the frame relative to the B body of the constraint
      * @param[in] transform (const glm::mat4x4&)
      */
      virtual void SetFrameB(const glm::mat4x4& transform) = 0;

      /**
      * @brief Getter for the frame to the constraint, relative to the A body
      * @return (const glm::mat4x4&) the transform that represents the frame
      */
      virtual const glm::mat4x4 GetFrameA() const = 0;

      /**
      * @brief Getter for the frame to the constraint, relative to the A body
      * @return (const glm::mat4x4&) the transform that represents the frame
      */
      virtual const glm::mat4x4 GetFrameB() const = 0;

    protected:
      float torque_limit_; //!< The torque limit that breaks the constraint
      float force_limit_; //!< The foce limit that breaks the constraint
    };
  }
}
