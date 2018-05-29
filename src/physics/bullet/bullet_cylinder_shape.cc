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
        btVector3(PhysicsShape::kDefaultExtents,
                  PhysicsShape::kDefaultExtents*1.5f, // Three times default, as half-extents
                  PhysicsShape::kDefaultExtents));
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletCylinderShape::GetType() const
    {
      return ShapeTypes::kCylinder;
    }

    //-------------------------------------------------------------------------
    void BulletCylinderShape::SetRadius(float radius)
    {
      btVector3 half_extents = btVector3(radius, GetHeight()*0.5f, radius);

      reinterpret_cast<btCylinderShape*>(shape_)->setImplicitShapeDimensions(half_extents);
      
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCylinderShape::GetRadius() const
    {
      return reinterpret_cast<btCylinderShape*>(shape_)->getRadius();
    }

    //-------------------------------------------------------------------------
    void BulletCylinderShape::SetHeight(float height)
    {
      btVector3 half_extents = btVector3(GetRadius(), height*0.5f, GetRadius());

      reinterpret_cast<btCylinderShape*>(shape_)->setImplicitShapeDimensions(half_extents);

      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    float BulletCylinderShape::GetHeight() const
    {
      return reinterpret_cast<btCylinderShape*>(shape_)->getHalfExtentsWithMargin().y() * 2;
    }
  }
}