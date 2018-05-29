#pragma once
#include "foundation/job/resource.h"
#include "foundation/utils/type_set.h"

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }

  namespace engine
  {
    class Application;

    /**
    * @class sulphur::engine::ISystemBase : sulphur::foundation::ITypeSetBase,
	*          sulphur::foundation::BaseResource
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

      /**
      * @todo Fix this on PS4, Maarten
      */
      //ISystemBase(const ISystemBase&) = delete;
      //void operator=(const ISystemBase&) = delete;

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

    };
  }
}
