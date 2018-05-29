#pragma once
#include "physics/bullet/bullet_shape.h"

#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletConvexShape : public sulphur::physics::BulletShape
    * @brief The specialization for convex mesh shapes.
    * @author Angelo van der Mark
    */
    class BulletConvexShape : public BulletShape
    {
    public:

      /**
      * @brief Constructs a convex mesh shape.
      */
      BulletConvexShape();

      /**
      * @see sulphur::physics::PhysicsShape::GetType
      */
      ShapeTypes GetType() const override;

      /**
      * @brief Sets the convex mesh shape using a set of vertices.
      * @param[in] points (const glm::vec3*) Pointer to the vertices of the mesh.
      * @param[in] num_points (unsigned int) The amount of vertices.
      */
      void SetVertices(const glm::vec3* points, unsigned int num_points);
    };
  }
}