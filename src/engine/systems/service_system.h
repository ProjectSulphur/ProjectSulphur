#pragma once

#include "engine/systems/system.h"

#include <foundation/utils/type_set.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::IServiceSystemBase : sulphur::engine::ISystemBase
    * @brief The base class of service systems, which provides its public interface
    * @author Maarten ten Velden
    */
    class IServiceSystemBase : public ISystemBase
    {
    public:
      /**
      * @brief Default constructor
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      IServiceSystemBase(const char* name);

    };

    /**
    * @class sulphur::engine::IServiceSystem <T> : sulphur::foundation::TypeSet <sulphur::engine::IServiceSystemBase, T>
    * @brief
    * @author Maarten ten Velden
    */
    template<typename T>
    class IServiceSystem : public foundation::TypeSet<IServiceSystemBase, T>
    {
    public:
      /**
      * @brief Default constructor
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      IServiceSystem(const char* name);

    };

    //-------------------------------------------------------------------------
    inline IServiceSystemBase::IServiceSystemBase(const char* name) :
      ISystemBase(name)
    {
    }

    //-------------------------------------------------------------------------
    template<typename T>
    inline IServiceSystem<T>::IServiceSystem(const char* name) :
      foundation::TypeSet<IServiceSystemBase, T>(name)
    {
    }
  }
}
