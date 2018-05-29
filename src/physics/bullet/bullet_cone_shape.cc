#include "physics/bullet/bullet_cone_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletConeShape::BulletConeShape()
    {
      shape_ = foundation::Memory::Construct<btConeShape>(PhysicsShape::kDefaultExtents,
                                                          PhysicsShape::kDefaultExtents * 3);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletConeShape::GetType() const
    {
      return ShapeTypes::kCone;
    }

    //-------------------------------------------------------------------------
    void BulletConeShape::SetRadius(float radius)
    {
      reinterpret_cast<btConeShape*>(shape_)->setRadius(radius);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletConeShape::GetRadius() const
    {
      return reinterpret_cast<btConeShape*>(shape_)->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletConeShape::SetHeight(float height)
    {
      reinterpret_cast<btConeShape*>(shape_)->setHeight(height);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletConeShape::GetHeight() const
    {
      return reinterpret_cast<btConeShape*>(shape_)->getHeight();
    }
  }
}