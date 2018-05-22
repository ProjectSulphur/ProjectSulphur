#include "engine/win32/win32_platform.h"
#include "engine/win32/win32_window.h"
#include "engine/application/application.h"

#include <foundation/logging/logger.h>

#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>

//------------------------------------------------------------------------------
static void GLFWErrorCallback(int error_code, const char* description)
{
  PS_LOG(Error, "[GLFW] %d : %s", error_code, description);
}

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Win32Platform::Win32Platform(void* window_handle)
    {
      // Error callback can be set before initialize, 
      // this allows for capturing of the initialization error.
      glfwSetErrorCallback(GLFWErrorCallback);

      PS_LOG_IF(glfwInit() == GLFW_FALSE, Fatal, "Could not initialize GLFW");
      
      if (window_handle != nullptr)
      {
#ifndef PS_EDITOR
        PS_LOG(Warning, "In release mode window handle is not expected to be passed in");
#endif

        window_ = foundation::Memory::ConstructUnique<Win32Window>();
        window_->Create(window_handle);
      }
      else
      {
#ifdef PS_EDITOR
        PS_LOG(Fatal, "In editor mode window handle cannot be nullptr");
#endif

        window_ = foundation::Memory::ConstructUnique<Win32GLFWWindowWrapper>();

        Win32GLFWWindowWrapper* window = reinterpret_cast<Win32GLFWWindowWrapper*>(window_.get());

        window->Create("Project Sulphur", glm::ivec2{ 1280, 720 });
        input_ = foundation::Memory::ConstructUnique<PlatformInput>();
        input_server_ = foundation::Memory::ConstructUnique<InputServer>(*window, *input_.get());
      }
    }

    //--------------------------------------------------------------------------
    Win32Platform::~Win32Platform()
    {
      glfwTerminate();
    }

    //--------------------------------------------------------------------------
    void Win32Platform::ProcessEvents()
    {
      glfwPollEvents();

      if (input_server_ != nullptr)
      {
        input_server_->ProcessEvents();
      }
    }

    //--------------------------------------------------------------------------
    bool Win32Platform::ShouldExit()
    {
      return window_->ShouldClose();
    }
  }
}