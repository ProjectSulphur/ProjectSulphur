#pragma once

#include "engine/systems/system.h"
#include "engine/systems/system_data.h"


namespace sulphur
{
  namespace engine
  {
    class Entity;

    /**
    * @class sulphur::engine::IComponentSystem
    * @brief The base class of components systems, which provides its public interface.
    * @details An owner system is a manager of objects that are components, meaning that they are
    *   dependent on, both for their lifetime is and proper functioning, on other objects.
    * @tparam[in] Handle (typename) The concrete handle used to add, remove, and look-up individual
    *   components. This handle should be derived from sulphur::engine::BaseHandle This has to be
    *   unique for each system to provide RTTI functionality.
    * @tparam[in] Data (typename) The container that stores the per-component data which is an
    *   instance of sulphur::engine::SystemData
    * @todo Data template parameter should eventually not default but is there for backwards
    *   compatability for now.
    * @author Raymi Klingers
    */
    class IComponentSystem : public ISystemBase
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*) The resource name for the system
      */
      IComponentSystem(const char* name);

      /**
      * @brief Creates a handle and allocates the data for it
      * @param[in] entity (sulphur::engine::Entity) The entity that this component will be
      *   linked to.
      * @return (Handle) The created handle
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity);
      /**
      * @brief Destroys the component the handle is pointing to
      * @param[in] handle (Handle) The handle that identifies the component
      * @remarks Reference doesn't matter here and might actually be slower than just a copy
      */
      virtual void Destroy(ComponentHandleBase handle) = 0;
    };
    
    //-------------------------------------------------------------------------
    inline IComponentSystem::IComponentSystem(const char* name) :
      ISystemBase(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename ComponentT>
    inline ComponentT IComponentSystem::Create(Entity&)
    {
      PS_LOG(Fatal, "Please override the Create function!");
      return ComponentT();
    }
  }
}