#include "engine/win32/win32_window.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Windows.h>
#include "foundation/logging/logger.h"

namespace sulphur
{
  namespace engine
  {
    void Win32Window::Create(void* window_handle)
    {
      assert(window_handle && "Window handle can't be null!");

      window_ = window_handle;
      mode_ = foundation::WindowMode::kWindowed;
      pointer_enabled_ = true;
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetPointerEnabled(bool enabled)
    {
      if (enabled == pointer_enabled_)
      {
        return;
      }

      pointer_enabled_ = enabled;
      // @todo Windows pointer stuff
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetWindowMode(foundation::WindowMode mode)
    {
      if (mode_ == mode)
      {
        return;
      }

      mode_ = mode;
      // @todo Windows change mode stuff
    }

    //--------------------------------------------------------------------------
    glm::ivec2 Win32Window::size() const
    {
      RECT rect;
      GetClientRect(static_cast<HWND>(window_), &rect);
      return glm::ivec2(static_cast<int>(rect.right - rect.left), static_cast<int>(rect.bottom - rect.top));
    }

    //--------------------------------------------------------------------------
    bool Win32Window::ShouldClose() const
    {
      // @todo Windows should close stuff'n things
      return false;
    }

    //--------------------------------------------------------------------------
    foundation::NativeWindowHandle Win32Window::GetWindowHandle() const
    {
      foundation::NativeWindowHandle handle = {};
      handle.type = foundation::NativeWindowHandle::Type::kWin32;
      handle.win32_window = window_;

      return handle;
    }

    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::Create(const foundation::String& title, glm::ivec2 size)
    {
      title_ = title;
      mode_ = foundation::WindowMode::kWindowed;
      pointer_enabled_ = true;

      // TODO(Kenneth): Replace with FATAL log
      PS_LOG_IF(glfwInit() == GLFW_FALSE, Fatal, "Failed to initialize GLFW");

      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      window_ = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

      PS_LOG_IF(window_ == nullptr, Fatal, "GLFW failed to create a window");

      glfwGetWindowPos(glfw_window(), &position_.x, &position_.y);
      glfwGetWindowSize(glfw_window(), &size_.x, &size_.y);
    }

    //--------------------------------------------------------------------------
    Win32GLFWWindowWrapper::~Win32GLFWWindowWrapper()
    {
      glfwDestroyWindow(glfw_window());
    }

    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::SetTitle(const foundation::String& title)
    {
      if (title_ == title) 
      {
        return;
      }

      title_ = title;

      glfwSetWindowTitle(glfw_window(), title.c_str());
    }

    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::SetPosition(glm::ivec2 position)
    {
      if (position_ == position)
      {
        return;
      }

      position_ = position;

      glfwSetWindowPos(glfw_window(), position.x, position.y);
    }
    
    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::SetSize(glm::ivec2 size)
    {
      if (size_ == size) 
      {
        return;
      }

      size_ = size;

      glfwSetWindowSize(glfw_window(), size.x, size.y);
    }

    //--------------------------------------------------------------------------
    glm::ivec2 Win32GLFWWindowWrapper::size() const
    {
      return size_;
    }

    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::SetPointerEnabled(bool enabled)
    {
      if (enabled == pointer_enabled_) 
      {
        return;
      }
      pointer_enabled_ = enabled;

      const int cursor_mode = enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
      glfwSetInputMode(glfw_window(), GLFW_CURSOR, cursor_mode);
    }

    //--------------------------------------------------------------------------
    bool Win32GLFWWindowWrapper::ShouldClose() const
    {
      return glfwWindowShouldClose(glfw_window()) == GL_TRUE;
    }

    //--------------------------------------------------------------------------
    void Win32GLFWWindowWrapper::SetWindowMode(foundation::WindowMode mode)
    {
      if (mode_ == mode) 
      {
        return;
      }

      mode_ = mode;

      GLFWmonitor* monitor = nullptr;
      glm::ivec2 pos = position_;

      if (mode == foundation::WindowMode::kFullscreen)
      {
        monitor = glfwGetPrimaryMonitor();
        pos = glm::vec2(0, 0);
      }

      glfwSetWindowMonitor(glfw_window(),
        monitor,
        pos.x, pos.y,
        size_.x, size_.y,
        GLFW_DONT_CARE
      );
    }

    //--------------------------------------------------------------------------
    foundation::NativeWindowHandle Win32GLFWWindowWrapper::GetWindowHandle() const
    {
      foundation::NativeWindowHandle handle = {};
      handle.type = foundation::NativeWindowHandle::Type::kWin32;
      handle.win32_window = glfwGetWin32Window(glfw_window());

      return handle;
    }
  }
}
