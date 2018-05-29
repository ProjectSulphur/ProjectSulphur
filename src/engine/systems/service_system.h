#pragma once

#include "engine/systems/system.h"

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

      /**
      * @internal
      * @brief Call-back which is called after everything system in the caller
      *        application has been terminated (i.e. termination of the
      *        platform-layer).
      */
      virtual void OnShutdown();

    };

    /**
    * @class sulphur::engine::IServiceSystem <T> : sulphur::foundation::TypeSet <sulphur::engine::IServiceSystemBase, T>
    * @brief
    * @author Maarten ten Velden
    */
    template<typename T>
    class IServiceSystem : public IServiceSystemBase
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
      IServiceSystemBase(name)
    {
    }
  }
}
