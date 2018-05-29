#pragma once

#include "foundation/utils/type_definitions.h"
#include <type_traits>

namespace sulphur
{
  namespace foundation
  {
    using typeid_t = size_t;

    inline typeid_t unique_val()
    {
      static typeid_t cur = 0;
      return cur++;
    }

    template <typename T>
    typeid_t type_id()
    {
      static typeid_t const x = unique_val();
      return x;
    }
  }
}
