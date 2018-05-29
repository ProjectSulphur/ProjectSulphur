#include "engine/win32/win32_platform.h"
#include "engine/win32/win32_window.h"

#include "engine/input/input.h"
#include "engine/input/input_server.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Win32Platform::Win32Platform(foundation::SharedPointer<IRenderer> renderer, 
                                 void* parent_window) :
      render_event_handler_(renderer)
    {
      window_ = foundation::Memory::ConstructUnique<Win32Window>(
        "Sulphur engine",
        glm::ivec2{ 1280, 720 },
        parent_window
      );
      
      input_ = foundation::Memory::ConstructUnique<PlatformInput>();
      input_server_ = foundation::Memory::ConstructUnique<InputServer>(*input_.get(), 
                                                                       *window_.get());

      window_->RegisterCallback(&render_event_handler_);
      window_->RegisterCallback(&imgui_event_handler_);
      window_->RegisterCallback(input_server_.get());
    }

    //--------------------------------------------------------------------------
    void Win32Platform::ProcessEvents()
    {
      window_->ProcessEvents();

      if (input_server_ != nullptr)
      {
        input_server_->ProcessEvents();
      }
    }

    //--------------------------------------------------------------------------
    bool Win32Platform::ShouldExit() const
    {
      return window_->ShouldClose();
    }

    //--------------------------------------------------------------------------
    Window& Win32Platform::window() const
    {
      return *window_;
    }

    //--------------------------------------------------------------------------
    Input& Win32Platform::input() const
    {
      return *input_.get();
    }
  }
}