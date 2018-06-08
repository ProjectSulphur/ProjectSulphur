#pragma once
#include <EASTL/queue.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using Queue = eastl::queue<T, eastl::deque<T, EASTLAllocator>>;
  }
}