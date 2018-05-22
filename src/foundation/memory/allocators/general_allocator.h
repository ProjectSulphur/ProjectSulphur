#pragma once

#ifdef PS_WIN32
#include "foundation/memory/allocators/malloc_allocator.h"
namespace sulphur 
{
  namespace foundation 
  {
    using GeneralAllocator = MallocAllocator;
  }
}
#elif defined (PS_PS4)
#include "foundation/memory/allocators/free_list_allocator.h"
namespace sulphur 
{
  namespace foundation 
  {
    using GeneralAllocator = FreeListAllocator;
  }
}
#endif