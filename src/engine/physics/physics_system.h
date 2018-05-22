#pragma once

#include "engine/systems/service_system.h"
#include "engine/core/entity_system.h"

#include <foundation/containers/map.h>
#include <foundation/memory/memory.h>
#include <physics/platform_physics.h>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>


namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::PhysicsSystem : public sulphur::engine::IServiceSystem<PhysicsSystem>
    * @brief The physics service for communicating with the physics implmentations and the engine
    * @todo Rule of 5
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class PhysicsSystem : public IServiceSystem<PhysicsSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      PhysicsSystem();

      /**
      * @see sulphur::engine::ISystemBase::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::ISystemBase::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @see sulphur::engine::ISystemBase::OnFixedUpdate
      */
      void OnFixedUpdate() override;

      /**
      * @param[in] ent (sulphur::engine::Entity) The entity requesting a physics body.
      * @return (sulphur::physics::PhysicsBody*) The body that's linked to this Entity.
      * @brief Creates a physics body to be used by the provided Entity.
      */
      physics::PhysicsBody* CreatePhysicsBody(Entity ent);

      /**
      * @param[in] ent (sulphur::engine::Entity ent)
      * @brief Destroys the physicsbody on the entity
      */
      void DestroyPhysicsBody(Entity ent);

      /**
      * @param[in] delta_time (float)
      * @brief Simulates one step in the implemented physics library
      */
      void SimulateStep(float delta_time);

      /**
      * @param[in] gravity (const glm::vec3&)
      * @brief sets the global gravity for the world
      */
      void SetGlobalGravity(const glm::vec3& gravity);

      /**
      * @brief Returns the global gravity for the world.
      * @return (glm::vec3) The gravity vector.
      */
      glm::vec3 GetGlobalGravity() const;

      /**
      * @param[in] ent (sulphur::engine::Entity ent)
      * @return (sulphur::physics::PhysicsBody*) The body that's linked to this Entity.
      * @brief Returns the encoupled physicsbody of the entity or nullptr if none
      */
      physics::PhysicsBody* GetPhysicsBody(Entity ent);

    private:
      /**
      * @brief Internal function used to synchronise states between physics and the engine.
      */
      void SyncPhysicsToEngine();

      using EntityBodyMap = foundation::Map<Entity, physics::PhysicsBody*>;
      using BodyIterator = EntityBodyMap::iterator; //!< The iterator for the map of physics bodies
      EntityBodyMap bodies_; //!< The list of Bullet rigid bodies, with a mapping to the platform physics body

      physics::PlatformPhysics* physics_; //!< A pointer to the platform-specific physics wrapper

    };
  }
}
