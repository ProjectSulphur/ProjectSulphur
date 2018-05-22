#pragma once
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using Vector = eastl::vector<T, EASTLAllocator>;
    template<typename T, size_t Size>
    using FixedVector = eastl::fixed_vector<T, Size, true, EASTLAllocator>;
  }
}