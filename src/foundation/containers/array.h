#pragma once
#include <EASTL/array.h>

namespace sulphur
{
  namespace foundation
  {
    template<typename T, size_t Size>
    using Array = eastl::array<T,Size>;
  }
}