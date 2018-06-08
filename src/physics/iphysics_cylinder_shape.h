#pragma once

#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsCylinderShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for cylinder shapes.
    * @author Angelo van der Mark
    */
    class IPhysicsCylinderShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kCylinder; }

      /**
      * @brief Sets the radius of this cylinder shape.
      * @param[in] radius (float) The radius to set.
      */
      virtual void SetRadius(float radius) = 0;

      /**
      * @brief Returns the radius of this cylinder shape.
      * @return (float) The radius of the cylinder shape.
      */
      virtual float GetRadius() const = 0;

      /**
      * @brief Sets the height of the cylinder shape.
      * @param[in] height (float) The total height of the cylinder.
      */
     virtual void SetHeight(float height) = 0;

      /**
      * @brief Returns the total height of this cylinder shape.
      * @return (float) The height of the cylinder shape.
      */
      virtual float GetHeight() const = 0;
    };
  }
}