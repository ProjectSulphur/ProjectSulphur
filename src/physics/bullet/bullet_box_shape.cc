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
      glm::vec3 default_extents(IPhysicsShape::kDefaultExtents,
                                IPhysicsShape::kDefaultExtents,
                                IPhysicsShape::kDefaultExtents);
      shape_ = foundation::Memory::Construct<btBoxShape>(
        BulletConversions::ToBt(default_extents));
      shape_->setUserPointer(this);
    }

    //-------------------------------------------------------------------------
    BulletBoxShape::~BulletBoxShape()
    {
      foundation::Memory::Destruct<btBoxShape>(shape_);
    }

    //-------------------------------------------------------------------------
    void BulletBoxShape::SetExtents(const glm::vec3& extents)
    {
      shape_->setImplicitShapeDimensions(BulletConversions::ToBt(extents));
      UpdateColliders();
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBoxShape::GetExtents() const
    {
      return BulletConversions::ToGlm(shape_->getHalfExtentsWithoutMargin());
    }

    //-------------------------------------------------------------------------
    void* BulletBoxShape::GetInternalShape() const
    {
      return shape_;
    }
  }
}