#pragma once

#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsSphereShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for sphere shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class IPhysicsSphereShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kSphere; }

      /**
      * @brief Sets the radius of this sphere shape.
      * @param[in] radius (float) The radius to set.
      */
      virtual void SetRadius(float radius) = 0;

      /**
      * @brief Returns the radius of this sphere shape.
      * @return (float) The radius of the sphere shape.
      */
      virtual float GetRadius() const = 0;
    };
  }
}