#include "physics/bullet/bullet_sphere_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletSphereShape::BulletSphereShape()
    {
      shape_ = foundation::Memory::Construct<btSphereShape>(IPhysicsShape::kDefaultExtents);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletSphereShape::~BulletSphereShape()
    {
      foundation::Memory::Destruct<btSphereShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletSphereShape::SetRadius(float radius)
    {
      shape_->setUnscaledRadius(radius);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletSphereShape::GetRadius() const
    {
      return shape_->getRadius();
    }

    //-------------------------------------------------------------------------
    void* BulletSphereShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}