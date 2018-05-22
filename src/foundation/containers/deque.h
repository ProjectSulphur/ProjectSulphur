#pragma once
#include <EASTL/deque.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using Deque = eastl::deque<T, EASTLAllocator>;
  }
}