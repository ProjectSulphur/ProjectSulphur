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
      shape_ = foundation::Memory::Construct<btSphereShape>(PhysicsShape::kDefaultExtents);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletSphereShape::GetType() const
    {
      return ShapeTypes::kSphere;
    }

    //-------------------------------------------------------------------------
    void BulletSphereShape::SetRadius(float radius)
    {
      reinterpret_cast<btSphereShape*>(shape_)->setUnscaledRadius(radius);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletSphereShape::GetRadius() const
    {
      return reinterpret_cast<btSphereShape*>(shape_)->getRadius();
    }
  }
}