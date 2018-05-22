#pragma once
#include "engine/systems/system.h"
#include "engine/systems/system_data.h"
#include "engine/core/handle_base.h"

#include <foundation/utils/type_set.h>

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }

  namespace engine
  {
    class Entity;

    /**
    * @class sulphur::engine::IWorldSystemBase : sulphur::engine::ISystemBase
    * @brief The base class of world systems, which provides its public interface.
    *        A world system is any system who's lifetime matches that of a world.
    * @see sulphur::engine::World
    * @author Raymi Klingers, Maarten ten Velden
    */
    class IWorldSystem : public ISystemBase
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      explicit IWorldSystem(const char* name);

      /**
      * @brief Destroys the data that the handle points to.
      * @param[in] handle (sulphur::engine::ComponentHandleBase) The handle that needs its data to be destroyed
      * @param[in] type (size_t) The type info of the handle.
      * @todo Maybe we could pass it as reference and invalidate the handle as well.
      */
      virtual void Destroy(ComponentHandleBase handle) = 0;

    protected:
      /**
      * @struct sulphur::engine::SparseHandle
      * @brief Strongly typed index to the sparse array
      * @todo Remove the usage of this
      * @remarks Not documenting this as it will be changed and it does not have anything special
      * @author Raymi Klingers
      */
      struct SparseHandle
      {
        static constexpr size_t kInvalidHandle = PS_SIZE_MAX;
        SparseHandle() { handle = kInvalidHandle; }
        SparseHandle(size_t handle) :handle(handle) {}

        bool operator==(const SparseHandle& other) const { return handle == other.handle; }

        size_t handle;
      };
      /**
      * @struct sulphur::engine::DenseHandle
      * @brief Strongly typed index to the sparse array
      * @todo Remove the usage of this
      * @remarks Not documenting this as it will be changed and it does not have anything special
      * @author Raymi Klingers
      */
      struct DenseHandle
      {
        static constexpr size_t kInvalidHandle = PS_SIZE_MAX;
        DenseHandle() { handle = kInvalidHandle; }
        DenseHandle(size_t handle) :handle(handle) {}

        bool operator==(const DenseHandle& other) const { return handle == other.handle; }

        size_t handle;
      };

    };

    /**
    * @class sulphur::engine::IWorldSystemBase : sulphur::engine::ISystemBase
    * @brief The base class of owner systems, which provides its public interface
    *        A owner system is a manager of objects that might contain components,
    *        but don't need to (i.e. they are self-owned).
    * @see sulphur::engine::IComponentSystem
    * @author Maarten ten Velden
    */
    template<typename Handle>
    class IOwnerSystem : public foundation::TypeSet<IWorldSystem, Handle>
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      IOwnerSystem(const char* name);

      /**
      * @brief Creates a handle and allocates the data for it
      * @return (Handle) The created handle
      */
      virtual Handle Create() = 0;
      /**
      * @brief Destroys the component the handle is pointing to
      * @param[in] handle (Handle) The handle that identifies the owner
      * @remarks Reference doesn't matter here and might actually be slower than just a copy
      */
      virtual void Destroy(Handle handle) = 0;

    protected:
      void Destroy(ComponentHandleBase handle) override;

    };

    /**
    * @class sulphur::engine::IComponentSystem <Handle, Data> : sulphur::foundation::TypeSet <sulphur::engine::IComponentSystemBase, Handle>
    * @brief The template class of component systems.  It should be templated on a derived class of @see sulphur::engine::BaseHandle It has an unique ID and
    * @todo Hide functions that should not be called by the user
    * @todo Reorder elements in this class
    * @todo Data template parameter should eventually not default but is there for backwards compatability for now.
    * @author Raymi Klingers
    */
    template<typename Handle, typename Data = int>
    class IComponentSystem : public foundation::TypeSet<IWorldSystem, Handle>
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      IComponentSystem(const char* name);

      /**
      * @brief Creates a handle and allocates the data for it
      * @param[in] entity (sulphur::engine::Entity) The entity that this component will be linked to
      * @todo Entity could be const I think
      * @return (Handle) The created handle
      */
      virtual Handle Create(Entity entity) = 0;
      /**
      * @brief Destroys the component the handle is pointing to
      * @param[in] handle (Handle) The handle that identifies the component
      * @remarks Reference doesn't matter here and might actually be slower than just a copy
      */
      virtual void Destroy(Handle handle) = 0;

    protected:
      void Destroy(ComponentHandleBase handle) override;

      Data component_data_;

    };

    //-------------------------------------------------------------------------
    inline IWorldSystem::IWorldSystem(const char* name) :
      ISystemBase(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename Handle>
    inline IOwnerSystem<Handle>::IOwnerSystem(const char* name) :
      foundation::TypeSet<IWorldSystem, Handle>(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename Handle, typename Data>
    inline IComponentSystem<Handle, Data>::IComponentSystem(const char* name) :
      foundation::TypeSet<IWorldSystem, Handle>(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename Handle>
    inline void IOwnerSystem<Handle>::Destroy(ComponentHandleBase handle)
    {
      Destroy(static_cast<Handle&>(handle));
    }

    //-------------------------------------------------------------------------
    template<typename Handle, typename Data>
    inline void IComponentSystem<Handle, Data>::Destroy(ComponentHandleBase handle)
    {
      Destroy(static_cast<Handle&>(handle));
    }
  }

}