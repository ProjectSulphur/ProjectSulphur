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
      shape_ = foundation::Memory::Construct<btConeShape>(IPhysicsShape::kDefaultExtents,
                                                          IPhysicsShape::kDefaultExtents * 3);
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletConeShape::~BulletConeShape()
    {
      foundation::Memory::Destruct<btConeShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletConeShape::SetRadius(float radius)
    {
      shape_->setRadius(radius);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletConeShape::GetRadius() const
    {
      return shape_->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletConeShape::SetHeight(float height)
    {
      shape_->setHeight(height);
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletConeShape::GetHeight() const
    {
      return shape_->getHeight();
    }

    //-------------------------------------------------------------------------
    void* BulletConeShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}