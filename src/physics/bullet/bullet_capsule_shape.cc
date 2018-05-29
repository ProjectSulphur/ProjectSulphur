#include "physics/bullet/bullet_capsule_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletCapsuleShape::BulletCapsuleShape()
    {
      shape_ = foundation::Memory::Construct<btCapsuleShape>(PhysicsShape::kDefaultExtents,
                                                             PhysicsShape::kDefaultExtents*3);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletCapsuleShape::GetType() const
    {
      return ShapeTypes::kCapsule;
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetRadius(float radius)
    {
      btVector3 half_extents = btVector3(radius, GetHeight()*0.5f, radius);

      reinterpret_cast<btCapsuleShape*>(shape_)->setImplicitShapeDimensions(half_extents);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetRadius() const
    {
      return reinterpret_cast<btCapsuleShape*>(shape_)->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetHeight(float height)
    {
      btVector3 half_extents = btVector3(GetRadius(), height*0.5f, GetRadius());

      reinterpret_cast<btCapsuleShape*>(shape_)->setImplicitShapeDimensions(half_extents);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetHeight() const
    {
      return reinterpret_cast<btCapsuleShape*>(shape_)->getHalfHeight() * 2;
    }
  }
}