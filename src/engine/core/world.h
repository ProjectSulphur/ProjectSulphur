#pragma once
#include "engine/systems/service_system.h"
#include "engine/systems/component_system.h"
#include "engine/systems/system_set.h"

#include <foundation/memory/memory.h>
#include <foundation/job/resource.h>

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }

  namespace engine
  {
    class Application;
    class IPhysics;
    class World;

    /**
    * @class sulphur::engine::WorldProviderSystem : sulphur::engine::IServiceSystem <sulphur::engine::WorldProviderSystem>
    * @brief A service system that manages the creation, deletion, and notification of worlds.
    * @see sulphur::engine::World
    * @author Maarten ten Velden
    */
    class WorldProviderSystem : public IServiceSystem<WorldProviderSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      WorldProviderSystem();
      /**
      * @brief Destructor
      */
      ~WorldProviderSystem() override;

      /**
      * @brief Create a new empty world with all systems initialized
      * @param[in] app (sulphur::engine::Application&) The application that is passed to world-systems
      * 
      * @see sulphur::engine::IWorldSystem
      */
      void CreateEmptyWorld(Application& app, foundation::JobGraph& job_graph);
      /**
      * @brief Get the currently active world instance
      * @remarks Asserts if the world is null
      * @return (sulphur::engine::World) The world instance the is currently active
      */
      World& GetWorld();

      /**
      * @see IServiceSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) final;
      /**
      * @see IServiceSystem::OnTerminate
      */
      void OnTerminate() final;
      /**
      * @see IServiceSystem::OnFixedUpdate
      */
      void OnFixedUpdate() final;
      /**
      * @see IServiceSystem::OnUpdate
      */
      void OnUpdate(float delta_time) final;
      /**
      * @see IServiceSystem::OnPreRender
      */
      void OnPreRender() final;
      /**
      * @see IServiceSystem::OnRender
      */
      void OnRender() final;
      /**
      * @see IServiceSystem::OnPostRender
      */
      void OnPostRender() final;

    private:
      World* current_world_;

    };

    /**
    * @class sulphur::engine::World : sulphur::foundation::BaseResource
    * @brief A world represents a scene and all the systems that make up that scene
    *        (transforms, rigid bodies, ...). Do not instantiate directly, but use
    *        the provider system to manage instances of me.
    * @see sulphur::engine::WorldProviderSystem
    * @author Kenneth Buijssen, Maarten ten Velden
    */
    class World : public foundation::BaseResource
    {
    public:
      /**
      * @brief Default constructor
      */
      World();
      /**
      * @brief Destructor
      */
      ~World();

      /**
      * @brief Initialize the world and its systems
      * @param[in] app (sulphur::engine::Application&) The application to pass to the systems
      */
      void Initialize(Application& app, foundation::JobGraph& job_graph);
      /**
      * @brief Terminate the world and its systems
      */
      void Terminate();

      void FixedUpdate();
      void Update(float delta_time);
      void PreRender();
      void Render();
      void PostRender();

      /**
      * @brief Get the system T in this world
      * @return (T&) A reference to the system instance
      * @remarks T must be derived from sulphur::engine::IWorldSystem
      */
      template<typename T>
      T& GetSystem()
      {
        static_assert(std::is_base_of<IWorldSystem, T>::value, "T should be derived from sulphur::engine::IWorldSystem");
        return systems_.Get<T>();
      }
      /**
      * @brief Get the system T in this world
      * @return (T&) A const-reference to the system instance
      * @remarks T must be derived from sulphur::engine::IWorldSystem
      */
      template<typename T>
      const T& GetSystem() const
      {
        static_assert(std::is_base_of<IWorldSystem, T>::value, "T should be derived from sulphur::engine::IWorldSystem");
        return systems_.Get<T>();
      }

      /**
      * @brief Get a system in this world by type-id
      * @param[in] idx (size_t) The index of the system. This can be obtained using
      *                         TSystem::type_id() for all world-systems.
      * @return (IWorldSystem&) A reference to the system instance without type information
      * @see sulphur::foundation::TypeSet
      */
      IWorldSystem& GetSystem(size_t idx)
      {
        return systems_.Get(idx);
      }

    private:
      SystemSet<IWorldSystem> systems_;

    };

  }
}
