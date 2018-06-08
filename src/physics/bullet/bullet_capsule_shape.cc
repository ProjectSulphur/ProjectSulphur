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
      shape_ = foundation::Memory::Construct<btCapsuleShape>(IPhysicsShape::kDefaultExtents,
                                                             IPhysicsShape::kDefaultExtents*3);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletCapsuleShape::~BulletCapsuleShape()
    {
      foundation::Memory::Destruct<btCapsuleShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetRadius(float radius)
    {
      btVector3 half_extents = btVector3(radius, GetHeight()*0.5f, radius);

      shape_->setImplicitShapeDimensions(half_extents);
      // Update m_collisionMargin using this function, because Bullet doesn't provide another way
      // https://www.youtube.com/watch?v=pIgZ7gMze7A
      shape_->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetRadius() const
    {
      return shape_->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletCapsuleShape::SetHeight(float height)
    {
      btVector3 half_extents = btVector3(GetRadius(), height*0.5f, GetRadius());

      shape_->setImplicitShapeDimensions(half_extents);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCapsuleShape::GetHeight() const
    {
      return shape_->getHalfHeight() * 2;
    }

    //-------------------------------------------------------------------------
    void* BulletCapsuleShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}