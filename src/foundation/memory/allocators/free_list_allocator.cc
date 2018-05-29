#include "foundation/memory/allocators/free_list_allocator.h"
#include "foundation/auxiliary/pointer_arithmetic.h"

#include <EASTL/utility.h>

#include <inttypes.h>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    FreeListAllocator::FreeListAllocator() :
      IAllocator(0)
    {

    }

    //--------------------------------------------------------------------------
    FreeListAllocator::FreeListAllocator(void* buffer, size_t size) :
      IAllocator(size),
      free_block_(nullptr)
    {
      free_block_ = reinterpret_cast<FreeBlock*>(buffer);
      free_block_->size = size;
      free_block_->next = nullptr;
    }

    //--------------------------------------------------------------------------
    FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& other)
    {
      IAllocator::operator=(eastl::move(other));
      free_block_ = other.free_block_;
      other.free_block_ = nullptr;
      return *this;
    }

    //--------------------------------------------------------------------------
    void* FreeListAllocator::Malloc(size_t size, size_t alignment)
    {
      FreeBlock* prev = nullptr;
      FreeBlock* cur = free_block_;
      while (cur != nullptr)
      {
        // Calculate adjustment needed to keep object correctly aligned
        size_t adjustment = AlignUpDeltaWithOffset(cur, alignment, sizeof(Header));

        size_t total_size = size + adjustment;

        // If allocation doesn't fit in this FreeBlock, try the next
        if (cur->size < total_size)
        {
          prev = cur;
          cur = cur->next;
          continue;
        }

        static_assert(sizeof(Header) >= sizeof(FreeBlock), "sizeof(Header) < sizeof(FreeBlock)");

        // If allocations in the remaining memory will be impossible
        if (cur->size - total_size <= sizeof(Header))
        {
          // Increase allocation size instead of creating a new FreeBlock
          total_size = cur->size;

          if (prev != nullptr)
          {
            prev->next = cur->next;
          }
          else
          {
            free_block_ = cur->next;
          }
        }
        else
        {
          // Else create a new FreeBlock containing remaining memory
          FreeBlock* next_block = reinterpret_cast<FreeBlock*>(OffsetBytes(cur, static_cast<intptr_t>(total_size)));
          next_block->size = cur->size - total_size;
          next_block->next = cur->next;

          if (prev != nullptr)
          {
            prev->next = next_block;
          }
          else
          {
            free_block_ = next_block;
          }
        }

        uintptr_t aligned_address = reinterpret_cast<uintptr_t>(cur) + adjustment;

        Header* header = reinterpret_cast<Header*>(aligned_address - sizeof(Header));
        header->block_size = total_size;
        header->offset = adjustment;

        return reinterpret_cast<void*>(aligned_address);
      }
      // Crash?
      return nullptr;
    }

    //--------------------------------------------------------------------------
    size_t FreeListAllocator::Free(void* ptr)
    {
      Header* header = reinterpret_cast<Header*>(OffsetBytes(ptr, -static_cast<intptr_t>(sizeof(Header))));

      size_t total_size = header->block_size - header->offset;

      size_t block_start = (reinterpret_cast<intptr_t>(ptr)) - header->offset;
      size_t block_size = header->block_size;
      size_t block_end = block_start + block_size;

      FreeBlock* prev = nullptr;
      FreeBlock* cur = free_block_;

      // Get the free block that's after us
      while (cur != nullptr)
      {
        if (reinterpret_cast<uintptr_t>(cur) >= block_end)
        {
          break;
        }

        prev = cur;
        cur = cur->next;
      }
      // The first free block was already past us so we become the first free one
      if (prev == nullptr)
      {
        prev = reinterpret_cast<FreeBlock*>(block_start);
        prev->size = block_size;
        prev->next = free_block_;

        free_block_ = prev;
      }
      else if (reinterpret_cast<uintptr_t>(prev) + prev->size == block_start)// Merge with the last
      {
        prev->size += block_size;
      }
      else// Create a new one and insert it in the list
      {
        FreeBlock* temp = reinterpret_cast<FreeBlock*>(block_start);
        temp->size = block_size;
        temp->next = prev->next;
        prev->next = temp;

        prev = temp;
      }
      // Merge this one with the next if they connect
      if (cur != nullptr && reinterpret_cast<uintptr_t>(cur) == block_end)
      {
        prev->size += cur->size;
        prev->next = cur->next;
      }

      return total_size;
    }

    //--------------------------------------------------------------------------
    FreeListAllocator::Header::Header() :
      block_size(0),
      offset(0)
    {

    }
  }
}