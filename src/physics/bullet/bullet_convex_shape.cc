#include "physics/bullet/bullet_convex_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

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
    BulletConvexShape::~BulletConvexShape()
    {
      foundation::Memory::Destruct<btConvexHullShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletConvexShape::SetVertices(const glm::vec3* points, unsigned int num_points)
    {      
      // Deconstruct the shape and construct again using placement new (no re-allocation)
      shape_->~btConvexHullShape();
      new(shape_) btConvexHullShape(reinterpret_cast<const btScalar*>(points),
                                    num_points, sizeof(glm::vec3));
      shape_->setUserPointer(this);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    void* BulletConvexShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}