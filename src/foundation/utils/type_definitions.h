#pragma once

#include <cinttypes>

using byte = uint8_t;
using ushort = uint16_t;
using uint = uint32_t;
using ulong = uint64_t;

#ifdef PS_PS4
using size = __size_t;
#else
using size = size_t;
#endif

#define PS_BYTE_MAX   0xFF
#define PS_USHORT_MAX 0xFFFF
#define PS_UINT_MAX   0xFFFFFFFF
#define PS_ULONG_MAX  0xFFFFFFFF
#define PS_SIZE_MAX   0xFFFFFFFFFFFFFFFF