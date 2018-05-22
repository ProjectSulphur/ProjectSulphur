#pragma once

#include <Windows.h>

namespace sulphur
{
  namespace foundation
  {
    /**
    * @class sulphur::foundation::Win32LibLoader
    * @brief Used to load function pointers from a .dll file
    * @author Daniel Konings
    */
    class Win32LibLoader
    {

    public:

      /**
      * @brief Unimplemented on Windows, as memory is managed way differently here
      */
      static void AllocateSharedMemory(size_t size = 0ul);

      /**
      * @brief Load a .dll using LoadLibraryA from Windows.h
      * @param[in] path (const char*) The path to the .dll to load
      * @return (void*) The loaded library, or nullptr if loading failed
      */
      static void* LoadLib(const char* path);

      /**
      * @brief Loads a function from a specified .dll
      * @param[in] name (const char*) The undecorated function name
      * @param[in] lib (void*) The library to load the function from
      * @return (T) The loaded function callback
      */
      template <typename T>
      static T LoadFunction(const char* name, void* lib);

      /**
      * @brief Unloads a provided library
      * @param[in] lib (void*) The library to unload
      */
      static void UnloadLib(void* lib);

    private:

      static bool has_own_memory_; //!< Does a shared library have its own memory yet?
    };

    //-----------------------------------------------------------------------------------------------
    template <typename T>
    inline T Win32LibLoader::LoadFunction(const char* name, void* lib)
    {
      return reinterpret_cast<T>(GetProcAddress(reinterpret_cast<HMODULE>(lib), name));
    }
  }
}