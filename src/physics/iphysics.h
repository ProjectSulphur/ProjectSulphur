#pragma once

#include "physics/raycast.h"
#include "physics/platform_physics_body.h"
#include "physics/physics_manifold.h"
#include "physics/iphysics_constraint.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysics
    * @brief The low level physics interface that the physics SDKs should implement
    * @author Daniel Konings, Benjamin Waanders, Angelo van der Mark
    */
    class IPhysics
    {

    public:

      /**
      * @brief Initializes the low level SDK
      * @return (bool) Was the initialization a success?
      */
      virtual bool Initialize() = 0;

      /**
      * @brief Destroys an initialized low level SDK
      */
      virtual void Destroy() = 0;

      /**
      * @brief Creates a physics body and creates the underlying low level SDK reference
      * @param[in] translation (const glm::vec3&) The initial translation of the physics body
      * @param[in] rotation (const glm::quat&) The initial rotation of the physics body
      */
      virtual PhysicsBody* AddPhysicsBody(
        const glm::vec3& translation, 
        const glm::quat& rotation) = 0;

      /**
      * @brief Removes a created physics body from the list of physics bodies in the simulation
      * @param[in] body (sulphur::physics::PhysicsBody*) The physics body to remove
      */
      virtual void RemovePhysicsBody(PhysicsBody* body) = 0;

      /**
      * @brief Creates a constraint for a body of the specified type.
      * @param[in] owner (sulphur::physics::PhysicsBody*) The owner of the constraint.
      * @param[in] type (sulphur::physics::IPhysicsConstraint::ConstraintTypes) The constraint type.
      * @return (sulphur::physics::IPhysicsConstraint*) The created constraint.
      */
      virtual IPhysicsConstraint* AddConstraint(PhysicsBody* owner, IPhysicsConstraint::ConstraintTypes type) = 0;

      /**
      * @brief Removes a constraint from the simulation.
      * @param[in] constraint (sulphur::physics::IPhysicsConstraint*) The constraint to remove.
      */
      virtual void RemoveConstraint(IPhysicsConstraint* constraint) = 0;

      /**
      * @brief Sets the global gravity of the physics implementation
      * @param[in] gravity (const glm::vec3&) The new gravity to set
      */
      virtual void SetGlobalGravity(const glm::vec3& gravity) = 0;

      /**
      * @return (glm::vec3) The global gravity of the physics implementation
      */
      virtual glm::vec3 GetGlobalGravity() const = 0;

      /**
      * @brief Simulates a frame in the low level SDK
      * @param[in] fixed_time_step (float) The fixed time step in seconds
      */
      virtual void SimulateStep(float fixed_time_step) = 0;

      /**
      * @brief Does a Raycast in the physics world of the low level SDK,
      * but only returns the closest object
      * @param[in] ray (const sulphur::foundation::Ray&) The ray to cast
      * @param[out] out (sulphur::physics::RaycastHitInfo*) The information of the hit, if any
      * @param[in] max_distance (float) The maximum distance the ray is allowed to intersect at
      * @return (bool) Was there a ray intersection with a collider?
      */
      virtual bool Raycast(const foundation::Ray& ray, RaycastHitInfo* out, float max_distance) = 0;

      /**
      * @see sulphur::physics::IPhysics::Raycast
      * @remarks This raycast method returns all intersections with a ray,
      * instead of only the closest
      * @param[in] ray (const sulphur::foundation::Ray&) The ray to cast
      * @param[out] hit (bool*) Was there a ray intersection at all?
      * @param[in] max_distance (float) The maximum distance the ray is allowed to intersect at
      * @return (sulphur::physics::RaycastHits) All intersections with the ray
      */
      virtual RaycastHits RaycastAll(const foundation::Ray&, bool* hit, float max_distance) = 0;

      /**
      * @brief The default gravity that all physics implementations should use
      */
      static const glm::vec3 kDefaultGravity;

      /**
      * @brief manifold buffer limit
      */
      static const size_t kManifoldBufferLimit;

      /**
      * @brief The list of manifolds that were constructed since the last fixed update
      */
      virtual PhysicsManifold* GetManifolds() = 0;

      /**
      * @brief the count of sulphur::physics::IPhysics::GetManifolds as in the amount of manifolds
      */
      virtual size_t GetManifoldsSize() = 0;
  
      /**
      * @brief Ensures that the manifold listner looks for the physicsbody in question
      */
      virtual void SubscribeCallback(PhysicsBody* body) = 0;

      /**
      * @brief Unsubbes the body from manifold forwarding
      */
      virtual void UnSubscribeCallback(PhysicsBody* body) = 0;
    };
  }
}
