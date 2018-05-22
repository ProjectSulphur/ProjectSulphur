
#include "engine/application/window.h"
#include "engine/input/mappings.h"

struct GLFWwindow;

namespace sulphur
{
  namespace engine
  {
    class PlatformInput;

    // NOTE: Forwards GLFW callbacks to the input system
    class Win32InputServer
    {
    public:
      Win32InputServer(const Win32GLFWWindowWrapper& window, PlatformInput& input_client);
      ~Win32InputServer();

      void ProcessEvents();

    private:
      static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
      static void CharCallback(GLFWwindow* window, unsigned int code_point);
      static void CursorPosCallback(GLFWwindow* window, double x_pos, double y_pos);
      static void CursorEnterCallback(GLFWwindow* window, int entered);
      static void MouseButtonCallback(GLFWwindow* window, int mouse_button, int action, int mods);
      static void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
      static void JoystickCallback(int joy, int event);

    private:
      static PlatformInput* input_client_;
      const Win32GLFWWindowWrapper& window_;

    };
  }
}