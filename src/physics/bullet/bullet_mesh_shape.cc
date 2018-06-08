#include "physics/bullet/bullet_mesh_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletMeshShape::BulletMeshShape()
    {
      mesh_ = foundation::Memory::Construct<btTriangleMesh>(true, false);

      //Add simple unit bounded plane, else Bullet complains.
      mesh_->addTriangle(btVector3(-.5f, .0f, -.5f),
        btVector3(-.5f, .0f, .5f),
        btVector3(.5f, .0f, .5f));
      mesh_->addTriangle(btVector3(-.5f, .0f, -.5f),
        btVector3(.5f, .0f, .5f),
        btVector3(.5f, .0f, -.5f));

      shape_ = foundation::Memory::Construct<btBvhTriangleMeshShape>(mesh_, true);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletMeshShape::~BulletMeshShape()
    {
      foundation::Memory::Destruct<btBvhTriangleMeshShape>(shape_);
      foundation::Memory::Destruct<btTriangleMesh>(mesh_);
    }

    //-------------------------------------------------------------------------
    void BulletMeshShape::SetMesh(const foundation::Vector<glm::vec3>& vertices,
      const foundation::Vector<uint32_t>& indices)
    {
      // Deconstruct the shape and construct again using placement new (no re-allocation)
      shape_->~btBvhTriangleMeshShape();
      mesh_->~btTriangleMesh();

      new(mesh_) btTriangleMesh(true, false);

      mesh_->preallocateIndices(indices.size());
      mesh_->preallocateVertices(vertices.size());
      
      for (size_t i = 0; i < indices.size()/3; ++i)
      {
        btVector3 v0 = BulletConversions::ToBt(vertices[indices[i * 3 + 0]]);
        btVector3 v1 = BulletConversions::ToBt(vertices[indices[i * 3 + 1]]);
        btVector3 v2 = BulletConversions::ToBt(vertices[indices[i * 3 + 2]]);
        mesh_->addTriangle(v0, v1, v2, true);
      }

      new(shape_) btBvhTriangleMeshShape(mesh_, true);
      shape_->setUserPointer(this);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    void* BulletMeshShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}