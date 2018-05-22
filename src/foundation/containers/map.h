#pragma once
#include <EASTL/map.h>
#include <EASTL/fixed_map.h>
#include <EASTL/vector_map.h>
#include <EASTL/vector_multimap.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename Key, typename T>
    using Map = eastl::map<Key, T, eastl::less<Key>, EASTLAllocator>;
    template<typename Key, typename T, size_t Size>
    using FixedMap = eastl::fixed_map<Key, T, Size, true, eastl::less<Key>, EASTLAllocator>;
    template<typename Key, typename T>
    using MultiMap = eastl::multimap<Key, T, eastl::less<Key>, EASTLAllocator>;

    template<typename Key, typename T, size_t Size, bool bEnableOverflow = true, typename Compare = eastl::less<Key>>
    using FixedMultiMap = eastl::fixed_multimap<Key, T, Size, bEnableOverflow, Compare, EASTLAllocator>;

    template<typename Key, typename T, typename Compare = eastl::less<Key>, 
      typename RandomAccessContainer = eastl::vector<eastl::pair<Key, T>, EASTLAllocator>>
    using VectorMap = eastl::vector_map<Key, T, Compare, EASTLAllocator, RandomAccessContainer>;

    template<typename Key, typename T, typename Compare = eastl::less<Key>,
      typename RandomAccessContainer = eastl::vector<eastl::pair<Key, T>, EASTLAllocator>>
    using VectorMultiMap = eastl::vector_multimap<Key, T, Compare, EASTLAllocator, RandomAccessContainer>;
  }
}