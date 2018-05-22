#pragma once

#include <cstddef>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletAllocator
    * @brief Used to wrap memory allocation of Bullet to go through our own custom allocators
    * @author Daniel Konings
    */
    class BulletAllocator
    {

    public:

      /**
      * @see http://bulletphysics.org/Bullet/BulletFull/btAlignedAllocator_8h.html#ac4c1af1ea3741655565171b50dc7cf13
      */
      static void* Allocate(size_t size);

      /**
      * @see http://bulletphysics.org/Bullet/BulletFull/btAlignedAllocator_8h.html#ac4c1af1ea3741655565171b50dc7cf13
      */
      static void Deallocate(void* ptr);
    };
  }
}