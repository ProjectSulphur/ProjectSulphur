#include "entity_system.h"

#include "engine/application/application.h"
#include "engine/core/world.h"
#include "engine/systems/component_system.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    Application* Entity::application_ = nullptr;

    //-------------------------------------------------------------------------
    void Entity::InjectDependencies(Application& application)
    {
      application_ = &application;
    }

    //-------------------------------------------------------------------------
    EntitySystem::EntitySystem() :
      IOwnerSystem("EntitySystem")
    {
    }

    //-------------------------------------------------------------------------
    void EntitySystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      // @note (Maarten) This is a very-dirty work around for a rather complex issue.
      //                 See the documentation of the function below for more info.
      Entity::InjectDependencies(app);

      world_ = &app.GetService<WorldProviderSystem>().GetWorld();
    }

    //-------------------------------------------------------------------------
    Entity EntitySystem::Create()
    {
      Entity idx;
      if (free_indices.size() >= kMinimumFreeIndices_)
      {
        idx.handle = free_indices.front();
        free_indices.pop_front();
      }
      else
      {
        generation.push_back(0);
        entity_components.push_back(EntityComponentData());
        idx.handle = static_cast<size_t>(generation.size() - 1);
        //assert( idx.handle < ( 1 << Entity::kIndexBits ) );// If this ever happens we're in trouble
      }
      idx.handle |= static_cast<size_t>(generation[idx.handle]) << Entity::kIndexBits;
      return idx;
    }

    //-------------------------------------------------------------------------
    void EntitySystem::Destroy(Entity entity)
    {
      const size_t idx = entity.GetIndex();
      ++generation[idx];
      free_indices.push_back(static_cast<uint>(idx));

      const EntityComponentData& list = entity_components[idx];
      for (int i = 0; i < list.component_types.size(); ++i)
      {
        world_->GetSystem(list.component_types[i]).Destroy(list.component_handles[i]);
      }
    }

    //-------------------------------------------------------------------------
    bool EntitySystem::Alive(Entity entity) const
    {
      return generation[entity.GetIndex()] == entity.GetGeneration();
    }

    //-------------------------------------------------------------------------
    void EntitySystem::Link(Entity entity, ComponentHandleBase handle, size_t type)
    {
      EntityComponentData& list = entity_components[entity.GetIndex()];
      list.component_handles.push_back(handle);
      list.component_types.push_back(type);
    }

    //-------------------------------------------------------------------------
    void EntitySystem::UnLink(Entity entity, ComponentHandleBase handle, size_t type)
    {
      EntityComponentData& list = entity_components[entity.GetIndex()];
      for (int i_type = 0; i_type < list.component_types.size(); ++i_type)
      {
        if (list.component_types[i_type] == type)
        {
          for (int i_handle = i_type; i_handle < list.component_handles.size(); ++i_handle)
          {
            if (list.component_handles[i_handle].handle == handle.handle && list.component_types[i_handle] == type)
            {
              world_->GetSystem(type).Destroy(handle);
              // Swap with back and pop
              list.component_handles[i_handle] = list.component_handles[list.component_handles.size() - 1];
              list.component_types[i_type] = list.component_types[list.component_types.size() - 1];
              list.component_handles.pop_back();
              list.component_types.pop_back();
              return;
            }
          }
        }
      }
    }

    //-------------------------------------------------------------------------
    ComponentHandleBase EntitySystem::GetHandle(Entity entity, size_t type) const
    {
      const EntityComponentData& list = entity_components[entity.GetIndex()];
      for (int i_type = 0; i_type < list.component_types.size(); ++i_type)
      {
        if (list.component_types[i_type] == type)
        {
          return  list.component_handles[i_type];
        }
      }
      return ComponentHandleBase::InvalidHandle();
    }
  }
}