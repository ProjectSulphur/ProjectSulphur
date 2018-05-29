#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletConeShape : public sulphur::physics::BulletShape
    * @brief The specialization for cone shapes.
    * @author Angelo van der Mark
    */
    class BulletConeShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a cone shape
      */
      BulletConeShape();

      ShapeTypes GetType() const override;

      /**
      * @brief Sets the radius of this cone shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius);

      /**
      * @brief Returns the radius of this cone shape.
      * @return (float) The radius of the cone shape.
      */
      float GetRadius() const;

      /**
      * @brief Sets the height of the cone shape.
      * @param[in] height (float) The total height of the cone.
      */
      void SetHeight(float height);

      /**
      * @brief Returns the total height of this cone shape.
      * @return (float) The height of the cone shape.
      */
      float GetHeight() const;
    };
  }
}