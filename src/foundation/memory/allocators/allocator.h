#pragma once
#include "foundation/auxiliary/pointer_arithmetic.h"

//#define PS_MEMORY_DEBUG

#ifdef PS_MEMORY_DEBUG
#include <vector>
#endif

namespace sulphur 
{
  namespace foundation 
  {
    class Memory;

    /**
    * @class sulphur::foundation::IAllocator
    * @brief The base class for every allocator to inherit from
    * @author Daniel Konings
    */
    class IAllocator
    {

      friend Memory;

    public:

      /**
      * @brief Construct with a maximum size
      * @param[in] max_allocated (size_t) The maximum size that is allowed to be allocated
      */
      IAllocator(size_t max_allocated);

      /**
      * @brief Shutdown this allocator, detects any leaks by asserting
      */
      void Shutdown();

      /**
      * @brief Default destructor
      */
      virtual ~IAllocator();

    protected:

      /**
      * @brief Allocates a block of memory and saves the changes in memory
      * @param[in] size (size_t) The size to allocate
      * @param[in] alignment (size_t) The alignment to align the memory boundaries at
      * @remarks Basically calls sulphur::foundation::IAllocator::Malloc
      * and keeps track of the changes in memory
      */
      void* Allocate(size_t size, size_t alignment);

      /**
      * @brief Deallocate a memory block, allocated by this allocator
      * @param[in] ptr (void*) The memory block to deallocate
      * @remarks Basically calls sulphur::foundation::IAllocator::Free
      * and keeps track of the changes in memory
      */
      void Deallocate(void* ptr);

    private:

      /**
      * @see sulphur::foundation::IAllocator::Allocate
      * @remarks To be implemented by the derived allocator
      */
      virtual void* Malloc(size_t size, size_t alignment) = 0;

      /**
      * @see sulphur::foundation::IAllocator::Deallocate
      * @remarks To be implemented by the derived allocator
      * @return (size_t) The size that was deallocated
      */
      virtual size_t Free(void* ptr) = 0;

      size_t open_allocations_; //!< The number of current open allocations
      size_t allocated_; //!< The total amount of currently allocated memory
      size_t max_allocated_; //!< The maximum memory that is allowed to be allocated
      bool alive_; //!< Is this allocator shutdown yet?

#ifdef PS_MEMORY_DEBUG
      struct DebugAllocationData 
      {
        void* pointer;
        size_t size;
      };

      std::vector<DebugAllocationData> debug_allocation_data_;
#endif
    };
  }
}