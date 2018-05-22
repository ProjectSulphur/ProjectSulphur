#pragma once

#ifdef PS_WIN32
#include "foundation/win32/win32_native.h"
namespace sulphur 
{
  namespace foundation 
  {
    using Native = Win32Native;
  }
}
#elif defined (PS_PS4)
#include "foundation/ps4/ps4_native.h"
namespace sulphur 
{
  namespace foundation 
  {
    using Native = PS4Native;
  }
}
#endif