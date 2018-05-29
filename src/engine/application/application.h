#pragma once
#include "engine/application/platform.h"
#include "engine/systems/system_set.h"
#include "engine/systems/service_system.h"

#include <foundation/job/resource.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;

    template<typename T>
    using SharedPointerResource = Resource<SharedPointer<T>>;
  }

  namespace physics
  {
    class BulletPhysics;
    using PlatformPhysics = BulletPhysics; //!< Platform specification of the library interface implementation
  }

  namespace engine
  {
    class IEditorHook;
    class IRenderer;
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
      * @param[in | opt] argc (int) The number of arguments passed into main, default = 0
      * @param[in | opt] argv (char**) The arguments passed into main, default = nullptr
      */
      virtual void Run(int argc = 0, char** argv = nullptr);

      /**
      * @brief Get a reference to the active platform-specific renderer
      * @return (sulphur::physics::IRenderer&) A reference to the platform-specific renderer
      */
      IRenderer& platform_renderer();

      /**
      * @brief Get a const-reference to the active platform-specific renderer
      * @return (const sulphur::physics::IRenderer&) A const-reference  to the platform-specific renderer
      */
      const IRenderer& platform_renderer() const;

      /**
      * @brief Get a reference to the active platform-specific physics
      * @return (sulphur::physics::PlatformPhysics&) A reference to the platform-specific physics
      */
      physics::PlatformPhysics& platform_physics();

      /**
      * @brief Get a const-reference  to the active platform-specific physics
      * @return (const sulphur::physics::PlatformPhysics&) A const-reference  to the platform-specific physics
      */
      const physics::PlatformPhysics& platform_physics() const;

      /**
      * @brief Get a reference to the active platform object
      * @return (sulphur::engine::Platform&) A reference to the platform object
      */
      Platform& platform();

      /**
      * @brief Get a const-reference to the active platform object
      * @return (const sulphur::engine::Platform&) A const-reference to the platform object
      */
      const Platform& platform() const;

      /**
      * @returns (const sulphur::foundation::String&) Returns the directory (path) where the project's assets are located.
      */
      const foundation::Path& project_directory() const;

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

      /**
      * @brief Get the hook to the editor specific logic of the application.
      */
      IEditorHook* editor_hook();

      /**
      * @brief Set the new directory to use as a project directory. This will reinitialize all systems that use the project directory to load assets from such as the script system.
      * @param[in] directory_path (sulphur::foundation::Path&) The path to the directory containing the cache files of the project.
      */
      void SetProjectDirectory(foundation::Path& directory_path);

    protected:
      /**
      * @brief Stores a path in which all the assets of the project are located
      */
      foundation::Path project_directory_;
      /**
      * @brief Low level platform
      */
      foundation::UniquePointer<Platform> platform_;
      /**
      * @brief Platform independent rendering interface
      */
      foundation::SharedPointerResource<IRenderer> renderer_;

      /**
      * @brief Platform independent physics interface
      */
      foundation::SharedPointerResource<physics::PlatformPhysics> physics_;

      /**
      * @brief Collection of all services currently registered to the application
      */
      SystemSet<IServiceSystemBase> services_;

    private:
      /**
      * @brief Class used to execute editor specific logic in the application.
      */
      IEditorHook* editor_hook_;
    };
  }
}
