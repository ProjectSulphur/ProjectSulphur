#pragma once

#include "engine/win32/win32_window.h"
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    class IRenderer;

    /**
     * @class sulphur::engine::Win32RenderEventHandler : public sulphur::engine::IWin32EventListener
     * @brief Helper to process Win32 events required by the renderer.
     * @author Kenneth Buijssen
     */
    class Win32RenderEventHandler : public IWin32EventListener
    {
    public:
      /**
       * @brief Constructor
       * @param[in] renderer (sulphur::foundation::SharedPointer<IRenderer>)
       *            The renderer the events are send to
       */
      explicit Win32RenderEventHandler(foundation::SharedPointer<IRenderer> renderer);

      bool OnWin32Event(void* window_handle, uint message, uint64_t word_param,
        int64_t long_param) override;
    private:
      foundation::SharedPointer<IRenderer> renderer_; //!< The renderer the events are send to
    };
  }
}
