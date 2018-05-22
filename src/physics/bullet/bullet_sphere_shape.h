#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletSphereShape : public sulphur::physics::BulletShape
    * @brief The specialization for sphere shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class BulletSphereShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a sphere shape
      */
      BulletSphereShape();

      ShapeTypes GetType() const override;

      /**
      * @brief Sets the radius of this sphere shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius);

      /**
      * @brief Returns the radius of this sphere shape.
      * @return (float) The radius of the sphere shape.
      */
      float GetRadius() const;
    };
  }
}