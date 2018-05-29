#pragma once
#include "engine/core/handle_base.h"
#include "engine/core/world.h"

namespace sulphur
{
  namespace engine
  {
    class Entity;

    class ComponentIDBase
    {
    public:
      virtual ~ComponentIDBase() = default;
      virtual ComponentHandleBase Create(Entity& ent, World& world, void** allocated) = 0;
      virtual ComponentHandleBase Copy(World& world, size_t handle, void** allocated) = 0;
      virtual size_t GetSystemID() = 0;
    };

    template <typename T>
    class ComponentID : public ComponentIDBase
    {

      using System = typename T::System;

    public:

      ComponentHandleBase Create(Entity& ent, World& world, void** allocated) override
      {
        if (allocated == nullptr)
        {
          return ComponentHandleBase::InvalidHandle();
        }

        System& s = GetSystem(world);
        T handle = s.template Create<T>(ent);
        *allocated = AllocationHack(handle);

        return static_cast<ComponentHandleBase>(handle);
      }

      ComponentHandleBase Copy(World& world, size_t handle, void** allocated) override
      {
        if (allocated == nullptr)
        {
          return ComponentHandleBase::InvalidHandle();
        }

        System& s = GetSystem(world);
        T comp = T(s, handle);
        *allocated = AllocationHack(comp);

        return static_cast<ComponentHandleBase>(comp);
      }

      size_t GetSystemID() override
      {
        return foundation::type_id<System>();
      }

    protected:

      System& GetSystem(World& world)
      {
        foundation::typeid_t id = foundation::type_id<System>();
        return static_cast<System&>(world.GetComponent(id));
      }

      void* AllocationHack(const T& handle)
      {
        const size_t size = sizeof(T);
        void* allocated = foundation::Memory::Allocate(size);
        memcpy(allocated, &handle, size);

        return allocated;
      }
    };
  }
}