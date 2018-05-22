#pragma once
#include <EASTL/hash_map.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/string_hash_map.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename Key, typename T>
    using HashMap = eastl::hash_map<Key, T, eastl::hash<Key>, eastl::equal_to<Key>, EASTLAllocator>;
    template<typename Key, typename T>
    using HashMultiMap = eastl::hash_multimap<Key, T, eastl::hash<Key>, eastl::equal_to<Key>, EASTLAllocator>;
    template<typename Key, typename T, size_t Size>
    using FixedHashMap = eastl::fixed_hash_map<Key, T, Size, Size + 1, true, eastl::hash<Key>, eastl::equal_to<Key>, false, EASTLAllocator>;
    template<typename Key, typename T, size_t Size>
    using FixedHashMultiMap = eastl::fixed_hash_multimap<Key, T, Size, Size + 1, true, eastl::hash<Key>, eastl::equal_to<Key>, false, EASTLAllocator>;
    template<typename T>
    using StringHashMap = eastl::string_hash_map<T, eastl::hash<eastl::string>, eastl::equal_to<eastl::string>, EASTLAllocator>;
  }
}