#include "engine/rewinder/systems/transform_storage.h"
#include "engine/systems/components/transform_system.h"
namespace sulphur
{
  namespace engine
  { 
    //-------------------------------------------------------------------------
    template<>
    inline void* Store(SparseHandle* buffer, size_t size)
    {
      void* raw_array = foundation::Memory::Allocate(size * sizeof( SparseHandle ), 64);
      memcpy_s(raw_array, size * sizeof( SparseHandle ), buffer, size * sizeof( SparseHandle ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore(SparseHandle* buffer, void* old, size_t size)
    {
       memcpy_s( buffer, size * sizeof( SparseHandle ), old, size * sizeof( SparseHandle ) );
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store(DenseHandle* buffer, size_t size)
    {
      void* raw_array = foundation::Memory::Allocate(size * sizeof( DenseHandle ), 64);
      memcpy_s(raw_array, size * sizeof( DenseHandle ), buffer, size * sizeof( DenseHandle ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore(DenseHandle* buffer, void* old, size_t size)
    {
       memcpy_s( buffer, size * sizeof( DenseHandle ), old, size * sizeof( DenseHandle ) );
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store( TransformSystem::TransformData* buffer, size_t size)
    {      
      void* raw_array = foundation::Memory::Allocate(size*sizeof( TransformSystem::TransformData ), 64);
      memcpy_s(raw_array, size * sizeof( TransformSystem::TransformData ), buffer, size * sizeof( TransformSystem::TransformData ) );
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore( TransformSystem::TransformData* buffer, void* old, size_t size)
    {
       memcpy_s( buffer, size * sizeof( TransformSystem::TransformData ), old, size * sizeof( TransformSystem::TransformData ) );
    }
    //-------------------------------------------------------------------------
    TransformRewindStorage::TransformRewindStorage(TransformSystem& system)
      :
      RewindStorageBase(element_list_, element_sizes_,3),
      storage_(
        reinterpret_cast<RewindStorageBase*>(this),
        StoreFunc<SparseHandle>(),
        StoreFunc<DenseHandle>(),
        StoreFunc<TransformSystem::TransformData>()),
      system_(system)
    {
    }

    //-------------------------------------------------------------------------
    void TransformRewindStorage::PrepareRestore(const FrameStorage& storage )
    {
      system_.sparse_array_.resize( storage.data[0].size );
      system_.dense_to_sparse_array_.resize( storage.data[1].size );
      system_.data_.Get()->resize(storage.data[2].size);
      element_list_[0] = system_.sparse_array_.data();
      element_sizes_[0] = system_.sparse_array_.size();
      element_list_[1] = system_.dense_to_sparse_array_.data();
      element_sizes_[1] = system_.dense_to_sparse_array_.size();
      element_list_[2] = system_.data_.Get()->data();
      element_sizes_[2] = system_.data_.Get()->size();
    }

    //-------------------------------------------------------------------------
    void TransformRewindStorage::PrepareStore()
    {
      element_list_[0] = system_.sparse_array_.data();
      element_sizes_[0] = system_.sparse_array_.size();
      element_list_[1] = system_.dense_to_sparse_array_.data();
      element_sizes_[1] = system_.dense_to_sparse_array_.size();
      element_list_[2] = system_.data_.Get()->data();
      element_sizes_[2] = system_.data_.Get()->size();
    }
  }
}