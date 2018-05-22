#pragma once

#include <stddef.h>

#ifdef PS_PS4
using ps_intptr = __intptr_t;
#else
using ps_intptr = intptr_t;
#endif

inline void* OffsetBytes(const void* adr, ps_intptr offset)
{
  return (void*)(((size_t)adr) + offset);
}
inline size_t AlignUp(const void* adr, size_t align)
{
  return (((size_t)(adr)+(size_t)(align - 1)) & (~(align - 1)));
}
inline size_t AlignUpDelta(const void* adr, size_t align)
{
  size_t delta = align - (((size_t)(adr)) & ((size_t)(align - 1)));
  if (delta == align)
  {
    return 0;
  }
  return delta;
}
inline size_t AlignUpDeltaWithOffset(const void* adr, size_t allign, size_t offset)
{
  size_t adjustment = AlignUpDelta(adr, allign);
  size_t extra_space = offset;

  if (adjustment < extra_space)
  {
    extra_space -= adjustment;

    adjustment += allign * (extra_space / allign);

    if (extra_space % allign > 0)
    {
      adjustment += allign;
    }
  }

  return adjustment;
}