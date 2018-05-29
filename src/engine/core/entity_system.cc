#include "entity_system.h"

#include "engine/core/component_id.h"
#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/mesh_render_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/scripting/script_system.h"
#include "engine/rewinder/rewind_system.h"
#include "engine/rewinder/systems/entity_storage.h"

#include <lua-classes/entity_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    Application* Entity::application_ = nullptr;

    //-------------------------------------------------------------------------
    void Entity::InjectDependencies(Application& application)
    {
      application_ = &application;
    }

    //-------------------------------------------------------------------------
    ScriptHandle Entity::AddComponent(int id)
    {
      /** @todo (Daniel Konings)
      * This is not ideal, it's a mess, it should be fixed, but we need to make a game.
      * The big problem here is that the ECS was never designed to be compatible with scripting.
      * For some reason everything is copied to avoid "cache misses", but the copying itself
      * is argueably slower anyway..
      * Because the handle is copied, when converting from derived -> base, all data is lost
      */
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      size_t sid = static_cast<size_t>(id);
      ScriptSystem& ss = application_->GetService<ScriptSystem>();
      ComponentIDBase* cid = ss.script_state()->GetComponentIDRegister(sid);

      void* allocated;
      ComponentHandleBase component = cid->Create(*this, world, &allocated);

      system.Link(
        *this,
        component,
        cid->GetSystemID());

      ScriptHandle v =
        ScriptUtils::InstantiateType(ss.script_state(), id, allocated);

      foundation::Memory::Deallocate(allocated);

      return v;
    }

    //-------------------------------------------------------------------------
    void Entity::RemoveComponent(int id)
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      size_t sid = static_cast<size_t>(id);
      ScriptSystem& ss = application_->GetService<ScriptSystem>();
      ComponentIDBase* cid = ss.script_state()->GetComponentIDRegister(sid);

      ComponentHandleBase h = system.GetHandle(*this, cid->GetSystemID());
      void* allocated;
      ComponentHandleBase component = cid->Copy(world, h.Handle(), &allocated);

      system.UnLink(
        *this,
        component,
        cid->GetSystemID());

      foundation::Memory::Deallocate(allocated);
    }

    //-------------------------------------------------------------------------
    ScriptHandle Entity::GetComponent(int id)
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      size_t sid = static_cast<size_t>(id);
      ScriptSystem& ss = application_->GetService<ScriptSystem>();
      ComponentIDBase* cid = ss.script_state()->GetComponentIDRegister(sid);

      ComponentHandleBase h = system.GetHandle(*this, cid->GetSystemID());
      void* allocated;
      cid->Copy(world, h.Handle(), &allocated);

      ScriptHandle v =
        ScriptUtils::InstantiateType(ss.script_state(), id, allocated);

      foundation::Memory::Deallocate(allocated);

      return v;
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
#ifdef PS_EDITOR
      storage_ = foundation::Memory::Construct<EntityRewindStorage>(*this);
      app.GetService<RewindSystem>().Register(storage_->storage_);
#endif
    }

    //-------------------------------------------------------------------------
    void EntitySystem::OnTerminate()
    {
#ifdef PS_EDITOR
      foundation::Memory::Destruct<EntityRewindStorage>(storage_);
#endif
    }

    //-------------------------------------------------------------------------
    Entity EntitySystem::Create(bool editor_owned)
    {
      Entity idx;
      if (free_indices_.size() >= kMinimumFreeIndices_)
      {
        idx.handle = free_indices_.front();
        entity_components_[idx.GetIndex()] = EntityComponentData(editor_owned);
        free_indices_.pop_front();
      }
      else
      {
        generation_.push_back(0);
        entity_components_.push_back(EntityComponentData(editor_owned));
        idx.handle = static_cast<size_t>(generation_.size() - 1);

        PS_LOG_IF(idx.handle >= (1ull << Entity::kIndexBits), Fatal, "Entity limit exceeded");
      }
      idx.handle |= static_cast<size_t>(generation_[idx.handle]) << Entity::kIndexBits;
      return idx;
    }

    //-------------------------------------------------------------------------
    Entity EntitySystem::Create()
    {
      return Create(false);
    }

    //-------------------------------------------------------------------------
    void EntitySystem::Destroy(Entity entity)
    {
      const size_t idx = entity.GetIndex();
      ++generation_[idx];
      free_indices_.push_back(static_cast<uint>(idx));

      EntityComponentData& list = entity_components_[idx];
      for (int i = 0; i < list.component_types.size(); ++i)
      {
        if (list.component_types[i] == foundation::type_id<TransformSystem>())
        {
          TransformSystem& transform_system = reinterpret_cast<TransformSystem&>(
            world_->GetComponent(list.component_types[i]));

          TransformComponent target(transform_system, list.component_handles[i].Handle());
          foundation::Vector<TransformComponent> children = target.GetChildren();
          for (int i_child = 0; i_child < children.size(); ++i_child)
          {
            Destroy(children[i_child].GetEntity());
          }
        }

        world_->GetComponent(list.component_types[i]).Destroy(list.component_handles[i]);
      }

      list.component_handles.clear();
      list.component_types.clear();
    }

    //-------------------------------------------------------------------------
    bool EntitySystem::Alive(Entity entity) const
    {
      return generation_[entity.GetIndex()] == entity.GetGeneration();
    }

    //-------------------------------------------------------------------------
    void EntitySystem::Link(Entity entity, ComponentHandleBase handle, size_t type)
    {
      EntityComponentData& list = entity_components_[entity.GetIndex()];
      list.component_handles.push_back(handle);
      list.component_types.push_back(type);
    }

    //-------------------------------------------------------------------------
    void EntitySystem::UnLink(Entity entity, ComponentHandleBase handle, size_t type)
    {
      EntityComponentData& list = entity_components_[entity.GetIndex()];
      for (int i_type = 0; i_type < list.component_types.size(); ++i_type)
      {
        if (list.component_types[i_type] == type)
        {
          for (int i_handle = i_type; i_handle < list.component_handles.size(); ++i_handle)
          {
            if (list.component_handles[i_handle].handle == handle.handle && list.component_types[i_handle] == type)
            {
              world_->GetComponent(type).Destroy(handle);
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
      const EntityComponentData& list = entity_components_[entity.GetIndex()];
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