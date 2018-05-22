#pragma once

#include "engine/application/platform.h"
#include "engine/core/world.h"
#include "engine/graphics/irenderer.h"

#include <physics/platform_physics.h>

#include <foundation/job/resource.h>
#include <foundation/job/job_graph.h>

namespace sulphur
{
  namespace foundation
  {
    template<typename T>
    using SharedPointerResource = Resource<SharedPointer<T>>;
  }

  namespace engine
  {
    /**
     * @class sulphur::engine::Application
     * @brief Application class, performing startup of the engine and allowing
     *        for definition of new systems.
     * @author Kenneth Buijssen
     * @todo (Kenneth) discuss with Raymi how to store generic systems
     */
    class Application : public foundation::BaseResource
    {
    public:
      /**
       * @brief Constructor
       * @param[in] window_handle (void*)
       *            Window handle can be provided if the application needs to use a specific handle.
       */
      Application();

      /**
       * @brief Default deconstructor
       */
      virtual ~Application() = default;

      /**
      * @brief Initialize callback
      * @param[in] job_graph (sulphur::foundation::JobGraph&)
      *            Reference to the job graph, used to schedule logic inside the frame
      */
      virtual void OnInitialize(foundation::JobGraph& job_graph);

      /**
       * @brief Initialize callback
       */
      virtual void OnInitialize() {};

      /**
      * @brief Terminate callback
      */
      virtual void OnTerminate() {};

      /**
       * @brief Transfer control to the application, executing the core loop
       */
      void Run();

      /**
      * @brief Get a reference to the active platform-specific renderer
      * @return (sulphur::physics::IRenderer&) A reference to the platform-specific renderer
      */
      IRenderer& platform_renderer()
      {
        return *(renderer_->get());
      }
      /**
      * @brief Get a const-reference to the active platform-specific renderer
      * @return (const sulphur::physics::IRenderer&) A const-reference  to the platform-specific renderer
      */
      const IRenderer& platform_renderer() const
      {
        return *(renderer_->get());
      }

      /**
      * @brief Get a reference to the active platform-specific physics
      * @return (sulphur::physics::PlatformPhysics&) A reference to the platform-specific physics
      */
      physics::PlatformPhysics& platform_physics()
      {
        return *(physics_->get());
      }
      /**
      * @brief Get a const-reference  to the active platform-specific physics
      * @return (const sulphur::physics::PlatformPhysics&) A const-reference  to the platform-specific physics
      */
      const physics::PlatformPhysics& platform_physics() const
      {
        return *(physics_->get());
      }

      /**
      * @brief Get a reference to the active platform object
      * @return (sulphur::engine::Platform&) A reference to the platform object
      */
      Platform& platform()
      {
        return *(platform_);
      }
      /**
      * @brief Get a reference to the active platform object
      * @return (const sulphur::engine::Platform&) A reference to the platform object
      */
      const Platform& platform() const
      {
        return *(platform_);
      }

      /**
      * @brief Get the service system T
      * @return (T&) A reference to the system instance
      * @remarks T must be derived from sulphur::engine::IServiceSystem
      */
      template<typename TSystem>
      TSystem& GetService()
      {
        return services_.Get<TSystem>();
      }
      /**
      * @brief Get the system T in this world
      * @return (T&) A const-reference to the system instance
      * @remarks T must be derived from sulphur::engine::IServiceSystem
      */
      template<typename TSystem>
      const TSystem& GetService() const
      {
        return services_.Get<TSystem>();
      }

    protected:
      foundation::UniquePointer<Platform> platform_;

      foundation::SharedPointerResource<IRenderer> renderer_;
      foundation::SharedPointerResource<physics::PlatformPhysics> physics_;

      SystemSet<IServiceSystemBase> services_;

    };
  }
}
