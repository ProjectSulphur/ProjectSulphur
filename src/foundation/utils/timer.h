#pragma once

#include <chrono>

namespace sulphur 
{
  namespace foundation 
  {

    /**
    * @class sulphur::foundation::Timer
    * @brief A simple timer class for measuring intervals.
    * @author Angelo van der Mark
    */
    class Timer
    {
    public:

      /**
      * @brief Constructor.
      */
      Timer();

      /**
      * @brief Destructor.
      */
      ~Timer();

      /**
      * @brief Starts the timer or resumes it if it has been paused.
      */
      void Start();

      /**
      * @brief Stops and sets the timer to zero.
      */
      void Stop();

      /**
      * @brief Pauses the timer. To unpause, call Start().
      */
      void Pause();

      /**
      * @brief Returns the time difference between now and the last GetDeltaX or Start call.
      * @remarks Returns 0 if the timer is not running.
      * @return (float) Time difference in seconds.
      */
      float GetDeltaSeconds();

      /**
      * @brief Returns the time difference between now and the last GetDeltaX or Start call.
      * @remarks Returns 0 if the timer is not running.
      * @return (float) Time difference in milliseconds.
      */
      float GetDeltaMilliseconds();

      /**
      * @brief Returns the time elapsed since the timer started.
      * @return (float) Total time in seconds.
      */
      float GetElapsedSeconds();

      /**
      * @brief Retruns the time elapsed since the timer started.
      * @return (float) Total time in milliseconds.
      */
      float GetElapsedMilliseconds();

      /**
      * @brief Returns whether or not the time is running.
      * @return (bool) True if the timer has started.
      */
      bool has_started() const;

    private:
      std::chrono::steady_clock::time_point last_time_point_; //!< Time frame to compare against when the timer is running.
      long long elapsed_time_ns_; //!< Stores elapsed time in nanoseconds.
      bool has_started_; //!< Whether timer is running.
    };
  }

}