#pragma once

#include "engine/application/application.h"
#include "engine/core/entity_component.h"
#include "engine/core/handle_base.h"
#include "engine/core/world.h"

#include <foundation/utils/type_definitions.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>

namespace sulphur
{
  namespace engine
  {
    class EntitySystem;
    class EntityRewindStorage;

    /**
    * @class sulphur::engine::Entity : sulphur::engine::ComponentHandleBase
    * @brief The entity that is used by the component systems. You can attach or remove components from this entity and it will automatically communicate with the entity manager and component system
    * @todo Hide functions that should not be called by the user
    * @todo Decide on 64 bit or 32 bit
    * @todo Move the definitions of variables and functions around
    * @todo Move this into the component folder? Or its own folder
    * @todo Check over the functions as many could be const
    * @author Raymi Klingers
    */
    SCRIPT_CLASS() class Entity : public ComponentHandleBase
    {
    public:

      using System = EntitySystem;

      SCRIPT_NAME(Entity);

      /**
      * @brief Adds a component by type ID as in the script state
      * @param[in] id (int) The ID of the component to add
      * @return (sulphur::engine::ScriptHandle) The created component as a script value
      */
      SCRIPT_FUNC() ScriptHandle AddComponent(int id);

      /**
      * @brief Adds a component to the entity
      * @return (sulphur::engine::Component) The added component
      */
      template<typename Component>
      Component Add();

      /**
      * @brief Removes a component by type ID as in the script state
      * @param[in] id (int) The ID of the component to remove
      */
      SCRIPT_FUNC() void RemoveComponent(int id);

      /**
      * @brief Removes a component from the entity
      * @param[in] handle (sulphur::engine::Component) The component to remove
      * @todo Make a remove type function as we won't allow for multiple components of the same type per entity.
      */
      template<typename Component>
      void Remove(Component handle);

      /**
      * @brief Gets a component by type ID as in the script state
      * @param[in] id (int) The ID of the component to get
      * @return (sulphur::engine::ScriptHandle) The component as a script value
      */
      SCRIPT_FUNC() ScriptHandle GetComponent(int id);

      /**
      * @brief Gets a component from the entity
      * @return (sulphur::engine::Component) The component if the entity has it else an invalid handle
      */
      template<typename Component>
      Component Get() const;
      /**
      * @brief Checks if the entity has this type of component
      * @return (bool) Does the entity have this type of component?
      */
      template<typename Component>
      bool Has() const;

      /**
      * @brief Inject the application class as a static resource for all entities to use
      * @param[in] application (sulphur::engine::Application&) The application instance to use
      * @remarks Note that this is a dirty work-around for proper dependency injection. The reason
      *          that this is here is because sulphur::engine::SystemData relies on entities being
      *          default-constructible, which makes this very difficult to eliminate completely.
      */
      static void InjectDependencies(Application& application);

    private:
      static Application* application_;

    };

    /**
    * @class sulphur::engine::EntitySystem :
    *        sulphur::engine::IOwnerSystem <sulphur::engine::Entity>
    * @brief Handles the entity lifetime and the linking of the components with entities.
    * @todo Hide functions that should not be called on the entity system but just on the entity.
    * @todo Move this into the component folder? Or its own folder
    * @author Raymi Klingers
    */
    class EntitySystem : public IOwnerSystem<Entity>
    {
      static constexpr uint kMinimumFreeIndices_ = 1024u; //!< Number of free indices before we start to reuse entity slots

      friend EntityRewindStorage;
    public:
      /**
       * @brief Default constructor
       */
      EntitySystem();

      /**
      * @see sulphur::engine::IOwnerSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::IOwnerSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Creates a new entity with proper generation and entity-component link storage.
      * @return (sulphur::engine::Entity) A new entity.
      */
      Entity Create();
      /**
      * @brief Adds the entity to the destruction list to be destroyed at the end of the frame.
      * @param[in] entity (sulphur::engine::Entity) The entity to destroy.
      */
      void Destroy(Entity entity);

      /**
      * @brief Checks if the entity is alive.
      * @param[in] entity (sulphur::engine::Entity) The entity to check.
      * @return (bool) is the entity alive?
      */
      bool Alive(Entity entity) const;
      /**
      * @brief Stores the link between the entity and the component.
      * @param[in] entity (sulphur::engine::Entity) The entity to store the link of.
      * @param[in] handle (sulphur::engine::ComponentHandleBase) The base handle without the type info.
      * @param[in] type (size_t) The type info of the handle.
      * @todo param type should probably be its own type. ComponentHandleType or something.
      */
      void Link(Entity entity, ComponentHandleBase handle, size_t type);
      /**
      * @brief Removes the link between the entity and the component and destroying the component.
      * @param[in] entity (sulphur::engine::Entity) The entity to remove the link of.
      * @param[in] handle (sulphur::engine::ComponentHandleBase) The base handle without the type info.
      * @param[in] type (size_t) The type info of the handle.
      * @todo param type should probably be its own type. ComponentHandleType or something.
      */
      void UnLink(Entity entity, ComponentHandleBase handle, size_t type);
      /**
      * @brief Stores the link between the entity and the component.
      * @param[in] entity (sulphur::engine::Entity) The entity to obtain the handle from.
      * @param[in] type (size_t) The type info of the handle.
      * @return (sulphur::foundation::ComponentHandleBase) The handle to the component. Returns an invalid handle if the entity doesn't have the component
      * @todo param type should probably be its own type. ComponentHandleType or something.
      */
      ComponentHandleBase GetHandle(Entity entity, size_t type) const;

      /**
      * @brief Destroys the entities that were marked for destruction, increments the generations and removes the links and destroys the components.
      * @param[in] entity (size_t) The entity index to destroy.
      */
      void DestroyMarkedForDestruction();
    private:

      /**
      * @brief Adds the entity to be destroyed immediately
      * @param[in] entity (sulphur::engine::Entity) The entity to destroy.
      */
      void DestroyImmediate( size_t index );
      /**
      * @brief Creates a new entity that is owned by the editor
      * @param[in] with_editor (bool) Setting this to true indicates the editor instantiated this
      *                               entity, rather than the local application
      * @return (sulphur::engine::Entity) A new entity.
      */
      Entity Create(bool with_editor);

      World* world_; //!< A pointer to the world that this system is a part of
      EntityRewindStorage* storage_;//<! A class to glue the data systems of the rewinder together


      foundation::Vector<byte> generation_;//!< Stores the current generation of the entity which is used in the Alive function @see sulphur::engine::EntitySystem::Alive.
      foundation::Deque<uint> free_indices_;//!< Stores free entity slots for reuse.
      foundation::Vector<EntityComponentData> entity_components_;//!< Stores the linking information of the components.
      foundation::Vector<size_t> to_destroy;//!< Stores entity indices that need to be destroyed.
    };

    //-------------------------------------------------------------------------
    template<typename Component>
    inline Component Entity::Add()
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      Component component = (static_cast<typename Component::System&>(
        world.GetComponent(foundation::type_id<typename Component::System>())
        )).template
        Create<Component>(*this);

      system.Link(
        *this,
        *static_cast<ComponentHandleBase*>(&component),
        foundation::type_id<typename Component::System>());

      return component;
    }
    
    //-------------------------------------------------------------------------
    template<typename Component>
    inline void Entity::Remove(Component handle)
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      system.UnLink(
        *this,
        *static_cast<ComponentHandleBase*>(&handle),
        foundation::type_id<typename Component::System>());
    }

    //-------------------------------------------------------------------------
    template<typename Component>
    inline Component Entity::Get() const
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      ComponentHandleBase avoid_error = system.GetHandle(*this, foundation::type_id<typename Component::System>());
      return Component(world.GetComponent<typename Component::System>(), avoid_error.Handle());
    }
    
    //-------------------------------------------------------------------------
    template<typename Component>
    inline bool Entity::Has() const
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetOwner<EntitySystem>();

      ComponentHandleBase avoid_error = system.GetHandle(*this, foundation::type_id<typename Component::System>());
      return avoid_error != Component::InvalidHandle();
    }
  }
}
