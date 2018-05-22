#pragma once
#include <EASTL/intrusive_list.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    using IntrusiveList = eastl::intrusive_list<eastl::intrusive_list_node>;
  }
}