#pragma once
#include <EASTL/intrusive_hash_map.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename Key, typename T, size_t Size>
    using IntrusiveHashMap = eastl::intrusive_hash_map<Key, T, Size>;
    template<typename Key, typename T, size_t Size>
    using IntrusiveHashMultiMap = eastl::intrusive_hash_multimap<Key, T, Size>;
  }
}