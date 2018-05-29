#pragma once
#include "engine/application/window.h"

namespace sulphur
{
  namespace engine
  {
    class PlatformInput;

    /**
     * @class sulphur::engine::Win32InputServer : public sulphur::engine::IWin32EventListener
     * @brief The input server processes platform input events and relays them to the input system
     * @author Maarten ten Velden, Kenneth Buijssen
     */
    class Win32InputServer : public IWin32EventListener
    {
    public:
      /**
       * @brief Constructor
       * @param[in] input_client (sulphur::engine::PlatformInput&)
       *            The input system to relay the input events to
       * @param[in] window (sulphur::engine::Win32Window&)
       *            The window from which to recieve input
       */
      Win32InputServer(PlatformInput& input_client, Win32Window& window);

      /**
       * @brief Deconstructor
       */
      ~Win32InputServer();
      
      bool OnWin32Event(void* window_handle, uint message, uint64_t word_param,
        int64_t long_param) override;

      /**
       * @brief Process any events required at the start of the frame
       */
      void ProcessEvents();

    private:
      PlatformInput& input_client_; //!< The input system to which to send the input
      Win32Window&   window_;       //!< The window from which to recieve input
    };
  }
}