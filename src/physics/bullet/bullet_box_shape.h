#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletBoxShape : public sulphur::physics::BulletShape
    * @brief The specialization for box shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class BulletBoxShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a box shape.
      */
      BulletBoxShape();

      /**
      * @see sulphur::physics::PhysicsShape::GetType
      */
      ShapeTypes GetType() const override;

      /**
      * @brief Sets the extents of this box shape.
      * @param[in] extents (const glm::vec3&) The extents to set.
      */
      void SetExtents(const glm::vec3& extents);

      /**
      * @brief Returns the extents of this box shape.
      * @return (glm::vec3) The extents of the box shape.
      */
      glm::vec3 GetExtents() const;
    };
  }
}