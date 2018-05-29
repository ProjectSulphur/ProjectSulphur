#include "physics/bullet/bullet_convex_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletConvexShape::BulletConvexShape()
    {
      shape_ = foundation::Memory::Construct<btConvexHullShape>();
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletConvexShape::GetType() const
    {
      return ShapeTypes::kConvex;
    }

    //-------------------------------------------------------------------------
    void BulletConvexShape::SetVertices(const glm::vec3* points, unsigned int num_points)
    {      
      // Deconstruct the shape and construct again using placement new (no re-allocation)
      shape_->~btCollisionShape();
      new(shape_) btConvexHullShape(reinterpret_cast<const btScalar*>(points),
                                    num_points, sizeof(glm::vec3));
      shape_->setUserPointer(this);

      UpdateColliders();
    }
  }
}