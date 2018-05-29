#pragma once

#include "foundation/memory/allocators/allocator.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @class sulphur::foundation::FreeListAllocator : public sulplhur::foundation::IAllocator
    * @brief An allocator that simply uses the algorithm as listed on
    * https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/
    * @author Daniel Konings
    */
    class FreeListAllocator : public IAllocator
    {

    public:

      /**
      * @brief Default constructor
      */
      FreeListAllocator();

      /**
      * @brief Constructs the FreeListAllocator from a buffer
      * @param[in] buffer (void*) The pointer to the start of the memory block this allocator uses
      * @param[in] size (size_t) The size of the memory block
      */
      FreeListAllocator(void* buffer, size_t size);

      /**
      * @brief Move assignment operator.
      */
      FreeListAllocator& operator=(FreeListAllocator&& other);
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
      * @struct sulphur::foundation::FreeListAllocator::Header
      * @brief The memory header for every free list block
      * @author Daniel Konings
      */
      struct Header
      {
        /**
        * @brief Default constructor
        * @remarks Sets block_size and offset to 0
        */
        Header();

        size_t block_size; //!< The size of the following block
        size_t offset; //!< The offset of the following block
      };

      /**
      * @struct sulphur::foundation::FreeBlock
      * @brief A linked list structure to walk through the free list structure
      * @author Daniel Konings
      */
      struct FreeBlock
      {
        size_t size; //!< The size of the free block
        FreeBlock* next; //!< The next free block after this block
      };

      FreeBlock* free_block_; //!< The current free block we're at
    };
  }
}