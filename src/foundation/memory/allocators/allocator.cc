#include "foundation/memory/allocators/allocator.h"
#include "foundation/memory/memory.h"

#include "foundation/logging/logger.h"

#include <stdio.h>
#include <memory>
#include <assert.h>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    IAllocator::IAllocator(size_t max_allocated) :
      open_allocations_(0),
      allocated_(0),
      max_allocated_(max_allocated),
      alive_(true)
    {

    }

    //--------------------------------------------------------------------------
    void* IAllocator::Allocate(size_t size, size_t alignment)
    {
      bool initialized = Memory::IsInitialized();
      assert(initialized == true);

      if (initialized == false)
      {
        return nullptr;
      }

      if (size == 0)
      {
        return nullptr;
      }

      if (allocated_ + size > max_allocated_)
      {
        printf("Allocating more memory than allowed, allocated: %zu | allocating: %zu | max: %zu\n",
          allocated_,
          size,
          max_allocated_);

        return nullptr;
      }

      ++open_allocations_;
      allocated_ += size;

      void* pointer = Malloc(size, alignment);

#ifdef PS_MEMORY_DEBUG
      debug_allocation_data_.push_back({ pointer, size });
#endif

      return pointer;
    }

    //--------------------------------------------------------------------------
    void IAllocator::Deallocate(void* ptr)
    {
      if (ptr == nullptr)
      {
        return;
      }

      bool initialized = Memory::IsInitialized();
      assert(initialized == true);

      if (initialized == false)
      {
        return;
      }

      assert(open_allocations_ - 1 >= 0);

      --open_allocations_;
      allocated_ -= Free(ptr);

#ifdef PS_MEMORY_DEBUG
      // Find the allocation at the pointer 
      for (size_t i = 0; i < debug_allocation_data_.size(); i++)
      {
        if (debug_allocation_data_[i].pointer == ptr)
        {
          // Set the pointer and size to zero
          debug_allocation_data_[i].pointer = nullptr;
          debug_allocation_data_[i].size = 0;

          break;
        }
      }
#endif
    }

    //--------------------------------------------------------------------------
    void IAllocator::Shutdown()
    {
      if (alive_ == false)
      {
        return;
      }

      // With _DEBUG check debug_allocation_data
#ifdef PS_MEMORY_DEBUG
      for (size_t i = 0; i < debug_allocation_data_.size(); i++)
      {
        
        if (debug_allocation_data_[i].pointer != nullptr)
        {
          // Printf is required since dynamic allocation of EASTL string is not possible in allocator shutdown
          printf("Leak: at 0x%p of size %llu\n", debug_allocation_data_[i].pointer, debug_allocation_data_[i].size);
        }
      }
#endif

      /* Memory debugging notes
      * sizeof(foundation::Vector) == 42
      *
      */

      assert(open_allocations_ == 0);
      assert(allocated_ == 0);

      printf("[Allocator] No memory leaks detected\n");
      alive_ = false;
    }

    //--------------------------------------------------------------------------
    IAllocator::~IAllocator()
    {
      Shutdown();
    }
  }
}