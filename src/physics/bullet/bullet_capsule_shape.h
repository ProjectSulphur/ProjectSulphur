#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletCapsuleShape : public sulphur::physics::BulletShape
    * @brief The specialization for capsule shapes.
    * @author Angelo van der Mark
    */
    class BulletCapsuleShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a capsule shape
      */
      BulletCapsuleShape();

      ShapeTypes GetType() const override;

      /**
      * @brief Sets the radius of this capsule shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius);

      /**
      * @brief Returns the radius of this capsule shape.
      * @return (float) The radius of the capsule shape.
      */
      float GetRadius() const;

      /**
      * @brief Sets the height of the capsule shape.
      * @note This takes the radius into account and should not be set lower than the radius.
      * @param[in] height (float) The total height of the capsule.
      */
      void SetHeight(float height);

      /**
      * @brief Returns the total height of this capsule shape.
      * @return (float) The height of the capsule shape.
      */
      float GetHeight() const;
    };
  }
}