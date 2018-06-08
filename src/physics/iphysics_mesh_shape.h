#pragma once

#include "physics/iphysics_shape.h"

#include <foundation/containers/vector.h>
#include <glm/glm.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsMeshShape : public sulphur::physics::IPhysicsShape
    * @brief The interface for concave mesh shapes.
    * @author Angelo van der Mark
    */
    class IPhysicsMeshShape : public IPhysicsShape
    {
    public:

      /**
      * @see sulphur::physics::IPhysicsShape::GetType
      */
      ShapeTypes GetType() const override { return IPhysicsShape::ShapeTypes::kMesh; }

      /**
      * @brief Sets the mesh shape using a set of vertices and indices.
      * @param[in] vertices (const sulphur::foundation::Vector<glm::vec3>&) Array of vertices of the mesh.
      * @param[in] indices (const sulphur::foundation::Vector<uint32_t>&) Indices of the mesh, each set of 3 defining one triangle.
      */
      virtual void SetMesh(const foundation::Vector<glm::vec3>& vertices,
                           const foundation::Vector<uint32_t>& indices) = 0;
    };
  }
}