#pragma once
#include <EASTL/bitset.h>

namespace sulphur
{
  namespace foundation
  {
    template<size_t Size>
    using BitSet = eastl::bitset<Size>;
  }
}