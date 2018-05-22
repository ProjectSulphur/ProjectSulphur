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
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletCapsuleShape::GetType() const
    {
      return ShapeTypes::kCapsule;
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetRadius(float radius)
    {
      // Bullet doesn't allow modifying capsules at run-time, 
      // but we're doing it anyway by recreating the shape.

      float height = GetHeight();
      PhysicsBody* body = attached_body_;

      AttachToBody(nullptr);

      foundation::Memory::Destruct<btCollisionShape>(shape_);
      shape_ = foundation::Memory::Construct<btCapsuleShape>(radius, height);

      AttachToBody(body);
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetRadius() const
    {
      return reinterpret_cast<btCapsuleShape*>(shape_)->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetHeight(float height)
    {
      // Bullet doesn't allow modifying capsules at run-time, 
      // but we're doing it anyway by recreating the shape.

      float radius = GetRadius();
      PhysicsBody* body = attached_body_;

      AttachToBody(nullptr);

      foundation::Memory::Destruct<btCollisionShape>(shape_);
      shape_ = foundation::Memory::Construct<btCapsuleShape>(radius, height);

      AttachToBody(body);
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetHeight() const
    {
      return reinterpret_cast<btCapsuleShape*>(shape_)->getHalfHeight() * 2;
    }
  }
}