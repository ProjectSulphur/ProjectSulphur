#pragma once

#include "engine/win32/win32_window.h"

namespace sulphur
{
  namespace engine
  {
    /**
     * @class sulphur::engine::Win32ImguiEventHandler : public sulphur::engine::IWin32EventListener
     * @brief Helper to process Win32 events required by imgui, for example input.
     * @author Kenneth Buijssen
     */
    class Win32ImguiEventHandler : public IWin32EventListener
    {
    public:
      bool OnWin32Event(void* window_handle, uint message, uint64_t word_param,
        int64_t long_param) override;
    };
  }
}