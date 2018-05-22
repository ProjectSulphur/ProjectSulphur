#pragma once
#include <EASTL/list.h>
#include <EASTL/fixed_list.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using List = eastl::list<T, EASTLAllocator>;
    template<typename T, size_t Size>
    using FixedList = eastl::fixed_list<T, Size, true, EASTLAllocator>;
  }
}