#include "engine/rewinder/systems/entity_storage.h"
#include "engine/core/entity_system.h"
#include "engine/rewinder/system_stored_data.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    template<>
    inline void* Store(byte* buffer, size_t size)
    {
      void* raw_array = foundation::Memory::Allocate(size*sizeof(byte), 64);
      memcpy_s(raw_array, size, buffer, size);
      return raw_array;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore(byte* /*buffer*/, void* /*old*/, size_t)
    {
      //@ todo implement restoring of the data
    }
    //-------------------------------------------------------------------------
    template<>
    inline void* Store(uint* buffer, size_t)
    {
      return buffer;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore(uint* /*buffer*/, void* /*old*/, size_t)
    {
      //@ todo implement restoring of the data
    }
    /**
    * @struct sulphur::engine::StoredEntityComponentData
    * @brief A struct with the data that is going to be stored for rewinding
    * @todo find a better place for this
    * @author Raymi Klingers
    */
    struct StoredEntityComponentData
    {
      size_t type_size;//<! How many types of components an entity has
      size_t handle_size;//<! How many number of component handles an entity has
      size_t* component_types;//<! The type array
      ComponentHandleBase* handles;//<! The handle array
    };
    //-------------------------------------------------------------------------
    template<>
    inline void* Store(EntityComponentData* buffer, size_t size)
    {
      StoredEntityComponentData* data = static_cast<StoredEntityComponentData*>(foundation::Memory::Allocate(size * sizeof(EntityComponentData), 64));
      for (int i = 0; i < size; ++i)
      {
        data[i].type_size = buffer[i].component_types.size();
        data[i].component_types = static_cast<size_t*>(foundation::Memory::Allocate(data[i].type_size * sizeof(size_t), 64));
        for (int dataI = 0; dataI < data[i].type_size; ++dataI)
        {
          data[i].component_types[dataI] = buffer[i].component_types[dataI];
        }
        data[i].handle_size = buffer[i].component_types.size();
        data[i].handles = static_cast<ComponentHandleBase*>(foundation::Memory::Allocate(data[i].type_size * sizeof(ComponentHandleBase), 64));
        for (int dataI = 0; dataI < data[i].type_size; ++dataI)
        {
          data[i].handles[dataI] = buffer[i].component_handles[dataI];
        }
      }
      return data;
    }
    //-------------------------------------------------------------------------
    template<>
    inline void Restore(EntityComponentData* /*buffer*/, void* /*old*/ , size_t)
    {
      //@ todo implement restoring of the data
    }
    //--------------------------------------------------------------------------
    EntityRewindStorage::EntityRewindStorage(EntitySystem& system):
      RewindStorageBase(element_list_, element_sizes_,3),
      storage_(RewindStorage(
        reinterpret_cast<RewindStorageBase*>(this),
        StoreFunc<byte>(),
        StoreFunc<uint>(),
        StoreFunc<EntityComponentData>())),
      system_(system)
    {
    }

    //--------------------------------------------------------------------------
    void EntityRewindStorage::PrepareRestore( const FrameStorage& )
    {
    }
    //--------------------------------------------------------------------------
    void EntityRewindStorage::PrepareStore()
    {
      element_list_[0] = system_.generation_.data();
      element_sizes_[0] = system_.generation_.size();
      element_sizes_[1] = system_.free_indices_.size();
      if (system_.free_indices_.size() != 0)
      {
        uint* raw_array = static_cast<uint*>(foundation::Memory::Allocate(
          system_.free_indices_.size(),
          64));
        size_t iterator = 0;
        for (uint& i : system_.free_indices_)
        {
          raw_array[iterator] = i;
          ++iterator;
        }
        element_list_[1] = raw_array;
      }
      else
      {
        element_list_[1] = nullptr;
      }
      element_list_[2] = system_.entity_components_.data();
      element_sizes_[2] = system_.entity_components_.size();
    }
  }
}