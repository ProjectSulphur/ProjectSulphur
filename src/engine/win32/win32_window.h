#pragma once
#include <foundation/utils/type_definitions.h>
#include <foundation/application/window_mode.h>
#include <foundation/containers/string.h>

#include <glm/glm.hpp>
#include "EASTL/weak_ptr.h"
#include "foundation/containers/vector.h"

struct GLFWwindow;

namespace sulphur
{
  namespace engine
  {
    /**
     * @class sulphur::engine::IWin32EventListener
     * @brief Interface to allow classes to subscribe to win32 events
     */
    class IWin32EventListener
    {
    public:
      /**
       * @brief Deconstructor
       */
      virtual ~IWin32EventListener() = default;
      
      /**
       * @brief Win32 event callback
       * @param[in] window_handle (void*) The native window handle for the recieving window
       * @param[in] message (uint)        Win32 event message code
       * @param[in] word_param (uint64_t) Win32 event word param
       * @param[in] long_param (int64_t)  Win32 event long param
       */
      virtual bool OnWin32Event(void* window_handle, uint message, 
                                uint64_t word_param, int64_t long_param) = 0;
    };

    /**
     * @class sulphur::engine::Win32Window
     * @brief Win32 window implementation
     * @author Kenneth Buijssen, Jelle de Haan
     */
    class Win32Window
    {
    public:
      /**
       * @brief Create a new window
       * @param[in] title (sulphur::foundation::String&) The window title
       * @param[in] size (glm::ivec2) The window size
       * @param[in] parent_window (void*) The window will be nested inside of the parent window
       * @remark The size is ignored if a parent window has been provided
       */
      Win32Window(const foundation::String& title, glm::ivec2 size, void* parent_window = nullptr);

      /**
       * @brief Desconstructor
       */
      ~Win32Window();

      /**
       * @brief Get the window title
       * @return (sulphur::foundation::String) The window title
       */
      foundation::String GetTitle() const;

      /**
       * @brief Set the window title
       * @param[in] title (sulphur::foundation::String&) The window title
       */
      void SetTitle(const foundation::String& title);

      /**
       * @brief Get the window position
       * @return (glm::ivec2) The window position
       */
      glm::ivec2 GetPosition() const;

      /**
       * @brief Set the window position
       * @param[in] position (ivec2) The window position
       */
      void SetPosition(glm::ivec2 position);

      /**
       * @brief Get the window size
       * @return (glm::ivec2) The window size
       */
      glm::ivec2 GetSize() const;

      /**
       * @brief Set the window size
       * @param[in] size (ivec2) The window size
       */
      void SetSize(glm::ivec2 size);

      /**
       * @brief Check if the pointer (cursor) is enabled or not
       * @return True if the pointer is enabled
       */
      bool GetPointerEnabled() const;

      /**
       * @brief Enable or disable the pointer
       * @param[in] enabled (bool) If the pointer should be enabled
       */
      void SetPointerEnabled(bool enabled);

      /**
       * @brief Get the window mode
       * @return (sulphur::foundation::WindowMode) The window mode
       */
      foundation::WindowMode GetWindowMode() const;

      /**
       * @brief Set the window mode
       * @param[in] mode (sulphur::foundation::WindowMode) The window mode
       */
      void SetWindowMode(foundation::WindowMode mode);

      /**
       * @brief Check if the window should be closed
       * @return (bool) True if the window should be clossed
       */
      bool ShouldClose() const;

      /**
       * @brief Process the platform events
       */
      void ProcessEvents();

      /**
       * @brief Get the platform specific window handle
       * @return (void*) The platform specific window handle
       */
      void* GetNativeWindowHandle() const;

      /**
       * @brief Register a windows event callback
       * @param[in] callback (sulphur::engine::IWin32EventListener*) The event callback
       */
      void RegisterCallback(IWin32EventListener* callback);

      /**
       * @brief Unregister a windows event callback
       * @param[in] callback (sulphur::engine::IWin32EventListener*) The event callback
       */
      void UnregisterCallback(IWin32EventListener* callback);

    private:
      /**
       * @brief Native WinProc callback
       * @param[in] window_handle (void*)         The native window handle for the recieving window
       * @param[in] message (uint)                Win32 event message code
       * @param[in] word_param (unsigned __int64) Win32 event word param
       * @param[in] long_param (__int64)          Win32 event long param
       */
      static __int64 __stdcall WindowProc(void* window_handle, uint message, 
                                          unsigned __int64 word_param, __int64 long_param);
    protected:
      void* window_;                       //!< Native window handle
      void* parent_;                       //!< The native window handle of the parent window
      bool should_close_;                  //!< Window close flag
      bool pointer_enabled_;               //!< Flag to indicate if the pointer is enabled or not
      foundation::WindowMode window_mode_; //!< Current window mode

      foundation::Vector<IWin32EventListener*> callbacks_; //!< List of registered callbacks
    };
  }
}
