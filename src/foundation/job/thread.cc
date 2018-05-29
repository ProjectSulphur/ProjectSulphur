#include "foundation/job/thread.h"

namespace sulphur
{
  namespace foundation
  {
    thread_local Thread* Thread::current_ = nullptr;

    //--------------------------------------------------------------------------
    Thread::Thread(eastl::function<void()> function) :
      interrupted_(false),
      is_done_(false)
    {
      // Wrap the function in a lambda so we can automate the book keeping
      auto thread_function = [this](eastl::function<void()> func) {
        current_ = this;
        func();
        is_done_ = true;
      };

      thread_ = std::thread(thread_function, function);
    }

    //--------------------------------------------------------------------------
    Thread::~Thread()
    {
      // We first interrupt the thread to signal it to stop
      // after this it is safe to try and join our thread
      Interrupt();
      thread_.join();
    }

    //--------------------------------------------------------------------------
    void Thread::Interrupt()
    {
      interrupted_.store(true);
    }

    //--------------------------------------------------------------------------
    bool Thread::IsDone() const
    {
      return is_done_.load();
    }

    //--------------------------------------------------------------------------
    void Thread::InterruptPoint()
    {
      if (current_->interrupted_.load())
      {
#ifdef PS_WIN32
        throw ThreadInterruptedException();
#endif
      }
    }
    
    //--------------------------------------------------------------------------
    bool Thread::IsInterrupted()
    {
      return current_->interrupted_.load();
    }
  }
}