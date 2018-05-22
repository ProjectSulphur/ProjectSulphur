#pragma once

#ifdef PS_WIN32
#include "foundation/win32/win32_lib_loader.h"
namespace sulphur 
{
  namespace foundation 
  {
    using LibLoader = Win32LibLoader;
  }
}
#elif defined PS_PS4
#include "foundation/ps4/ps4_lib_loader.h"
namespace sulphur 
{
  namespace foundation 
  {
    using LibLoader = PS4LibLoader;
  }
}
#endif

