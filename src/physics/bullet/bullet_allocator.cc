#include "physics/bullet/bullet_allocator.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    void* BulletAllocator::Allocate(size_t size)
    {
      return foundation::Memory::Allocate(size);
    }

    //-------------------------------------------------------------------------
    void BulletAllocator::Deallocate(void* ptr)
    {
      foundation::Memory::Deallocate(ptr);
    }
  }
}