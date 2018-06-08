#include "engine/rewinder/rewind_system.h"
#include "engine/rewinder/rewindable_storage_base.h"
#include "engine/rewinder/system_stored_data.h"
#include <foundation/job/job_graph.h>
#include <foundation/job/job.h>
#include <foundation/job/data_policy.h>
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    void HistoryBuffer::Add(const FrameStorage& frame)
    {
      frame_data_.push_back(frame);
    }

    //--------------------------------------------------------------------------
    FrameStorage& HistoryBuffer::operator[](size_t index )
    {
      return frame_data_[index];
    }

    RewindSystem::RewindSystem()
      :
      IServiceSystem( "Rewinder" ),
      frames_to_skip_( 0 ),
      frame_limit_( 0 ),
      frame_to_restore_( -1 ),
      prev_restored_frame( -1 ),
      active_( false )
    {}

    //--------------------------------------------------------------------------
    void RewindSystem::OnInitialize(Application&, foundation::JobGraph& job_graph )
    {
      const auto restore_rewind = [](RewindSystem& rewinder)
      {
        rewinder.RestoreFrame();
      };
      foundation::Job rewind_restore_job = make_job("restore", "restore_rewind",
        restore_rewind, bind_write(*this));
      job_graph.Add(std::move(rewind_restore_job));

      foundation::Job store_rewind_job = foundation::make_job("store_rewind", "", []() {});
      store_rewind_job.set_blocker("update");
      job_graph.Add(std::move(store_rewind_job));

      const auto store_rewind = [](RewindSystem& rewinder)
      {
        rewinder.StoreFrame();
      };
      foundation::Job rewind_store_job = make_job("store", "store_rewind",
        store_rewind, bind_write(*this));
      job_graph.Add(std::move(rewind_store_job));
    }

    //--------------------------------------------------------------------------
    void RewindSystem::OnTerminate()
    {
    }

    //--------------------------------------------------------------------------
    void RewindSystem::Register(RewindStorage& system_storage_data)
    {
      systems_storage_.push_back(&system_storage_data);
      systems_frame_data_.push_back(HistoryBuffer());
    }

    //--------------------------------------------------------------------------
    void RewindSystem::RestoreFrame()
    {
      for (size_t i = 0; i < systems_storage_.size(); ++i)
      {
        // Call restore function of system storage data
        systems_storage_[i]->Restore( systems_frame_data_[i][frame_to_restore_] );
      }
    }

    //--------------------------------------------------------------------------
    void RewindSystem::StoreFrame()
    {
      // Check if we need to skip this frame
      for ( size_t i = 0; i < systems_storage_.size(); ++i )
      {
        // Call store function of system storage data
        systems_frame_data_[i].Add( systems_storage_[i]->Store() );
      }
      return;
    }

    //--------------------------------------------------------------------------
    bool RewindSystem::IsRewinding()
    {
      return (frame_to_restore_ != -1);
    }

    bool RewindSystem::WasRewinding()
    {
      bool ret = ( frame_to_restore_ == -1 && prev_restored_frame != -1 );
      prev_restored_frame = frame_to_restore_;
      return ret;
    }

    //--------------------------------------------------------------------------
    void RewindSystem::StoreToDisk(const char*)
    {
    }

    //--------------------------------------------------------------------------
    size_t RewindSystem::CalculateTotalMemoryUsage()
    {
      return size_t();
    }

    //--------------------------------------------------------------------------
    size_t RewindSystem::CalculateSystemMemoryUsage(size_t)
    {
      return size_t();
    }

    //--------------------------------------------------------------------------
    void RewindSystem::set_frames_to_skip(size_t frames_to_skip)
    {
      frames_to_skip_ = frames_to_skip;
    }

    //--------------------------------------------------------------------------
    size_t RewindSystem::frames_to_skip()
    {
      return frames_to_skip_;
    }

    //--------------------------------------------------------------------------
    void RewindSystem::set_frame_limit(size_t frame_limit)
    {
      frame_limit_ = frame_limit;
    }

    //--------------------------------------------------------------------------
    size_t RewindSystem::frame_limit()
    {
      return frame_limit_;
    }

    //--------------------------------------------------------------------------
    void RewindSystem::set_frame_to_restore(int frame)
    {
      frame_to_restore_ = frame;
    }

    //--------------------------------------------------------------------------
    int RewindSystem::frame_to_restore()
    {
      return frame_to_restore_;
    }

    //--------------------------------------------------------------------------
    void RewindSystem::set_active(bool frame)
    {
      active_ = frame;
    }

    //--------------------------------------------------------------------------
    bool RewindSystem::active()
    {
      return active_;
    }
  }
}