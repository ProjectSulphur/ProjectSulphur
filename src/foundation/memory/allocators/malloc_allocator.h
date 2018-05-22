#pragma once

#include "foundation/memory/allocators/allocator.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @class sulphur::foundation::MallocAllocator : public sulplhur::foundation::IAllocator
    * @brief An allocator that simply uses "malloc" and "free", used on PC as default allocator
    * @author Daniel Konings
    */
    class MallocAllocator : public IAllocator
    {

    public:

      /**
      * @brief Default constructor
      */
      MallocAllocator();

      /**
      * @brief Create the allocator following the rest of the allocators' interface
      * @param[in] buffer (void*) The start address of a memory block, unused
      * @param[in] size (size_t) The size of this allocator
      */
      MallocAllocator(void* buffer, size_t size);

    private:

      /**
      * @see sulphur::foundation::IAllocator::Malloc
      */
      virtual void* Malloc(size_t size, size_t alignment) override;

      /**
      * @see sulphur::foundation::IAllocator::Free
      */
      virtual size_t Free(void* ptr) override;

      /**
      * @struct sulphur::foundation::Header
      * @brief A memory header to keep administration of an allocation
      * @author Daniel Konings
      */
      struct Header
      {
        size_t size; //!< The size of the allocation
        size_t alignment; //!< The alignment of the allocation
      };
    };
  }
}