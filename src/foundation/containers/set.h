#pragma once
#include <EASTL/set.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T, typename Compare = eastl::less<T>>
    using Set = eastl::set<T, Compare, EASTLAllocator>;
    template<typename T>
    using MultiSet = eastl::multiset<T>;
  }
}