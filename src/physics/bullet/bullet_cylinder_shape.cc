#include "physics/bullet/bullet_cylinder_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletCylinderShape::BulletCylinderShape()
    {
      shape_ = foundation::Memory::Construct<btCylinderShape>(
        btVector3(IPhysicsShape::kDefaultExtents,
                  IPhysicsShape::kDefaultExtents*1.5f, // Three times default, as half-extents
                  IPhysicsShape::kDefaultExtents));
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletCylinderShape::~BulletCylinderShape()
    {
      foundation::Memory::Destruct<btCylinderShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletCylinderShape::SetRadius(float radius)
    {
      btVector3 half_extents = btVector3(radius, GetHeight()*0.5f, radius);

      // Mimic btCylinderShape's constructor for setting the correct margin:
      btVector3 margin(shape_->getMargin(), shape_->getMargin(), shape_->getMargin());
      shape_->setImplicitShapeDimensions(half_extents - margin);
      shape_->setSafeMargin(half_extents);
      
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCylinderShape::GetRadius() const
    {
      return shape_->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletCylinderShape::SetHeight(float height)
    {
      btVector3 half_extents = btVector3(GetRadius(), height*0.5f, GetRadius());

      // Mimic btCylinderShape's constructor for setting the correct margin:
      btVector3 margin(shape_->getMargin(), shape_->getMargin(), shape_->getMargin());
      shape_->setImplicitShapeDimensions(half_extents - margin);
      shape_->setSafeMargin(half_extents);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCylinderShape::GetHeight() const
    {
      return shape_->getHalfExtentsWithMargin().y() * 2;
    }

    //-------------------------------------------------------------------------
    void* BulletCylinderShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}