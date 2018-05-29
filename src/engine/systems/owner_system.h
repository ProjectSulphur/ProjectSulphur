#pragma once

#include "engine/systems/system.h"
#include "engine/systems/system_set.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::IOwnerSystemBase : sulphur::engine::ISystemBase
    * @brief The base class of world systems, which provides its public interface.
    * @details A world system is any system who's lifetime matches that of a world.
    * @see sulphur::engine::World
    * @author Raymi Klingers, Maarten ten Velden
    */
    class IOwnerSystemBase : public ISystemBase
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      explicit IOwnerSystemBase(const char* name);

    };

    /**
    * @class sulphur::engine::IOwnerSystem : sulphur::engine::TypeSet
    *   <sulphur::foundation::TypeSet <sulphur::foundation::IOwnerSystemBase, T>
    * @brief The base class of owner systems, which provides its public interface.
    * @details An owner system is a manager of objects that might contain components, but don't
    *   need to (i.e. they are self-owned).
    * @tparam[in] T (typename) The concrete type of the system to provide RTTI functionality.
    * @see sulphur::engine::IComponentSystem
    * @author Maarten ten Velden
    */
    template<typename T>
    class IOwnerSystem : public IOwnerSystemBase
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      IOwnerSystem(const char* name);

    };

    //-------------------------------------------------------------------------
    inline IOwnerSystemBase::IOwnerSystemBase(const char* name) :
      ISystemBase(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename T>
    inline IOwnerSystem<T>::IOwnerSystem(const char* name) :
      IOwnerSystemBase(name)
    {
    }
  }

}