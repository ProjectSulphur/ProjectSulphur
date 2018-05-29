//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 

#pragma once

#include <inttypes.h>
#include <intrin.h>

// This requires SSE4.2 which is present on Intel Nehalem (Nov. 2008)
// and AMD Bulldozer (Oct. 2011) processors.  I could put a runtime
// check for this, but I'm just going to assume people playing with
// DirectX 12 on Windows 10 have fairly recent machines.
#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif

namespace sulphur
{
  namespace graphics
  {
    template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
    {
      return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
    {
      return (T)((size_t)value & ~mask);
    }

    template <typename T> __forceinline T AlignUp(T value, size_t alignment)
    {
      return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T> __forceinline T AlignDown(T value, size_t alignment)
    {
      return AlignDownWithMask(value, alignment - 1);
    }

    inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
    {
#if ENABLE_SSE_CRC32
      const uint64_t* Iter64 = (const uint64_t*)AlignUp(Begin, 8);
      const uint64_t* const End64 = (const uint64_t* const)AlignDown(End, 8);

      // If not 64-bit aligned, start with a single u32
      if ((uint32_t*)Iter64 > Begin)
        Hash = _mm_crc32_u32((uint32_t)Hash, *Begin);

      // Iterate over consecutive u64 values
      while (Iter64 < End64)
        Hash = _mm_crc32_u64((uint64_t)Hash, *Iter64++);

      // If there is a 32-bit remainder, accumulate that
      if ((uint32_t*)Iter64 < End)
        Hash = _mm_crc32_u32((uint32_t)Hash, *(uint32_t*)Iter64);
#else
      // An inexpensive hash for CPUs lacking SSE4.2
      for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
        Hash = 16777619U * Hash ^ *Iter;
#endif

      return Hash;
    }

    template <typename T> inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
    {
      static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
      return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
    }

    inline void hash_combine(std::size_t& /*seed*/) {}

    template <typename T, typename... Rest>
    inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
    {
      std::hash<T> hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      hash_combine(seed, rest...);
    }

  } // namespace Utility
}