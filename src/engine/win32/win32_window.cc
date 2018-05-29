#include "engine/win32/win32_window.h"

#include <Windows.h>
#include <tchar.h> 
#include "foundation/logging/logger.h"
#include "engine/application/window.h"
#include "win32_messages.h"

static constexpr LONG kStyleWindowed   = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU;
static constexpr LONG kStyleBorderless = 0;
static constexpr LONG kStyleFullscreen = 0;
static constexpr LONG kStyleNested = WS_VISIBLE | WS_BORDER;

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Win32Window::Win32Window(const foundation::String& title, 
                             glm::ivec2 size, 
                             void* parent_window) :
      parent_(parent_window),
      should_close_(false),
      pointer_enabled_(true),
      window_mode_(foundation::WindowMode::kWindowed)
    {
      constexpr char window_class[] = "win32app";
      const HINSTANCE h_instance = GetModuleHandle(nullptr);

#pragma warning(push)
#pragma warning(disable: 4302)
      WNDCLASSEX wcex;
      wcex.cbSize = sizeof(WNDCLASSEX);
      wcex.style = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc = reinterpret_cast<WNDPROC>(WindowProc);
      wcex.cbClsExtra = 0;
      wcex.cbWndExtra = 0;
      wcex.hInstance = h_instance;
      wcex.hIcon = LoadIcon(h_instance, MAKEINTRESOURCE(IDI_APPLICATION));
      wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
      wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
      wcex.lpszMenuName = nullptr;
      wcex.lpszClassName = window_class;
      wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
#pragma warning(pop)

      PS_LOG_IF(!RegisterClassEx(&wcex), Fatal, "Win32: Could not register window class");
                  
      window_ = CreateWindow(
        window_class,                 // window_class name
        title.c_str(),                // window_title
        WS_OVERLAPPEDWINDOW,          // style
        CW_USEDEFAULT, CW_USEDEFAULT, // window position
        CW_USEDEFAULT, CW_USEDEFAULT, // window size
        NULL,                         // parent
        NULL,                         // menu
        h_instance,                   // instance of the current application
        this                          // expose this class to winproc on WM_NCCREATE
      );

      PS_LOG_IF(window_ == nullptr, Fatal, "Win32: Failed to create window");

      // Set the size to ensure the inner size is correct
      SetSize(size);

      if (parent_ != nullptr) 
      {
        SetParent(static_cast<HWND>(window_), static_cast<HWND>(parent_));

        // Set the position and size to match the parent
        SetPosition({});
        RECT parent_rect{};
        GetClientRect(static_cast<HWND>(parent_), &parent_rect);
        SetSize({ parent_rect.left - parent_rect.right, parent_rect.top - parent_rect.bottom });

        // Clear the normal style and replace it with a nested style
        LONG style = GetWindowLong(static_cast<HWND>(window_), GWL_STYLE);
        style &= ~(kStyleWindowed | kStyleBorderless | kStyleFullscreen | kStyleNested);
        SetWindowLong(static_cast<HWND>(window_), GWL_STYLE, style);
      }

      ShowWindow(static_cast<HWND>(window_), 1);
      UpdateWindow(static_cast<HWND>(window_));
    }

    //--------------------------------------------------------------------------
    Win32Window::~Win32Window()
    {
      DestroyWindow(static_cast<HWND>(window_));
    }

    //--------------------------------------------------------------------------
    foundation::String Win32Window::GetTitle() const
    {
      foundation::String title = foundation::String(GetWindowTextLength(static_cast<HWND>(window_)) + 1, '\0');
      GetWindowText(static_cast<HWND>(window_), &title[0], static_cast<int>(title.size()));
      return title;
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetTitle(const foundation::String& title)
    {
      SetWindowText(static_cast<HWND>(window_), title.c_str());
    }

    //--------------------------------------------------------------------------
    glm::ivec2 Win32Window::GetPosition() const
    {
      RECT rect = {};
      ClientToScreen(static_cast<HWND>(window_), reinterpret_cast<LPPOINT>(&rect));
      return glm::ivec2(rect.left, rect.top);
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetPosition(glm::ivec2 position)
    {
      RECT wr = {};
      AdjustWindowRect(
        &wr, 
        static_cast<DWORD>(GetWindowLong(static_cast<HWND>(window_), GWL_STYLE)), 
        false
      );

      const int x = position.x + wr.left;
      const int y = position.y + wr.top;

      SetWindowPos(static_cast<HWND>(window_), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
    }

    //--------------------------------------------------------------------------
    glm::ivec2 Win32Window::GetSize() const
    {
      RECT rect;
      GetClientRect(static_cast<HWND>(window_), &rect);

      return { rect.right, rect.bottom };
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetSize(glm::ivec2 size)
    {
      RECT rect = { 0, 0, size.x, size.y };
      AdjustWindowRectEx(
        &rect, 
        static_cast<DWORD>(GetWindowLong(static_cast<HWND>(window_), GWL_STYLE)), 
        false, 
        WS_EX_APPWINDOW
      );

      SetWindowPos(
        static_cast<HWND>(window_), 
        HWND_TOP, 
        0, 0, rect.right - rect.left, rect.bottom - rect.top, 
        SWP_NOMOVE
      );
    }

    //--------------------------------------------------------------------------
    bool Win32Window::GetPointerEnabled() const
    {
      return pointer_enabled_;
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetPointerEnabled(bool enabled)
    {
      if (enabled == pointer_enabled_)
      {
        return;
      }

      pointer_enabled_ = enabled;
      ShowCursor(pointer_enabled_);
    }

    //--------------------------------------------------------------------------
    foundation::WindowMode Win32Window::GetWindowMode() const
    {
      return window_mode_;
    }

    //--------------------------------------------------------------------------
    void Win32Window::SetWindowMode(foundation::WindowMode mode)
    {
      if (parent_ != nullptr)
      {
        return;
      }

      const HWND hwnd = static_cast<HWND>(window_);

      if (window_mode_ == mode)
      {
        return;
      }

      glm::ivec2 size = GetSize();
      LONG style = GetWindowLong(hwnd, GWL_STYLE);
      
      // Clear the style to remove the old window mode
      style &= ~(kStyleWindowed | kStyleBorderless | kStyleFullscreen | kStyleNested);

      // Inform the renderer that we will be leaving fullscreen
      if (window_mode_ == foundation::WindowMode::kFullscreen)
      {
        PostMessage(hwnd, static_cast<uint>(Win32Messages::kExitFullscreen), 0, 0);
      }
      
      // Set the correct style properties based on the new window mode
      if (mode == foundation::WindowMode::kWindowed)
      {
        style |= kStyleWindowed;
      }
      else if (mode == foundation::WindowMode::kBorderless)
      {
        style |= kStyleBorderless;
      }
      else if (mode == foundation::WindowMode::kFullscreen)
      {
        // Fullscreen is handled by the renderer
        PostMessage(hwnd, static_cast<uint>(Win32Messages::kEnterFullscreen), 0, 0);
        style |= kStyleFullscreen;
      }
      
      // Update the style and window mode
      SetWindowLong(hwnd, GWL_STYLE, style);
      window_mode_ = mode;

      // Reset to keep the inner size consitent between windowed and 
      SetSize(size);

      // Force a resive of the window to stop it from being minimized afte
      // switching from borderless to windowed
      ShowWindow(static_cast<HWND>(window_), 1);
      UpdateWindow(static_cast<HWND>(window_));
    }

    //--------------------------------------------------------------------------
    bool Win32Window::ShouldClose() const
    {
      return should_close_;
    }

    //--------------------------------------------------------------------------
    void* Win32Window::GetNativeWindowHandle() const
    {
      return window_;
    }

    //--------------------------------------------------------------------------
    void Win32Window::RegisterCallback(IWin32EventListener* callback)
    {
      PS_LOG_IF(eastl::find(callbacks_.begin(), callbacks_.end(), callback) != callbacks_.end(),
        Fatal, "Tried to register the same event listener twice!");

      callbacks_.push_back(callback);
    }

    //--------------------------------------------------------------------------
    void Win32Window::UnregisterCallback(IWin32EventListener* callback)
    {
      callbacks_.erase(eastl::remove(callbacks_.begin(), callbacks_.end(), callback));
    }

    //--------------------------------------------------------------------------
    __int64 Win32Window::WindowProc(void * window_handle, uint message, unsigned __int64 word_param, __int64 long_param)
    {
      const HWND hwnd = static_cast<HWND>(window_handle);
      Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

      // When a new window is created, make sure to store a pointer to the window 
      // class in its user data.
      if (message == WM_NCCREATE)
      {
        window = static_cast<Window*>(
          reinterpret_cast<CREATESTRUCT*>(long_param)->lpCreateParams);

        SetLastError(0);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        PS_LOG_IF(GetLastError() != 0, Fatal, "Could not store the window pointer in long_param");
      }

      // Ensure that we can access the window
      if (!window)
      {
        return DefWindowProc(hwnd, message, word_param, long_param);
      }

      // If we have a parent, override behavior to function as an embedded window
      if (window-> parent_) 
      {
        switch(message) 
        {
        case WM_MOVE:
          window->SetPosition({0, 0});
        case WM_SIZE:
          RECT rect;
          GetClientRect(static_cast<HWND>(window->parent_), &rect);
          window->SetSize({rect.right, rect.bottom});
        }
      }

      // Call all the event callbacks
      for (IWin32EventListener* callback : window->callbacks_)
      {
        // Check if the callback consumed the event, if so stop processing the message
        if (callback->OnWin32Event(window_handle, message, word_param, long_param))
        {
          return 0;
        }
      }

      // Let the window handle any events that are left
      switch (message)
      {
      case WM_DESTROY:
        window->should_close_ = true;
        return 0;
      default:
        // Since we did not handle the event, let windows take care of it
        return DefWindowProc(hwnd, message, word_param, long_param);
      }      
    }

    //--------------------------------------------------------------------------
    void Win32Window::ProcessEvents()
    {
      // Check if the parent changed in size
      if (parent_ != nullptr) 
      {
        RECT rect = {};
        GetClientRect(static_cast<HWND>(parent_), &rect);
        const glm::ivec2 size = GetSize();
        if (size.x != rect.right || size.y != rect.bottom) {
          SetSize({});
        }  
      }

      // Main message loop:  
      MSG msg;
      while (PeekMessage(&msg, static_cast<HWND>(window_), 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }
}