#pragma once

namespace sulphur 
{
  namespace foundation 
  {
    class Memory;

    /**
    * @class sulphur::foundation::Win32Native
    * @brief Provides native Windows functionalities to be cross-platform accessible
    * @author Daniel Konings
    */
    class Win32Native
    {
      friend Memory;

    protected:

      /**
      * @brief Does nothing on Windows, as shared libraries work differently anyway
      * @remarks Returns nullptr
      */
      static void* InitializeMemoryPool(size_t size = 0ul);
    };
  }
}