#pragma once
#include <EASTL/internal/function.h>
#include <atomic>
#include <thread>


namespace sulphur
{
  namespace foundation
  {
    /**
     * @struct sulphur::foundation::ThreadInterruptedException : std::exception
     * @brief Custom exception type used for thread interrupts
     * @author Kenneth Buijssen
     */
    struct ThreadInterruptedException : std::exception {};

    /**
     * @class sulphur::foundation::Thread
     * @brief An interruptable thread implementation arround std::thread
     * @author Kenneth Buijssen
     */
    class Thread
    {
    public:
      /**
       * @brief Create a new thread which executes the function specified
       * @param[in] function (eastl::function<void()>)
       *            The function to execute on the thread
       */
      explicit Thread(eastl::function<void()> function);

      /**
       * @brief Interrupt and join the thread
       */
      ~Thread();

      /**
       * @brief Set the interrupt flag for the thread, triggering an interrupt 
       *        at the next interruption point.
       */
      void Interrupt();

      /**
       * @brief Check if the thread is done
       * @return (bool) True if the thread is done executing
       */
      bool IsDone() const;

      /**
       * @brief Specify a point at which the thread execution can be interrupted
       * @throw ThreadInterruptedException if the thread has been interrupted
       */
      static void InterruptPoint();

      /**
       * @brief Check if the thread interrupt flag has been set
       * @return (bool) True if the interrupt flag has been set
       */
      static bool IsInterrupted();

    private:
      static thread_local Thread* current_; //!< Reference to the current thread

      std::thread thread_;           //!< Thread executing the work
      std::atomic_bool interrupted_; //!< Signals if the thread should be interrupted
      std::atomic_bool is_done_;     //!< Signals if the thread is done with the work
    };
  }
}
