#pragma once
#include <foundation/utils/type_definitions.h>
#include <foundation/application/window_mode.h>
#include <foundation/application/native_window_handle.h>
#include <foundation/containers/string.h>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace sulphur
{
  namespace engine
  {
    /**
     * @class sulphur::engine::Win32Window
     * @brief Base Win32 window implementation
     * @author Jelle de Haan
     */
    class Win32Window
    {
    public:
      /**
      * @brief Deconstructor
      */
      virtual ~Win32Window() {};

      /**
      * @brief Create a window using the given handle
      * @param[in] window_handle (void*) the window handle
      */
      virtual void Create(void* window_handle);

      /**
       * @see title_
       */
      virtual const foundation::String& title() const { return title_; };

      /**
      * @brief Set the window title
      * @param[in] (const sulphur::foundation::String) window title
      */
      virtual void SetTitle(const foundation::String&) {};

      /**
       * @see position_
       */
      virtual glm::ivec2 position() const { return position_; };

      /**
       * @brief Set the window position
       * @param[in] (glm::ivec2) The new window position
       */
      virtual void SetPosition(glm::ivec2) {};

      /**
       * @see size_
       */
      virtual glm::ivec2 size() const;

      /**
       * @brief Set the window size
       * @param[in] (glm::ivec2) The new window size
       */
      virtual void SetSize(glm::ivec2) {};

      /**
       * @see pointer_enabled_
       */
      virtual bool pointer_enabled() const { return pointer_enabled_; };

      /**
       * @brief Indicate if the pointer should be enabled or not.
       *        Disabled pointers are hidden and provide unlimited virtual cursor movement.
       */
      virtual void SetPointerEnabled(bool enabled);
      
      /**
       * @brief
       * @return True if the window has recived a close event
       */
      virtual bool ShouldClose() const;

      /**
       * @see window_mode_
       */
      virtual foundation::WindowMode window_mode() const { return mode_; };

      /**
       * @brief Sets the window mode
       * @param[in] mode (sulphur::engine::WindowMode) The new window mode
       */
      virtual void SetWindowMode(foundation::WindowMode mode);

      /**
       * @brief Get the native window handle
       * @return (foundation::NativeWindowHandle) The native window handle
       */
      virtual foundation::NativeWindowHandle GetWindowHandle() const;
      
    protected:
      glm::ivec2 position_;      //!< The position of the window
      glm::ivec2 size_;          //!< The size of the window

      void* window_;             //!< Native window handle
      foundation::String title_; //!< Window title
      foundation::WindowMode mode_;          //!< Current window mode
      bool pointer_enabled_;     //!< Indicates if the pointer is currently enabled

    private:
#ifdef _WIN64
      static __int64 WindowProc(void* window_handle, uint message, unsigned __int64 word_param, __int64 long_param);
#else
      static long WindowProc(void* window_handle, uint message, uint word_param, long long_param);
#endif
    };

    /**
     * @class sulphur::engine::Win32GLFWWindowWrapper : sulphur::engine::Win32Window
     * @brief Win32 window managed by GLFW
     * @author Kenneth Buijssen
     */
    class Win32GLFWWindowWrapper : public Win32Window
    {
    public:
      /**
       * @brief Deconstructor
       */
      ~Win32GLFWWindowWrapper() override;

      /**
       * @brief Create a window with the given title and size
       * @param[in] title (const sulphur::foundation::String) window title
       * @param[in] size (glm::ivec2) window size
       */
      void Create(const foundation::String& title, glm::ivec2 size);

      void SetTitle(const foundation::String& title) override;
      void SetPosition(glm::ivec2 position) override;

      glm::ivec2 size() const override;
      void SetSize(glm::ivec2 size) override;

      void SetPointerEnabled(bool enabled) override;
      bool ShouldClose() const override;

      void SetWindowMode(foundation::WindowMode mode) override;
      foundation::NativeWindowHandle GetWindowHandle() const override;

      /**
       * @see glfw_window_
       */
      GLFWwindow* glfw_window() const { return reinterpret_cast<GLFWwindow*>(window_); }
    };
  }
}
