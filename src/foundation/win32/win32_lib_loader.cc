#include "foundation/win32/win32_lib_loader.h"
#include "foundation/containers/string.h"
#include "foundation/memory/memory.h"
#include <assert.h>

namespace sulphur
{
  namespace foundation
  {
    //-----------------------------------------------------------------------------------------------
    bool Win32LibLoader::has_own_memory_ = false;

    //-----------------------------------------------------------------------------------------------
    void Win32LibLoader::AllocateSharedMemory(size_t size)
    {
      if (has_own_memory_ == true)
      {
        return;
      }

      foundation::Memory::Initialize(size);
      has_own_memory_ = true;
    }

    //-----------------------------------------------------------------------------------------------
    void* Win32LibLoader::LoadLib(const char* path)
    {
      String full_path = String(path) + ".dll";

      void* lib = LoadLibraryA(full_path.c_str());
      assert(lib != nullptr);

      return lib;
    }

    //-----------------------------------------------------------------------------------------------
    void Win32LibLoader::UnloadLib(void* lib)
    {
      FreeLibrary(reinterpret_cast<HMODULE>(lib));
    }
  }
}