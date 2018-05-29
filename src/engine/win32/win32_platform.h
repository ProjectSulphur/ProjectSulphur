#pragma once

#include "engine/application/window.h"

#include "engine/win32/win32_imgui_event_handler.h"
#include "engine/win32/win32_render_event_handler.h"
#include "engine/win32/win32_input_server.h"
#include "engine/input/input.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    class Input;
    class PlatformInput;
    class Win32InputServer;
    class IRenderer;

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
       * @param[in] renderer (foundation::SharedPointer<IRenderer>)
       *            The current renderer which will render to the window
       * @param[in] parent_window (void*) 
       *            Window handle for the parent window in which to embed the engine window
       */
      explicit Win32Platform(foundation::SharedPointer<IRenderer> renderer, 
                             void* parent_window = nullptr);

      /**
       * @brief Deconstructor
       */
      ~Win32Platform() = default;

      /**
       * @brief Process platform events
       */
      void ProcessEvents();

      /** 
       * @brief Check if the exit signal has been recieved
       * @returns True if the application should exit
       */
      bool ShouldExit() const;

      /**
       * @see window_
       */
      Window& window() const;

      /**
       * @see input_
       */
      Input& input() const;

    private:
      foundation::UniquePointer<Window> window_;                 //!< Window implementation
      foundation::UniquePointer<PlatformInput> input_;           //!< Input implementation
      foundation::UniquePointer<Win32InputServer> input_server_; //!< Input server

      Win32ImguiEventHandler  imgui_event_handler_;  //! Handler for imgui events
      Win32RenderEventHandler render_event_handler_; //!< Handler for render events
    };
  }
}
