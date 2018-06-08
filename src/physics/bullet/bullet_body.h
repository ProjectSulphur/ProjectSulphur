#pragma once

#include "physics/iphysics_body.h"
#include "physics/physics_collider.h"

#include <foundation/containers/vector.h>

class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;
class btDiscreteDynamicsWorld;
class btCompoundShape;
class btManifoldPoint;
struct btCollisionObjectWrapper;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletBody
    * @brief Used to communicate between the engine its components and Bullet's physicsbodies
    * @author Daniel Konings, Benjamin Waanders, Angelo van der Mark
    */
    class BulletBody : IPhysicsBody
    {
    public:
      /**
      * @brief Creates a physics body with an initial translation and rotation
      * @remarks Sets velocity vectors to a zero vector
      * @param[in] translation (const glm::vec3&) The initial translation
      * @param[in] rotation (const glm::quat&) The initial rotation
      */
      BulletBody(btDiscreteDynamicsWorld* dynamics_world,
        const glm::vec3& translation,
        const glm::quat& rotation);

      /**
      * @brief The deconstructor. It cleans up the constract with Bullet
      */
      ~BulletBody() override;
 
      /**
      * @see sulphur::physics::IPhysicsBody::SetTranslation
      */
      void SetTranslation(const glm::vec3& translation) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetTranslation
      */
      glm::vec3 GetTranslation() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetRotation
      */
      void SetRotation(const glm::quat& rotation) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetRotation
      */
      glm::quat GetRotation() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetMass
      */
      void SetMass(float mass) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetMass
      */
      float GetMass() const override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetInverseMass
      */
      float GetInverseMass() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetInertiaTensor
      */
      void SetInertiaTensor(const glm::vec3& inertia) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetInertiaTensor
      */
      glm::vec3 GetInertiaTensor() const override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetInverseInertiaTensor
      */
      glm::vec3 GetInverseInertiaTensor() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetLinearVelocity
      */
      void SetLinearVelocity(const glm::vec3& velocity) override;
      /**
      * @see sulphur::physics::IPhysicsBody::AddLinearVelocity
      */
      void AddLinearVelocity(const glm::vec3& velocity) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetLinearVelocity
      */
      glm::vec3 GetLinearVelocity() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetAngularVelocity
      */
      void SetAngularVelocity(const glm::vec3& velocity) override;
      /**
      * @see sulphur::physics::IPhysicsBody::AddAngularVelocity
      */
      void AddAngularVelocity(const glm::vec3& velocity) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetAngularVelocity
      */
      glm::vec3 GetAngularVelocity() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetLinearDamping
      */
      void SetLinearDamping(float damping) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetLinearDamping
      */
      float GetLinearDamping() const override;
      /**
      * @see sulphur::physics::IPhysicsBody::SetAngularDamping
      */
      void SetAngularDamping(float damping) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetAngularDamping
      */
      float GetAngularDamping() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::ApplyForce
      */
      void ApplyForce(const glm::vec3& force) override;
      /**
      * @see sulphur::physics::IPhysicsBody::ApplyTorque
      */
      void ApplyTorque(const glm::vec3& torque) override;
      /**
      * @see sulphur::physics::IPhysicsBody::ApplyForceAtPosition
      */
      void ApplyForceAtPosition(const glm::vec3& force, const glm::vec3& position) override;

      /**
      * @see sulphur::physics::IPhysicsBody::ApplyImpulse
      */
      void ApplyImpulse(const glm::vec3& impulse) override;
      /**
      * @see sulphur::physics::IPhysicsBody::ApplyAngularImpulse
      */
      void ApplyAngularImpulse(const glm::vec3& impulse) override;
      /**
      * @see sulphur::physics::IPhysicsBody::ApplyImpulseAtPosition
      */
      void ApplyImpulseAtPosition(const glm::vec3& impulse, const glm::vec3& position) override;

      /**
      * @see sulphur::physics::IPhysicsBody::IsSleeping
      */
      bool IsSleeping() const override;
      /**
      * @see sulphur::physics::IPhysicsBody::ForceWake
      */
      void ForceWake() override;
      /**
      * @see sulphur::physics::IPhysicsBody::ForceSleep
      */
      void ForceSleep() override;

      /**
      * @see sulphur::physics::IPhysicsBody::LockTranslation
      */
      void LockTranslation(const glm::bvec3& lock_axes) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetTranslationLock
      */
      glm::bvec3 GetTranslationLock() const override;
      /**
      * @see sulphur::physics::IPhysicsBody::LockRotation
      */
      void LockRotation(const glm::bvec3& lock_axes) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetRotationLock
      */
      glm::bvec3 GetRotationLock() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::SetPhysicsBodyType
      */
      void SetPhysicsBodyType(PhysicsBodyType type) override;
      /**
      * @see sulphur::physics::IPhysicsBody::GetPhysicsBodyType
      */
      PhysicsBodyType GetPhysicsBodyType() const override;

      /**
      * @see sulphur::physics::IPhysicsBody::AddShape
      * @remarks Modifying the body's shape will recalculate the inertia tensor automatically.
      */
      PhysicsCollider* AddShape(IPhysicsShape* shape) override;

      /**
      * @see sulphur::physics::IPhysicsBody::RemoveShape
      * @remarks Modifying the body's shape will recalculate the inertia tensor automatically.
      */
      void RemoveShape(PhysicsCollider* collider) override;

      /**
      * @brief Updates the collider BulletBody-side.
      * @param[in] collider (sulphur::physics::PhysicsCollider*) The collider to update.
      * @remarks Modifying the body's shape will recalculate the inertia tensor automatically.
      */
      void UpdateCollider(PhysicsCollider* collider) override;

      /**
      * @brief Set the friction combine mode.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode to set it to.
      * @internal
      */
      void set_friction_combine_mode(PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Returns the friction combine mode.
      * @return (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode currently in use.
      * @internal
      */
      PhysicsCollider::MaterialCombineMode friction_combine_mode() const;

      /**
      * @brief Set the restitution combine mode.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode to set it to.
      * @internal
      */
      void set_restitution_combine_mode(PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Returns the restitution combine mode.
      * @return (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode currently in use.
      * @internal
      */
      PhysicsCollider::MaterialCombineMode restitution_combine_mode() const;

      /**
      * @brief Set the static friction coefficient.
      * @param[in] friction (float) The static friction coefficient to use.
      * @internal
      */
      void set_static_friction(float friction);

      /**
      * @brief Returns the static friction coefficient.
      * @return (float) The static friction coefficient currently in use.
      * @internal
      */
      float static_friction() const;

      /**
      * @brief Returns the Bullet rigid body.
      * @returns (btRigidBody*) The Bullet rigid body.
      * @internal
      */
      btRigidBody* rigid_body() const;

    private:

      /**
      * @brief Returns the index of the given collider on this body.
      * @param[in] collider (sulphur::physics::PhysicsCollider*) The collider on this body.
      * @return (int) The index of the collider. Returns -1 if the collider is not attached.
      * @internal
      */
      int GetColliderIndex(PhysicsCollider* collider) const;

      btDiscreteDynamicsWorld* dynamics_world_; //!< The world that the body lives in
      btDefaultMotionState* motion_state_; //!< The default motion state
      btRigidBody* rigid_body_; //!< The Bullet rigid body

      btCompoundShape* shape_; //!< The shape container, can manage multiple shapes.
      foundation::Vector<PhysicsCollider*> colliders_; //!< List of colliders currently attached to this body.

      float kinematic_mass_; //!< The body's mass, stored in case the body is set to kinematic.

      // Because Bullet handles materials per body, additional material data and functions will be handled by this class as well.

      /**
      * @brief Custom material callback. Function signature is defined by Bullet. Internal use only.
      * @param[in] contact_point (btManifoldPoint&) The contact point.
      * @param[in] col_obj0_wrap (btCollisionObjectWrapper*) Collision object wrapper.
      * @param[in] part_id0 (int) Collision data.
      * @param[in] index0 (int) Collision data.
      * @param[in] col_obj1_wrap (btCollisionObjectWrapper*) Collision object wrapper.
      * @param[in] part_id1 (int) Collision data.
      * @param[in] index1 (int) Collision data.
      * @returns (bool) Unused return value.
      */
      static bool CustomMaterialCallback(btManifoldPoint& contact_point,
        const btCollisionObjectWrapper* col_obj0_wrap, int part_id0, int index0,
        const btCollisionObjectWrapper* col_obj1_wrap, int part_id1, int index1);

      PhysicsCollider::MaterialCombineMode friction_combine_mode_; //!< Combine mode used for calculating friction.
      PhysicsCollider::MaterialCombineMode restitution_combine_mode_; //!< Combine mode used for calculating restitution.
      float static_friction_; //!< Static friction coefficient, which doesn't exist in Bullet.
    };
  }
}
