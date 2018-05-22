#pragma once

#include "engine/application/window.h"
#include "engine/input/input.h"
#include "engine/input/input_server.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    /**
     * @class sulphur::engine::Win32Platform
     * @brief Win32 implementation of the platform
     * @author Kenneth Buijssen
     */
    class Win32Platform
    {
    public:
      /**
       * @brief Constructor
       * @param[in] window_handle (void*) Window handle to use
       */
      Win32Platform(void* window_handle);

      /**
       * @brief Deconstructor
       */
      ~Win32Platform();

      /**
       * @brief Process platform events
       */
      void ProcessEvents();

      /** 
       * @brief Check if the exit signal has been recieved
       * @returns True if the application should exit
       */
      bool ShouldExit();

      /**
       * @see window_
       */
      Window& window() const;

      /**
       * @see input_
       */
      Input& input() const;

    private:
      foundation::UniquePointer<Window> window_; //!< Window implementation
      foundation::UniquePointer<PlatformInput> input_; //!< Input implementation
      foundation::UniquePointer<InputServer> input_server_; //!< Input server
    };

    //--------------------------------------------------------------------------
    inline Window& Win32Platform::window() const
    {
      return *window_;// .get();
    }

    //--------------------------------------------------------------------------
    inline Input& Win32Platform::input() const
    {
      return *input_.get();
    }
  }
}
