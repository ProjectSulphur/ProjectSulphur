#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletCylinderShape : public sulphur::physics::BulletShape
    * @brief The specialization for cylinder shapes.
    * @author Angelo van der Mark
    */
    class BulletCylinderShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a cylinder shape
      */
      BulletCylinderShape();

      ShapeTypes GetType() const override;

      /**
      * @brief Sets the radius of this cylinder shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius);

      /**
      * @brief Returns the radius of this cylinder shape.
      * @return (float) The radius of the cylinder shape.
      */
      float GetRadius() const;

      /**
      * @brief Sets the height of the cylinder shape.
      * @param[in] height (float) The total height of the cylinder.
      */
      void SetHeight(float height);

      /**
      * @brief Returns the total height of this cylinder shape.
      * @return (float) The height of the cylinder shape.
      */
      float GetHeight() const;
    };
  }
}