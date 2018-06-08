#pragma once

#include "physics/iphysics_mesh_shape.h"

class btBvhTriangleMeshShape;
class btTriangleMesh;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletMeshShape : public sulphur::physics::IPhysicsMeshShape
    * @brief The specialization for concave mesh shapes.
    * @remarks This collision shape is only supported as a static collider.
    * @author Angelo van der Mark
    */
    class BulletMeshShape : public IPhysicsMeshShape
    {
    public:

      /**
      * @brief Constructs a mesh shape.
      */
      BulletMeshShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletMeshShape();

      /**
      * @see sulphur::physics::IPhysicsMeshShape::SetMesh
      */
      void SetMesh(const foundation::Vector<glm::vec3>& vertices,
        const foundation::Vector<uint32_t>& indices) override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btBvhTriangleMeshShape* shape_; //!< Implementation-specific shape instance.
      btTriangleMesh* mesh_; //!< Pointer to the mesh data.
    };
  }
}