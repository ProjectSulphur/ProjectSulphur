#include "engine/rewinder/system_stored_data.h"
#include "engine/rewinder/frame_storage.h"
#include "engine/rewinder/rewindable_storage_base.h"
#include "engine/systems/system_data.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {    
    //-------------------------------------------------------------------------
    template<>
    inline void* Store( SystemDataBase* storage, size_t )
    {
      size_t element_sizes[]{
        storage->sparse_array_.size() * sizeof( SparseHandle ),
        storage->dense_to_sparse_array_.size() * sizeof( DenseHandle ),
        storage->generation_.size() * sizeof( unsigned char ),
        storage->free_list_.size() * sizeof( size_t )
      };
      // Buffer sizes of the container [sparse, dense to sparse, generation, freelist]
      size_t buffer_size = sizeof( size_t ) * 4 + element_sizes[0] + element_sizes[1] + element_sizes[2] + element_sizes[3];
      void* raw_array = foundation::Memory::Allocate( buffer_size, 64 );
      void* start = raw_array;
      for ( int i = 0; i < 4; ++i )
      {
        *reinterpret_cast< size_t* >( raw_array ) = element_sizes[i];
        raw_array = reinterpret_cast< size_t* >( raw_array ) + 1;
      }

      // Those data buffers
      void* buffers[]
      {
        storage->sparse_array_.data(),
        storage->dense_to_sparse_array_.data(),
        storage->generation_.data(),
        storage->free_list_.data()
      };
      for ( int i = 0; i < 4; ++i )
      {
        memcpy_s( raw_array,
          element_sizes[i],
          buffers[i],
          element_sizes[i] );
        raw_array = reinterpret_cast< char* >( raw_array ) + element_sizes[i];
      }
      return start;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore( SystemDataBase* storage, void*old, size_t  )
    {
      // Restore old buffer sizes
      size_t* buffer_sizes = reinterpret_cast< size_t* >( old );
      storage->sparse_array_.resize( buffer_sizes[0] );
      storage->dense_to_sparse_array_.resize( buffer_sizes[1] );
      storage->generation_.resize( buffer_sizes[2] );
      storage->free_list_.resize( buffer_sizes[3] );
      void* data = buffer_sizes + 4;
      void* buffers[]
      {
        storage->sparse_array_.data(),
        storage->dense_to_sparse_array_.data(),
        storage->generation_.data(),
        storage->free_list_.data()
      };
      for ( int i = 0; i < 4; ++i )
      {
        memcpy_s( buffers[i],
          buffer_sizes[i],
          data,
          buffer_sizes[i] );
        data = reinterpret_cast< char* >( data ) + buffer_sizes[i];
      }
    }
    //-------------------------------------------------------------------------
    FrameStorage RewindStorage::Store()
    {
      storage_data_->PrepareStore();
      element_list_ = storage_data_->element_list_;
      FrameStorage storage;
      for (int i = 0; i < store_functions_.size(); ++i)
      {
         storage.data.push_back( FrameData{ store_functions_[i]( element_list_[i],
            storage_data_->element_sizes_[i] ), storage_data_->element_sizes_[i] } );
      }
      return storage;
    }
    //-------------------------------------------------------------------------
    void RewindStorage::Restore(const FrameStorage& storage)
    {
      storage_data_->PrepareRestore(storage);
      for (int i = 0; i < restore_functions_.size(); ++i)
      {
        restore_functions_[i](element_list_[i], storage.data[i].data, storage.data[i].size);
      }
    }
    //-------------------------------------------------------------------------
    void RewindStorage::AddSystemStorageFunction()
    {
      AddFunction<0>( StoreFunc<SystemDataBase>() );
    }
  }
}