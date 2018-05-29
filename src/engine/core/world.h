#pragma once
#include "engine/systems/service_system.h"
#include "engine/systems/owner_system.h"
#include "engine/systems/component_system.h"
#include "engine/systems/system_set.h"

#include "engine/scripting/scriptable_object.h"

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
      * @param[in] job_graph (sulphur::foundation::JobGraph&) 
      *            Job graph used to schedule execution of jobs during the frame
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

    private:

      World* current_world_; //!< The current world

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
      * @param[in] job_graph (sulphur::foundation::JobGraph&) Job graph used to schedule execution of jobs during the frame
      */
      void Initialize(Application& app, foundation::JobGraph& job_graph);
      /**
      * @brief Terminate the world and its systems
      */
      void Terminate();

      /**
      * @brief Get the self-owned system T in this world
      * @return (T&) A reference to the system instance
      * @remarks T must be derived from sulphur::engine::IOwnerSystemBase
      */
      template<typename T>
      T& GetOwner()
      {
        static_assert(std::is_base_of<IOwnerSystemBase, T>::value,
          "T should be derived from sulphur::engine::IOwnerSystemBase");
        return owners_.Get<T>();
      }
      /**
      * @brief Get the self-owned system T in this world
      * @return (T&) A const-reference to the system instance
      * @remarks T must be derived from sulphur::engine::IOwnerSystemBase
      */
      template<typename T>
      const T& GetOwner() const
      {
        static_assert(std::is_base_of<IOwnerSystemBase, T>::value,
          "T should be derived from sulphur::engine::IOwnerSystemBase");
        return owners_.Get<T>();
      }

      /**
      * @brief Get a self-owned system in this world by type-id
      * @param[in] idx (size_t) The index of the system. This can be obtained using
      *                         TSystem::type_id() for all self-owned systems.
      * @return (sulphur::engine::IOwnerSystemBase&) A reference to the system instance without
      *   type information
      * @see sulphur::foundation::TypeSet
      */
      IOwnerSystemBase& GetOwner(size_t idx)
      {
        return owners_.Get(idx);
      }

      /**
      * @brief Get the component system T in this world
      * @return (T&) A reference to the system instance
      * @remarks T must be derived from sulphur::engine::IComponentSystemBase
      */
      template<typename T>
      T& GetComponent()
      {
        static_assert(std::is_base_of<IComponentSystem, T>::value,
          "T should be derived from sulphur::engine::IComponentSystem");
        return components_.Get<T>();
      }
      /**
      * @brief Get the component system T in this world
      * @return (T&) A const-reference to the system instance
      * @remarks T must be derived from sulphur::engine::IComponentSystemBase
      */
      template<typename T>
      const T& GetComponent() const
      {
        static_assert(std::is_base_of<IComponentSystem, T>::value,
          "T should be derived from sulphur::engine::IComponentSystem");
        return components_.Get<T>();
      }

      /**
      * @brief Get a component system in this world by type-id
      * @param[in] idx (size_t) The index of the component system. This can be obtained using
      *                         TSystem::type_id() for all component systems.
      * @return (sulphur::engine::IComponentSystem&) A reference to the system instance without
      *   type information
      */
      IComponentSystem& GetComponent(size_t idx)
      {
        return components_.Get(idx);
      }

    private:
      SystemSet<IOwnerSystemBase> owners_; //!< A unique set of all owner systems in this world
      SystemSet<IComponentSystem> components_; //!< A unique set of all component systems in this world

    };

    /**
    * @class sulphur::engine::ScriptableWorld : public sulphur::engine::ScriptableObject
    * @brief Used to statically wrap world functionalities in Lua
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptableWorld : public ScriptableObject
    {

    public:

      /**
      * @brief Default constructor
      */
      ScriptableWorld();

      /**
      * @brief Default constructor
      * @param[in] world (sulphur::engine::World*) The world contained in this ScriptableWorld
      */
      ScriptableWorld(World* world);

      /**
      * @brief Initializes that static properties of the scriptable world
      * @param[in] system (sulphur::engine::WorldProviderSystem*) The world provider system
      * @param[in] app (sulphur::engine::Application*) The application
      * @param[in] graph (sulphur::foundation::JobGraph*) The job graph
      */
      static void Initialize(
        WorldProviderSystem* system,
        Application* app);

      SCRIPT_NAME(World);

      /**
      * @brief Creates a world and returns it using the WorldProviderSystem
      * @return (sulphur::engine::ScriptableWorld*) The created world
      */
      SCRIPT_FUNC(static) ScriptableWorld Create();

      /**
      * @brief Creates an entity using this world
      * @return (sulphur::engine::Entity) The handle to the created entity
      */
      SCRIPT_FUNC() Entity CreateEntity() const;

      /**
      * @brief Destroys an entity using this world
      * @param[in] ent (sulphur::engine::ScriptHandle) The entity to destroy
      */
      SCRIPT_FUNC() void DestroyEntity(ScriptHandle ent) const;

    protected:

      /**
      * @return (bool) Is the ScriptableWorld interface initialized and ready to be used?
      */
      static bool IsValid();

    private:

      World* world_; //!< A reference to the world contained within this ScriptableWorld

      static WorldProviderSystem* system_; //!< A static reference to the world provider system
      static Application* app_; //!< A static reference to the application
    };
  }
}
