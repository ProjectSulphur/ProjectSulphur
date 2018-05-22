#pragma once
#include "foundation/job/resource.h"

namespace sulphur {
  namespace foundation {
    class JobGraph;
  }
}

namespace sulphur
{
  namespace engine
  {
    class Application;
    
    /**
    * @class sulphur::engine::ISystemBase
    * @brief The shared base class of all systems
    * @author Raymi Klingers, Maarten ten Velden
    */
    class ISystemBase : public foundation::BaseResource
    {
    public:
      /**
      * @brief Default constructor
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      ISystemBase(const char* name);
      virtual ~ISystemBase() = default;

      ISystemBase(const ISystemBase&) = delete;
      void operator=(const ISystemBase&) = delete;
      
      /**
      * @internal
      * @brief Initializes the system
      * @param[in] app (sulphur::engine::Application&) The current application
      * @param[in] job_graph (sulphur::foundation::JobGraph&) 
      *            The job graph to schedule updates during the frame
      */
      virtual void OnInitialize(Application& app, foundation::JobGraph& job_graph);

      /**
      * @internal
      * @brief Terminates the system
      */
      virtual void OnTerminate();

      /**
      * @internal
      * @brief Update of the system
      * @param[in] (float) Delta time
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnUpdate(float);
      /**
      * @internal
      * @brief Fixed update of the system
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnFixedUpdate();
      /**
      * @internal
      * @brief Late update of the system
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnLateUpdate();
      /**
      * @internal
      * @brief Pre rendering the system
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnPreRender();
      /**
      * @internal
      * @brief Rendering logic the system
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnRender();
      /**
      * @internal
      * @brief Post rendering logic of the system
      * @deprecated Use jobs instead of overloading this callback
      */
      virtual void OnPostRender();

    };
  }
}
