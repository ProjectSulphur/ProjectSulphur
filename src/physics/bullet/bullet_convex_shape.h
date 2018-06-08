#pragma once

#include "physics/iphysics_convex_shape.h"

class btConvexHullShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletConvexShape : public sulphur::physics::IPhysicsConvexShape
    * @brief The specialization for convex mesh shapes.
    * @author Angelo van der Mark
    */
    class BulletConvexShape : public IPhysicsConvexShape
    {
    public:

      /**
      * @brief Constructs a convex mesh shape.
      */
      BulletConvexShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletConvexShape();

      /**
      * @brief Sets the convex mesh shape using a set of vertices.
      * @param[in] points (const glm::vec3*) Pointer to the vertices of the mesh.
      * @param[in] num_points (unsigned int) The amount of vertices.
      */
      void SetVertices(const glm::vec3* points, unsigned int num_points) override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btConvexHullShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}