#pragma once

#include "engine/systems/service_system.h"
#include "engine/core/entity_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/scriptable_object.h"
#include "engine/scripting/script_system.h"
#include "engine/assets/mesh.h"

#include <foundation/containers/map.h>
#include <foundation/containers/set.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

#include <physics/platform_physics.h>
#include <physics/platform_physics_constraint.h>
#include <physics/iphysics_shape.h>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>


namespace sulphur
{
  namespace engine
  {
    class ScriptSystem;

    /**
    * @class sulphur::engine::PhysicsSystem : public sulphur::engine::IServiceSystem<PhysicsSystem>
    * @brief The physics service for communicating with the physics implmentations and the engine
    * @todo Rule of 5
    * @author Benjamin Waanders, Angelo van der Mark
    */
    SCRIPT_CLASS() class PhysicsSystem :
      public IServiceSystem<PhysicsSystem>,
      public ScriptableObject
    {
    public:
      SCRIPT_NAME(Physics);
      /**
      * @brief Default constructor
      */
      PhysicsSystem();

      /**
      * @brief Default constructor
      */
      ~PhysicsSystem();

      /**
      * @see sulphur::engine::ISystemBase::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::ISystemBase::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @param[in] entity (sulphur::engine::Entity) The entity requesting a physics body.
      * @return (sulphur::physics::PhysicsBody*) The body that's linked to this Entity.
      * @brief Creates a physics body to be used by the provided Entity.
      */
      physics::PhysicsBody* CreatePhysicsBody(Entity entity);

      /**
      * @param[in] entity (sulphur::engine::Entity)
      * @brief Destroys the physicsbody on the entity
      */
      void DestroyPhysicsBody(Entity entity);

      /**
      * @brief Creates a constraint for an entity.
      * @param[in] owner (sulphur::engine::Entity) The owning entity.
      * @param[in] type (sulphur::physics::IPhysicsConstraint::ConstraintTypes) The constraint type to create.
      */
      physics::IPhysicsConstraint* CreateConstraint(Entity owner,
                                   physics::IPhysicsConstraint::ConstraintTypes type);

      /**
      * @brief Destroys the constraint.
      * @param[in] constraint (sulphur::physics::IPhysicsConstraint*) The constraint to destroy.
      */
      void DestroyConstraint(physics::IPhysicsConstraint* constraint);

      /**
      * @brief Creates a collider instance based on a primitive collision shape.
      * @param[in] entity (sulphur::engine::Entity) The entity requesting a collider.
      * @param[in] type (sulphur::physics::IPhysicsShape::ShapeTypes) The type of primitive collider.
      * @return (sulphur::physics::PhysicsCollider*) The created collider instance.
      */
      physics::PhysicsCollider* CreatePrimitiveCollider(Entity entity,
                                physics::IPhysicsShape::ShapeTypes type);

      /**
      * @brief Creates a collider instance based on a mesh resource.
      * @remarks If a collision shape of this mesh already exists, it will be re-used.
      * @remarks Invalid mesh handles will use a unit cube as fallback collision shape.
      * @remarks Mesh collision shapes do NOT get updated when the corresponding mesh changes!
      * @param[in] entity (sulphur::engine::Entity) The entity requesting a collider.
      * @param[in] mesh (sulphur::engine::MeshHandle) Handle to the relevant mesh resource.
      * @param[in] convex (bool) Whether the mesh should be turned into a convex collider. Enabled by default.
      * @todo Concave collision shapes are currently not yet supported.
      * @return (sulphur::physics::PhysicsCollider*) The created collider instance.
      */
      physics::PhysicsCollider* CreateMeshCollider(
        Entity entity, MeshHandle mesh, bool convex = true);
      
      /**
      * @brief Creates a collider instance based on an existing collision shape.
      * @remarks Any changes made to the given collision shape will affect all colliders using them.
      * @param[in] entity (sulphur::engine::Entity) The entity requesting a collider.
      * @param[in] shape (sulphur::physics::IPhysicsShape*) An existing collision shape.
      * @return (sulphur::physics::PhysicsCollider*) The created collider instance.
      */
      physics::PhysicsCollider* CreateCollider(Entity entity, physics::IPhysicsShape* shape);

      /**
      * @brief Destroys the collider instance.
      * @param[in] collider (sulphur::physics::PhysicsCollider*) The collider to destroy.
      */
      void DestroyCollider(physics::PhysicsCollider* collider);

      /**
      * @param[in] delta_time (float)
      * @brief Simulates one step in the implemented physics library
      */
      void SimulateStep(float delta_time);

      /**
      * @brief Sets the global gravity for the world.
      * @param[in] gravity (const glm::vec3&) The gravity to use.
      */
      SCRIPT_FUNC() void SetGlobalGravity(const glm::vec3& gravity);

      /**
      * @brief Returns the global gravity for the world.
      * @return (glm::vec3) The gravity vector.
      */
      glm::vec3 GetGlobalGravity() const;

      /**
      * @param[in] entity (sulphur::engine::Entity)
      * @return (sulphur::physics::PhysicsBody*) The body that's linked to this Entity.
      * @brief Returns the encoupled physicsbody of the entity or nullptr if none
      */
      physics::PhysicsBody* GetPhysicsBody(Entity entity);

      /**
      * @brief Called every update and gathers changes in the transforms
      */
      void GatherChangedTransforms();

      /*
      * @brief Called to add a collision enter callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddCollisionEnterCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to add a collision stay callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddCollisionStayCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to add a collision leave callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddCollisionLeaveCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to add an overlap enter callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddOverlapEnterCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to add an overlap stay callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddOverlapStayCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to add an overlap leave callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void AddOverlapLeaveCallback(Entity entity, ScriptableCallback callback);

      /*
      * @brief Called to remove a collision enter callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveCollisionEnterCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to remove a collision stay callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveCollisionStayCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to remove a collision leave callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveCollisionLeaveCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to remove an overlap enter callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveOverlapEnterCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to remove an overlap enter callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveOverlapStayCallback(Entity entity, ScriptableCallback callback);
      /*
      * @brief Called to remove an overlap leave callback
      * @param[in] entity (sulphur::engine::Entity) The entity to call the callback upon
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback function to call
      */
      void RemoveOverlapLeaveCallback(Entity entity, ScriptableCallback callback);

      /*
      * @brief Scripting raycast funtion
      * @param[in] ray (const foundation::Ray&) The ray to cast
      * @param[in] distance (float) The max distance for the raycast
      * @return (ScriptHandle) the raycast return for scripting
      */
      SCRIPT_FUNC(static) ScriptHandle RayCast(const foundation::Ray& ray, float distance);

      /*
      * @brief Scripting raycast all funtion
      * @param[in] ray (const foundation::Ray&) The ray to cast
      * @param[in] distance (float) The max distance for the raycast
      * @return (ScriptHandle) the raycast return for scripting
      */
      SCRIPT_FUNC(static) ScriptHandle RayCastAll(const foundation::Ray& ray, float distance);

    private:
      /**
      * @brief Internal function used to synchronise states between physics and the engine.
      */
      void SyncPhysicsToEngine();

      static PhysicsSystem* self_; //!< Instance to current active physiscs system for use in scripting

      using EntityBodyMap = foundation::Map<Entity, physics::PhysicsBody*>; //!< Map to keep track of the relation between entities and low level PhysicsBodies
      using EntityBodyMapIt = EntityBodyMap::iterator; //!< The iterator for the map of physics bodies
      EntityBodyMap bodies_; //!< The list of Bullet rigid bodies, with a mapping to the platform physics body

      using BodyEntityMap = foundation::Map<physics::PhysicsBody*, Entity>; //!< Map to keep track of the relation between low level PhysicsBodies and entities
      using BodyEntityMapIt = BodyEntityMap::iterator; //!< The iterator for the map of physics bodies
      BodyEntityMap entities_; //!< The list of Bullet rigid bodies, with a mapping to the platform physics body
      foundation::Vector<EntityBodyMapIt> changes_; //!< Set of components that have changed transform since last frame

      foundation::Vector<physics::IPhysicsShape*> primitive_shapes_; //!< List of primitive shapes that are currently in use.
      foundation::Map<MeshHandle, physics::IPhysicsShape*> convex_shapes_; //!< List of convex mesh shapes that are currently in use.
      foundation::Map<MeshHandle, physics::IPhysicsShape*> concave_shapes_; //!< List of concave mesh shapes that are currently in use.
      MeshHandle fallback_mesh_; //!< Fallback mesh instance for shapes.

      physics::PlatformPhysics* physics_; //!< A pointer to the platform-specific physics wrapper
      ScriptSystem* script_system_; //!< Cached local scripting system;

      /**
      * @enum For internal use of callbacks administration
      */
      enum struct CallbackType : int
      {
        kCollisionEnter = 0,
        kCollisionStay = 1,
        kCollisionLeave = 2,
        kOverlapEnter = 3,
        kOverlapStay = 4,
        kOverlapLeave = 5,
        kSize = 6
      };

      /**
      * @struct sulphur::engine::PhysicsScriptingCallbacks
      * @brief a collection that keeps track of callbacks and their respective bodies it SoA so every index is aligned
      */
      struct PhysicsScriptingCallbacks
      {
        physics::PhysicsBody** bodies_; //!< The related physics body for the callback, stored so no real time fetching has to be done based on entity
        CallbackType* types_; //!< The callback type
        ScriptableCallback* callbacks_; //!< The actual callback object for scripting to call
        size_t size_; //!< The size of this SoA
      };

      physics::PhysicsManifold* contact_history_; //!< Collection that keeps track of past collision events for the callbacks
      size_t contact_history_size_; //!< Size of the contact_history_;
      PhysicsScriptingCallbacks callbacks_; //!< SoA scripting callbacks

      /**
      * @brieft internal add callback function
      * @param[in] entity (sulphur::engine::Entity) The entity related to the callback
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback
      * @param[in] type (sulpur::engine::PhysicsSystem::CallbackType) the type of callback
      */
      void AddCallback(Entity entity, ScriptableCallback callback, CallbackType type);

      /**
      * @brieft internal remove callback function
      * @param[in] entity (sulphur::engine::Entity) The entity related to the callback
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback
      * @param[in] type (sulpur::engine::PhysicsSystem::CallbackType) the type of callback
      */
      void RemoveCallback(Entity entity, ScriptableCallback callback, CallbackType type);

      /**
      * @brief Internal function to call a callback
      * @param[in] callback (int) the callback index in sulphur::engine::PhysicsSystem::callbacks_
      * @param[in] manifold (int) the manifold index in sulphur::physics::PlatformPhysics::GetManifolds()
      * @param[in] swap_manifold_order (bool) if the order of the manifold should be swapped (body_a = body_b and body_b = body_a)
      */
      void inline CallCallback(int callback, int manifold, bool swap_manifold_order = false);
    };

    /**
    * @struct sulphur::engine::Finder
    */
    struct Finder
    {
      /**
      * @brief compare method for creation of a unique sulphur::engine::PhysicsSystem::changes_ collection
      */
      inline bool operator()(
        const foundation::Map<Entity, physics::PhysicsBody*>::iterator& lhs,
        const TransformComponent& rhs)
      {
        return lhs->first.GetIndex() == rhs.GetIndex();
      }
    };
  }
}
