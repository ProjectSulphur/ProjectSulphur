#pragma once
#include "engine/systems/service_system.h"
#include "engine/rewinder/frame_storage.h"
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    class RewindStorage;
    /**
    * @class sulphur::engine::HistoryBuffer
    * @brief The rewind system which is used to be able to rewind frames.
    * @todo Implement this with a ring buffer and maybe move this to its own file.
    * @author Raymi Klingers
    */
    class HistoryBuffer
    {
    public:
      /**
      * @brief Adds a frame to the data which pops out the oldest when over its size limit.
      * @param[in] frame (const sulphur::engine::FrameStorage&) The system storage data
      */
      void Add(const FrameStorage& frame);
      /**
      * @brief Adds a frame to the data which pops out the oldest when over its size limit.
      * @param[in] index (size_t) The frame index
      * @return (sulphur::engine::FrameStorage&) The frame with data.
      */
      FrameStorage& operator[](size_t index);
      foundation::Vector<FrameStorage> frame_data_;//<! Storing the data per frame
    };
    /**
    * @struct sulphur::engine::RewindSystem : public sulphur::engine::IServiceSystem <sulphur::engine::RewindSystem>
    * @brief The rewind system which is used as a main control point to rewind frames.
    * @todo Implement missing features as soon as the other systems are implemented which the features depend on.
    * @author Raymi Klingers
    */
    class RewindSystem : public IServiceSystem<RewindSystem>
    {
    public:
      /**
      * @brief The constructor of the system
      */
      RewindSystem();
      /**
      * @see sulphur::engine::ISystemBase::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::ISystemBase::OnTerminate
      */
      void OnTerminate() override;
      /**
      * @brief Adds a system storage reference to the list of system storage data that need to be rewind
      * @param[in] system_storage_data (sulphur::engine::RewindStorage&) The system storage data
      */
      void Register(RewindStorage& system_storage_data );
      /**
      * @brief Function to restore the state of all rewinded systems to the given state.
      */
      void RestoreFrame();
      /**
      * @brief Function to stores the state of all registered systems.
      */
      void StoreFrame();
      /**
      * @brief Function to check if we are rewinding
      * @return (bool) Whether the rewind system is rewinding
      */
      bool IsRewinding();
      /**
      * @brief Function to check if we were rewinding
      * @return (bool) Whether the rewind system was rewinding
      */
      bool WasRewinding();
      /**
      * @brief Stores the currents data in a file on the disk to free up a lot of memory
      * @param[in] system_storage_data (sulphur::engine::SystemStorageData) The file to store the data in
      */
      void StoreToDisk(const char* filename);
      /**
      * @brief Calculates the amount of memory that all the systems consume to store rewind data for the given length of rewind time.
      * @return (size_t) The amount of memory in bytes
      */
      size_t CalculateTotalMemoryUsage();
      /**
      * @brief Calculates the amount of memory that one system consumes to store rewind data for the given length of rewind time.
      * @return (size_t) The amount of memory in bytes
      */
      size_t CalculateSystemMemoryUsage(size_t system);
      /**
      * @brief Simple setter for the frames to skip
      */
      void set_frames_to_skip(size_t frames_to_skip);
      /**
      * @brief Simple getter for the frames to skip
      * @return (size_t) The number of frames to skip.
      */
      size_t frames_to_skip();
      /**
      * @brief Simple setter for the maximum frames 
      */
      void set_frame_limit(size_t frame_limit);
      /**
      * @brief Simple getter for the maximum frames
      * @return (size_t) The maximum frames
      */
      size_t frame_limit();
      /**
      * @brief Simple setter for the frame that needs to be restored
      */
      void set_frame_to_restore(int frame);
      /**
      * @brief Simple getter for the frame that needs to be restored
      * @return (int) The frame
      */
      int frame_to_restore();
      /**
      * @brief Simple setter for the activeness
      */
      void set_active(bool frame);
      /**
      * @brief Simple getter for the activeness
      * @return (bool) The activeness
      */
      bool active();
    private:
      foundation::Vector<HistoryBuffer> systems_frame_data_;//<! A history buffer per system to store its data in.
      foundation::Vector<RewindStorage*> systems_storage_;//<! References to the systems their system storage data.
      size_t frames_to_skip_;//<! Number of frames to skip before storing a frame.
      size_t frame_limit_;//<! Maximum frames stored
      int frame_to_restore_;//<! The frame to restore and is -1 when no frame needs to be restored.
      int prev_restored_frame;//<! The frame to restore and is -1 when no frame needs to be restored.
      bool active_;//< The aciveness of the rewinder
    };
  }
}