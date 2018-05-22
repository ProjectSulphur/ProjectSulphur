#pragma once
#include <EASTL/set.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using Set = eastl::set<T, eastl::less<T>, EASTLAllocator>;
    template<typename T>
    using MultiSet = eastl::multiset<T>;
  }
}