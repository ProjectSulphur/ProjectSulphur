#pragma once

#include "physics/iphysics_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsBoxShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for box shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class IPhysicsBoxShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kBox; }

      /**
      * @brief Sets the extents of this box shape.
      * @param[in] extents (const glm::vec3&) The extents to set.
      */
      virtual void SetExtents(const glm::vec3& extents) = 0;

      /**
      * @brief Returns the extents of this box shape.
      * @return (glm::vec3) The extents of the box shape.
      */
      virtual glm::vec3 GetExtents() const = 0;
    };
  }
}