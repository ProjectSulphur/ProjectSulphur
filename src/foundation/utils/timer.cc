#include "timer.h"

namespace sulphur
{

  namespace foundation
  {
    //-------------------------------------------------------------------------
    Timer::Timer() :
      elapsed_time_ns_(0),
      has_started_(false)
    {}

    //-------------------------------------------------------------------------
    Timer::~Timer()
    {}

    //-------------------------------------------------------------------------
    void Timer::Start()
    {
      last_time_point_ = std::chrono::high_resolution_clock::now();
      has_started_ = true;
    }

    //-------------------------------------------------------------------------
    void Timer::Stop()
    {
      elapsed_time_ns_ = 0;
      has_started_ = false;
    }

    //-------------------------------------------------------------------------
    void Timer::Pause()
    {
      if (!has_started_)
      {
        return;
      }

      std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
      long long delta_ns = std::chrono::duration_cast
        <std::chrono::nanoseconds>(now - last_time_point_).count();
      elapsed_time_ns_ += delta_ns;
      has_started_ = false;
    }

    //-------------------------------------------------------------------------
    float Timer::GetDeltaSeconds()
    {
      return GetDeltaMilliseconds() * 0.001f;
    }

    //-------------------------------------------------------------------------
    float Timer::GetDeltaMilliseconds()
    {
      if (!has_started_)
      {
        return 0.0f;
      }

      std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
      long long delta_ns = std::chrono::duration_cast
        <std::chrono::nanoseconds>(now - last_time_point_).count();
      elapsed_time_ns_ += delta_ns;
      last_time_point_ = now;

      float delta = static_cast<float>(delta_ns) * 0.000001f;
      return delta;
    }

    //-------------------------------------------------------------------------
    float Timer::GetElapsedSeconds()
    {
      return GetElapsedMilliseconds() * 0.001f;
    }

    //-------------------------------------------------------------------------
    float Timer::GetElapsedMilliseconds()
    {
      if (!has_started_)
      {
        return static_cast<float>(elapsed_time_ns_) * 0.000001f;
      }

      std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
      long long delta_ns = std::chrono::duration_cast
        <std::chrono::nanoseconds>(now - last_time_point_).count();
      return static_cast<float>((elapsed_time_ns_ + delta_ns)) * 0.000001f;
    }

    //-------------------------------------------------------------------------
    bool Timer::has_started() const
    {
      return has_started_;
    }
  }
}