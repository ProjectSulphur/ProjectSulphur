#include "physics/bullet/bullet_shape.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>


namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletShape::BulletShape() :
      shape_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    BulletShape::~BulletShape()
    {
      if (shape_ != nullptr)
      {
        foundation::Memory::Destruct<btCollisionShape>(shape_);
        shape_ = nullptr;
      }
    }

    //-------------------------------------------------------------------------
    void* BulletShape::GetInternalShape() const
    {
      return reinterpret_cast<void*>(shape_);
    }
  }
}