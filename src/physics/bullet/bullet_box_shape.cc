#include "physics/bullet/bullet_box_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletBoxShape::BulletBoxShape()
    {
      glm::vec3 default_extents(PhysicsShape::kDefaultExtents,
        PhysicsShape::kDefaultExtents,
        PhysicsShape::kDefaultExtents);
      shape_ = foundation::Memory::Construct<btBoxShape>(
        BulletConversions::ToBt(default_extents));
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    PhysicsShape::ShapeTypes BulletBoxShape::GetType() const
    {
      return ShapeTypes::kBox;
    }

    //-------------------------------------------------------------------------
    void BulletBoxShape::SetExtents(
      const glm::vec3& extents)
    {
      reinterpret_cast<btBoxShape*>(shape_)->setImplicitShapeDimensions(
        BulletConversions::ToBt(extents));
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBoxShape::GetExtents() const
    {
      return BulletConversions::ToGlm(
        reinterpret_cast<btBoxShape*>(shape_)->getHalfExtentsWithoutMargin());
    }
  }
}