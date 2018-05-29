#pragma once
#include "engine/rewinder/rewindable_storage_base.h"
#include "engine/rewinder/system_stored_data.h"

namespace sulphur
{
  namespace engine
  {
    class TransformSystem;
    /**
    * @class sulphur::engine::TransformRewindStorage : sulphur::engine::TransformRewindStorage
    * @brief A class that glues together the rewind storage system with the transform storage as it has a different structure.
    * @author Raymi Klingers
    */
    class TransformRewindStorage : public RewindStorageBase
    {
    public:
      /**
      * @brief Constructor that takes the system to get its data from
      * @param[in] system (sulphur::engine::TransformSystem&) The transform system that this wraps
      */
      TransformRewindStorage(TransformSystem& system);
      /**
      * @see sulphur::engine::RewindStorageBase::PrepareRestore
      */
      virtual void PrepareRestore( const FrameStorage& storage ) override;

      /**
      * @see sulphur::engine::RewindStorageBase::PrepareStore
      */
      virtual void PrepareStore() override;
    public:
      void* element_list_[3];//<! List of the data that needs to be stored, in this case the entity component data, generation and freelist which will be cut down later on
      size_t element_sizes_[3];//<! The size of the arrays
      RewindStorage storage_;//<! The system that stores the obtained data
      TransformSystem& system_;//<! Reference to the system to obtain the data
    };
  }
}