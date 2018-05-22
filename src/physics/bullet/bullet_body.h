#pragma once
#include "physics/iphysics_body.h"

class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;
class btDiscreteDynamicsWorld;

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

    protected:
      friend class PhysicsShape; //!< Declared friend class for attaching shapes to the body.
      /**
      * @see sulphur::physics::IPhysicsBody::SetShape
      */
      void SetShape(PhysicsShape* shape) override;

    private:
      btDiscreteDynamicsWorld* dynamics_world_; //!< The world that the body lives in
      btDefaultMotionState* motion_state_; //!< The default motion state
      btRigidBody* rigid_body_; //!< The Bullet rigid body
    };
  }
}
