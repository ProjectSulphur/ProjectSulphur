#pragma once
#include <EASTL/slist.h>
#include <EASTL/fixed_slist.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using ForwardList = eastl::slist<T,EASTLAllocator>;
    template<typename T, size_t Size>
    using FixedForwardList = eastl::fixed_slist<T, Size, true, EASTLAllocator>;
  }
}