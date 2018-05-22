#pragma once

#include "engine/application/application.h"
#include "engine/systems/component_system.h"
#include "engine/core/entity_component.h"
#include "engine/core/handle_base.h"
#include "engine/core/world.h"

#include <foundation/utils/generation_handle.h>
#include <foundation/utils/type_definitions.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>

namespace sulphur
{
  namespace engine
  {
    class EntitySystem;

    /**
    * @class sulphur::engine::Entity : public sulphur::engine::ComponentHandleBase
    * @brief The entity that is used by the component systems. You can attach or remove components from this entity and it will automatically communicate with the entity manager and component system
    * @todo Hide functions that should not be called by the user
    * @todo Decide on 64 bit or 32 bit
    * @todo Move the definitions of variables and functions around
    * @todo Move this into the component folder? Or its own folder
    * @todo Check over the functions as many could be const
    * @author Raymi Klingers
    */
    class Entity : public ComponentHandleBase
    {
    public:
      using System = EntitySystem;

      /**
      * @brief Adds a component to the entity
      * @return (sulphur::engine::Component) The added component
      */
      template<typename Component>
      Component Add();
      /**
      * @brief Removes a component from the entity
      * @param[in] handle (sulphur::engine::Component) The component to remove
      * @todo Make a remove type function as we won't allow for multiple components of the same type per entity.
      */
      template<typename Component>
      void Remove(Component handle);
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
    * @class sulphur::engine::EntitySystem : public sulphur::engine::IOwnerSystem <sulphur::engine::EntitySystem>
    * @brief Handles the entity lifetime and the linking of the components with entities.
    * @todo Hide functions that should not be called on the entity system but just on the entity.
    * @todo Move this into the component folder? Or its own folder
    * @author Raymi Klingers
    */
    class EntitySystem : public IOwnerSystem<Entity>
    {
      static constexpr uint kMinimumFreeIndices_ = 1024u; //!< Number of free indices before we start to reuse entity slots

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
      * @brief Creates a new entity with proper generation and entity-component link storage.
      * @return (sulphur::engine::Entity) A new entity.
      */
      Entity Create() override;
      /**
      * @brief Destroys the entity, increments the generation and removes the linking and destroys the components.
      * @param[in] entity (sulphur::engine::Entity) The entity to destroy.
      */
      void Destroy(Entity entity) override;

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

    private:
      World* world_; //!< A pointer to the world that this system is a part of

      foundation::Vector<byte> generation;//!< Stores the current generation of the entity which is used in the Alive function @see sulphur::engine::EntitySystem::Alive.
      foundation::Deque<uint> free_indices;//!< Stores free entity slots for reuse.
      foundation::Vector<EntityComponentData> entity_components;//!< Stores the linking information of the components.

    };

    //-------------------------------------------------------------------------
    template<typename Component>
    inline Component Entity::Add()
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetSystem<EntitySystem>();

      Component component = (static_cast<typename Component::System&>(
        world.GetSystem(Component::System::type_id())
        )).Create(*this);

      system.Link(
        *this,
        *static_cast<ComponentHandleBase*>(&component),
        Component::System::type_id());

      return component;
    }
    template<typename Component>
    inline void Entity::Remove(Component handle)
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetSystem<EntitySystem>();

      system.UnLink(
        *this,
        *static_cast<ComponentHandleBase*>(&handle),
        Component::System::type_id());
    }

    //-------------------------------------------------------------------------
    template<typename Component>
    inline Component Entity::Get() const
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetSystem<EntitySystem>();

      ComponentHandleBase avoid_error = system.GetHandle(*this, Component::System::type_id());
      return Component(world.GetSystem<typename Component::System>(), avoid_error.Handle());
    }
    template<typename Component>
    inline bool Entity::Has() const
    {
      World& world = application_->GetService<WorldProviderSystem>().GetWorld();
      EntitySystem& system = world.GetSystem<EntitySystem>();

      ComponentHandleBase avoid_error = system.GetHandle(*this, Component::System::type_id());
      return avoid_error != Component::InvalidHandle();
    }
  }
}
