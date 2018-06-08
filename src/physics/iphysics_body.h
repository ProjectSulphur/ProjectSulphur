#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sulphur
{
  namespace physics
  {
    class PhysicsCollider;
    class IPhysicsShape;

    /**
    * @brief Indicates the type of physics body, which in turn influences its behaviour in the simulation.
    */
    enum struct PhysicsBodyType
    {
      kStatic, //!< Static body type
      kDynamic, //!< Dynamic body type
      kKinematic //!< Kinematic body type
    };

    /**
    * @class sulphur::physics::IPhysicsBody
    * @brief Interface used to transfer entity transformation data to the low level SDK.
    * @author Daniel Konings, Benjamin Waanders, Angelo van der Mark
    */
    class IPhysicsBody
    {
    public:

      /**
      * @brief Virtual destructor.
      */
      virtual ~IPhysicsBody() = default;

      /**
      * @brief Sets the translation of this physics body.
      * @param[in] translation (const glm::vec3&) The new translation to set.
      */
      virtual void SetTranslation(const glm::vec3& translation) = 0;

      /**
      * @brief Returns the translation of this physics body.
      * @return (glm::vec3) The translation of this physics body.
      */
      virtual glm::vec3 GetTranslation() const = 0;

      /**
      * @brief Sets the rotation of this physics body.
      * @param[in] rotation (const glm::quat&) The new rotation to set.
      */
      virtual void SetRotation(const glm::quat& rotation) = 0;
      
      /**
      * @brief Return the rotation of this physics body.
      * @return (glm::quat) The rotation of this physics body.
      */
      virtual glm::quat GetRotation() const = 0;

      /**
      * @brief Sets the mass of this physics body.
      * @param[in] mass (float) Mass in Kg.
      */
      virtual void SetMass(float mass) = 0;

      /**
      * @brief Returns the mass of this physics body.
      * @return (float) Mass in Kg.
      */
      virtual float GetMass() const = 0;

      /**
      * @brief Returns the inverse mass of this physics body.
      * @return (float) Inverse mass in Kg.
      */
      virtual float GetInverseMass() const = 0;
      
      /**
      * @brief Sets the inertia tensor of this physics body.
      * @remarks The inertia tensor will be recalculated when the collision shape or mass changes.
      * @param[in] inertia (const glm::vec3&) The inertia tensor's diagonal.
      */
      virtual void SetInertiaTensor(const glm::vec3& inertia) = 0;

      /**
      * @brief Returns the inertia tensor of this physics body.
      * @return (glm::mat3x3) The inertia tensor.
      */
      virtual glm::vec3 GetInertiaTensor() const = 0;

      /**
      * @brief Returns the inverse inertia tensor of this physics body.
      * @return (glm::mat3x3) The inverse inertia tensor.
      */
      virtual glm::vec3 GetInverseInertiaTensor() const = 0;

      /**
      * @brief Sets the linear velocity of this physics body.
      * @param[in] velocity (const glm::vec3&) Velocity to set it to.
      */
      virtual void SetLinearVelocity(const glm::vec3& velocity) = 0;

      /**
      * @brief Adds a linear velocity to this physics body.
      * @param[in] velocity (const glm::vec3&) Velocity to add.
      */
      virtual void AddLinearVelocity(const glm::vec3& velocity) = 0;

      /**
      * @brief Returns the linear velocity of this physics body.
      * @return (glm::vec3) The current linear velocity.
      */
      virtual glm::vec3 GetLinearVelocity() const = 0;

      /**
      * @brief Set the angular velocity of this physics body.
      * @param[in] velocity (const glm::vec3&) Angular velocity to set it to.
      */
      virtual void SetAngularVelocity(const glm::vec3& velocity) = 0;

      /**
      * @brief Adds an angular velocity to this physics body.
      * @param[in] velocity (const glm::vec3&) Angular velocity to add.
      */
      virtual void AddAngularVelocity(const glm::vec3& velocity) = 0;

      /**
      * @brief Returns the angular velocity of this physics body.
      * @return (glm::vec3) The current angular velocity.
      */
      virtual glm::vec3 GetAngularVelocity() const = 0;

      /**
      * @brief Sets the linear damping coefficient for this physics body.
      * @param[in] damping (float) The damping coefficient.
      */
      virtual void SetLinearDamping(float damping) = 0;

      /**
      * @brief Returns the linear damping coefficient of this physics body.
      * @return (float) The damping coefficient.
      */
      virtual float GetLinearDamping() const = 0;

      /**
      * @brief Sets the angular damping coefficient for this physics body.
      * @param[in] damping (float) The damping coefficient.
      */
      virtual void SetAngularDamping(float damping) = 0;

      /**
      * @brief Returns the linear damping coefficient of this physics body.
      * @return (float) The damping coefficient.
      */
      virtual float GetAngularDamping() const = 0;

      /**
      * @brief Applies a linear force on the center of mass.
      * @param[in] force (const glm::vec3&) The force to apply.
      */
      virtual void ApplyForce(const glm::vec3& force) = 0;

      /**
      * @brief Applies a torque to the physics body. This will not apply any linear force.
      * @param[in] torque (const glm::vec3&) The torque to apply.
      */
      virtual void ApplyTorque(const glm::vec3& torque) = 0;

      /**
      * @brief Applies a force on the rigid body from a position in world space.
      * @param[in] force (const glm::vec3&) The force to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the force is applied.
      */
      virtual void ApplyForceAtPosition(const glm::vec3& force, const glm::vec3& position) = 0;

      /**
      * @brief Applies a linear impulse on the center of mass.
      * @param[in] impulse (const glm::vec3&) The linear impulse to apply.
      */
      virtual void ApplyImpulse(const glm::vec3& impulse) = 0;

      /**
      * @brief Applies an angular impulse on the rigid body. This will not apply any linear impulse.
      * @param[in] impulse (const glm::vec3&) The angular impulse to apply.
      */
      virtual void ApplyAngularImpulse(const glm::vec3& impulse) = 0;

      /**
      * @brief Applies an impulse on the rigid body from a position in world space.
      * @param[in] impulse (const glm::vec3&) Impulse to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the impulse is applied.
      */
      virtual void ApplyImpulseAtPosition(const glm::vec3& impulse, const glm::vec3& position) = 0;

      /**
      * @brief Returns whether the physics body is active in the simulation.
      * @return (bool) True is the physics body is sleeping or static.
      */
      virtual bool IsSleeping() const = 0;

      /**
      * @brief Wakes the physics body if it is sleeping.
      */
      virtual void ForceWake() = 0;

      /**
      * @brief Makes the physics body sleep if it is active.
      */
      virtual void ForceSleep() = 0;

      /**
      * @brief Allows a total restriction of movement on the given axes. 
      * @note This only applies to physics-based movement. Directly moving the body is unaffected.
      * @param[in] lock_axes (const glm::bvec3&) The axes to lock. True means the axis is locked.
      */
      virtual void LockTranslation(const glm::bvec3& lock_axes) = 0;

      /**
      * @brief Returns what axes this body is locked to, restricting movement.
      * @return (glm::bvec3) Three booleans corresponding to each axis. True means translation is locked.
      */
      virtual glm::bvec3 GetTranslationLock() const = 0;

      /**
      * @brief Allows a total restriction of rotation on the given axes.
      * @note This only applies to physics-based movement. Directly rotating the body is unaffected.
      * @param[in] lock_axes (const glm::bvec3&) The axes to lock. True means the axis is locked.
      */
      virtual void LockRotation(const glm::bvec3& lock_axes) = 0;

      /**
      * @brief Returns what axes this body is locked to, restricting rotation.
      * @return (glm::bvec3) Three booleans corresponding to each axis. True means rotation is locked.
      */
      virtual glm::bvec3 GetRotationLock() const = 0;

      

      /**
      * @brief Changes this physics body state to static, dynamic, or kinematic.
      * @param[in] type (sulphur::physics::PhysicsBodyType) The new body type.
      * @note The mass of non-static bodies are set to 1 by default.
      * @todo Implement kinematic bodies.
      */
      virtual void SetPhysicsBodyType(PhysicsBodyType type) = 0;

      /**
      * @brief Returns what type this body has in the simulation.
      * @note Only if an entity has more than zero collider components and no rigid body components will the 'body' be static.
      * @return (sulphur::physics::PhysicsBodyType) The body type.
      */
      virtual PhysicsBodyType GetPhysicsBodyType() const = 0;

      /**
      * @brief Adds a physics shape to the body.
      * @param[in] shape (sulphur::physics::IPhysicsShape*) The shape to add to the body.
      * @return (sulphur::physics:PhysicsCollider*) The constructed collider linked to this body.
      */
      virtual PhysicsCollider* AddShape(IPhysicsShape* shape) = 0;

      /**
      * @brief Removes a collider from the body and destroys the collider instance.
      * @param[in] collider (sulphur::physics::PhysicsCollider*) The collider to remove from the body.
      */
      virtual void RemoveShape(PhysicsCollider* collider) = 0;

      /**
      * @brief Called when the collider may need to be updated PhysicsBody-side.
      * @param[in] collider (sulphur::physics::PhysicsCollider*) The collider to update.
      */
      virtual void UpdateCollider(PhysicsCollider* collider) = 0;
    };
  }
}
