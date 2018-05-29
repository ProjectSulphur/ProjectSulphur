#include "foundation/memory/allocators/malloc_allocator.h"

#include <EASTL/utility.h>

#include <memory>

#ifdef PS_WIN32
#define PS_MALLOC(x, y) _aligned_malloc(x, y)
#define PS_FREE(x) _aligned_free(x)
#elif defined (PS_PS4)
#define PS_MALLOC(x, y) malloc(x)
#define PS_FREE(x) free(x)
#endif

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    MallocAllocator::MallocAllocator() :
      IAllocator(0)
    {

    }

    //--------------------------------------------------------------------------
    MallocAllocator::MallocAllocator(void* /*buffer*/, size_t size) :
      IAllocator(size)
    {

    }

    //--------------------------------------------------------------------------
    MallocAllocator& MallocAllocator::operator=(MallocAllocator&& other)
    {
      IAllocator::operator=(eastl::move(other));
      return *this;
    }

    //--------------------------------------------------------------------------
    void* MallocAllocator::Malloc(size_t size, size_t alignment)
    {
      size_t header_size = sizeof(Header);

      void* base = PS_MALLOC(size + header_size + alignment - 1, alignment);
      
      void* ptr = OffsetBytes(base, header_size);
      size_t a = AlignUpDelta(ptr, alignment);

      ptr = OffsetBytes(ptr, a);

      Header* header = reinterpret_cast<Header*>(OffsetBytes(base, a));
      header->size = size;
      header->alignment = a;

      return ptr;
    }

    //--------------------------------------------------------------------------
    size_t MallocAllocator::Free(void* ptr)
    {
      Header* header;
      header = reinterpret_cast<Header*>(
        reinterpret_cast<intptr_t>(ptr) - sizeof(Header));

      size_t size = header->size;
      size_t align = header->alignment;

      PS_FREE(OffsetBytes(header, -intptr_t(align)));

      return size;
    }
  }
}