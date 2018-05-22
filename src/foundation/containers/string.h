#pragma once
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include "foundation/memory/allocators/eastl_allocator.h"

namespace sulphur
{
  namespace foundation
  {
    using String = eastl::basic_string<char, EASTLAllocator>;
    template<int Size>
    using FixedString = eastl::fixed_string<char, Size, true, EASTLAllocator>;

    inline String to_string(int value)
    {
      return String(String::CtorSprintf(), "%d", value);
    }
    inline String to_string(long value)
    {
      return String(String::CtorSprintf(), "%ld", value);
    }
    inline String to_string(long long value)
    {
      return String(String::CtorSprintf(), "%lld", value);
    }
    inline String to_string(unsigned value)
    {
      return String(String::CtorSprintf(), "%u", value);
    }
    inline String to_string(unsigned long value)
    {
      return String(String::CtorSprintf(), "%lu", value);
    }
    inline String to_string(unsigned long long value)
    {
      return String(String::CtorSprintf(), "%llu", value);
    }
    inline String to_string(float value)
    {
      return String(String::CtorSprintf(), "%f", value);
    }
    inline String to_string(double value)
    {
      return String(String::CtorSprintf(), "%f", value);
    }
    inline String to_string(long double value)
    {
      return String(String::CtorSprintf(), "%Lf", value);
    }
  }
}

namespace eastl
{
  template<> struct hash<sulphur::foundation::String> : eastl::string_hash<sulphur::foundation::String>
  {
  };
}
