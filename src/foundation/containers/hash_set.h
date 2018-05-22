#pragma once
#include <EASTL/hash_set.h>
#include <EASTL/fixed_hash_set.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using HashSet = eastl::hash_set<T,eastl::hash<T>, eastl::equal_to<T> , EASTLAllocator>;
    template<typename T, size_t Size>
    using FixedHashSet = eastl::fixed_hash_set<T, Size, true, EASTLAllocator>;

  }
}