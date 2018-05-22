#pragma once

#include "physics/raycast.h"
#include "foundation/memory/memory.h"
#include "physics/platform_physics_body.h"

#include <foundation/containers/vector.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysics
    * @brief The low level physics interface that the physics SDKs should implement
    * @author Daniel Konings, Benjamin Waanders
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
    };
  }
}
