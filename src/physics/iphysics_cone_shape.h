#pragma once

#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsConeShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for cone shapes.
    * @author Angelo van der Mark
    */
    class IPhysicsConeShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kCone; }

      /**
      * @brief Sets the radius of this cone shape.
      * @param[in] radius (float) The radius to set.
      */
      virtual void SetRadius(float radius) = 0;

      /**
      * @brief Returns the radius of this cone shape.
      * @return (float) The radius of the cone shape.
      */
      virtual float GetRadius() const = 0;

      /**
      * @brief Sets the height of the cone shape.
      * @param[in] height (float) The total height of the cone.
      */
      virtual void SetHeight(float height) = 0;

      /**
      * @brief Returns the total height of this cone shape.
      * @return (float) The height of the cone shape.
      */
      virtual float GetHeight() const = 0;
    };
  }
}