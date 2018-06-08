#pragma once

#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsCapsuleShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for capsule shapes.
    * @author Angelo van der Mark
    */
    class IPhysicsCapsuleShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kCapsule; }

      /**
      * @brief Sets the radius of this capsule shape.
      * @param[in] radius (float) The radius to set.
      */
      virtual void SetRadius(float radius) = 0;

      /**
      * @brief Returns the radius of this capsule shape.
      * @return (float) The radius of the capsule shape.
      */
      virtual float GetRadius() const = 0;

      /**
      * @brief Sets the height of the capsule shape.
      * @note This takes the radius into account and should not be set lower than the radius.
      * @param[in] height (float) The total height of the capsule.
      */
      virtual void SetHeight(float height) = 0;

      /**
      * @brief Returns the total height of this capsule shape.
      * @return (float) The height of the capsule shape.
      */
      virtual float GetHeight() const = 0;
    };
  }
}