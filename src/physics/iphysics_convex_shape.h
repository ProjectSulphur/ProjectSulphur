#pragma once

#include "physics/iphysics_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsConvexShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for convex mesh shapes.
    * @author Angelo van der Mark
    */
    class IPhysicsConvexShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kConvex; }

      /**
      * @brief Sets the convex mesh shape using a set of vertices.
      * @param[in] points (const glm::vec3*) Pointer to the vertices of the mesh.
      * @param[in] num_points (unsigned int) The amount of vertices.
      */
      virtual void SetVertices(const glm::vec3* points, unsigned int num_points) = 0;
    };
  }
}