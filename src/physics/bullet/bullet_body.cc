#include "physics/bullet/bullet_body.h"

#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include "physics/physics_shape.h"

#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletBody::BulletBody(
      btDiscreteDynamicsWorld* dynamics_world, 
      const glm::vec3& translation,
      const glm::quat& rotation) 
      : dynamics_world_(dynamics_world)
    {
      btTransform bt_transform;
      bt_transform.setIdentity();
      bt_transform.setOrigin(BulletConversions::ToBt(translation));
      bt_transform.setRotation(BulletConversions::ToBt(rotation));
      motion_state_ = foundation::Memory::Construct<btDefaultMotionState>(bt_transform);

      btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion_state_, nullptr);

      rigid_body_ = foundation::Memory::Construct<btRigidBody>(info);
      dynamics_world_->addRigidBody(rigid_body_);
    }

    //-------------------------------------------------------------------------
    BulletBody::~BulletBody()
    {
      dynamics_world_->removeRigidBody(rigid_body_);

      foundation::Memory::Destruct<btDefaultMotionState>(motion_state_);
      motion_state_ = nullptr;

      foundation::Memory::Destruct<btRigidBody>(rigid_body_);
      rigid_body_ = nullptr;
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetTranslation(const glm::vec3& translation)
    {
      btTransform& transform = rigid_body_->getWorldTransform();
      transform.setOrigin(BulletConversions::ToBt(translation));
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetTranslation() const
    {
      btTransform& transform = rigid_body_->getWorldTransform();
      return BulletConversions::ToGlm(transform.getOrigin());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetRotation(const glm::quat& rotation)
    {
      btTransform& transform = rigid_body_->getWorldTransform();
      transform.setRotation(BulletConversions::ToBt(rotation));
    }

    //-------------------------------------------------------------------------
    glm::quat BulletBody::GetRotation() const
    {
      btTransform& transform = rigid_body_->getWorldTransform();
      return BulletConversions::ToGlm(transform.getRotation());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetMass(float mass)
    {
      btVector3 local_inertia(0,0,0);
      if (rigid_body_->getCollisionShape() != nullptr)
      {
        rigid_body_->getCollisionShape()->calculateLocalInertia(mass, local_inertia);
      }
      rigid_body_->setMassProps(mass, local_inertia);
      rigid_body_->updateInertiaTensor();
    }

    //-------------------------------------------------------------------------
    float BulletBody::GetMass() const
    {
      float mass = rigid_body_->getInvMass();
      return mass == 0.0f ? 0.0f : 1.0f / mass;
    }

    //-------------------------------------------------------------------------
    float BulletBody::GetInverseMass() const
    {
      return rigid_body_->getInvMass();
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetInertiaTensor(const glm::vec3& inertia)
    {
      rigid_body_->setMassProps(GetMass(), BulletConversions::ToBt(inertia));
      rigid_body_->updateInertiaTensor();
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetInertiaTensor() const
    {
      return BulletConversions::ToGlm(rigid_body_->getLocalInertia());
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetInverseInertiaTensor() const
    {
      return BulletConversions::ToGlm(rigid_body_->getInvInertiaDiagLocal());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetLinearVelocity(const glm::vec3& velocity)
    {
      rigid_body_->setLinearVelocity(BulletConversions::ToBt(velocity));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::AddLinearVelocity(const glm::vec3& velocity)
    {
      rigid_body_->setLinearVelocity(
        rigid_body_->getLinearVelocity() + BulletConversions::ToBt(velocity));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetLinearVelocity() const
    {
      return BulletConversions::ToGlm(rigid_body_->getLinearVelocity());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetAngularVelocity(const glm::vec3& velocity)
    {
      rigid_body_->setAngularVelocity(BulletConversions::ToBt(velocity));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::AddAngularVelocity(const glm::vec3& velocity)
    {
      rigid_body_->setAngularVelocity(
        rigid_body_->getLinearVelocity() + BulletConversions::ToBt(velocity));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetAngularVelocity() const
    {
      return BulletConversions::ToGlm(rigid_body_->getAngularVelocity());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetLinearDamping(float damping)
    {
      rigid_body_->setDamping(damping, GetAngularDamping());
    }

    //-------------------------------------------------------------------------
    float BulletBody::GetLinearDamping() const
    {
      return rigid_body_->getLinearDamping();
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetAngularDamping(float damping)
    {
      rigid_body_->setDamping(GetLinearDamping(), damping);
    }

    //-------------------------------------------------------------------------
    float BulletBody::GetAngularDamping() const
    {
      return rigid_body_->getAngularDamping();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyForce(const glm::vec3& force)
    {
      rigid_body_->applyCentralForce(BulletConversions::ToBt(force));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyTorque(const glm::vec3& torque)
    {
      rigid_body_->applyTorque(BulletConversions::ToBt(torque));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyForceAtPosition(const glm::vec3& force, const glm::vec3& position)
    {
      rigid_body_->applyForce(BulletConversions::ToBt(force), BulletConversions::ToBt(position));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyImpulse(const glm::vec3& impulse)
    {
      rigid_body_->applyCentralImpulse(BulletConversions::ToBt(impulse));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyAngularImpulse(const glm::vec3& impulse)
    {
      rigid_body_->applyTorqueImpulse(BulletConversions::ToBt(impulse));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    void BulletBody::ApplyImpulseAtPosition(const glm::vec3& impulse,
      const glm::vec3& position)
    {
      rigid_body_->applyImpulse(
        BulletConversions::ToBt(impulse),
        BulletConversions::ToBt(position));
      ForceWake();
    }

    //-------------------------------------------------------------------------
    bool BulletBody::IsSleeping() const
    {
      return rigid_body_->isActive() == false;
    }

    //-------------------------------------------------------------------------
    void BulletBody::ForceWake()
    {
      rigid_body_->activate(true);
    }

    //-------------------------------------------------------------------------
    void BulletBody::ForceSleep()
    {
      rigid_body_->forceActivationState(ISLAND_SLEEPING);
    }

    //-------------------------------------------------------------------------
    void BulletBody::LockTranslation(const glm::bvec3& lock_axes)
    {
      rigid_body_->setLinearFactor(btVector3(lock_axes.x == false,
                                             lock_axes.y == false,
                                             lock_axes.z == false));
    }

    //-------------------------------------------------------------------------
    glm::bvec3 BulletBody::GetTranslationLock() const
    {
      btVector3 lock = rigid_body_->getLinearFactor();
      return glm::bvec3(lock.x() == 0.f, lock.y() == 0.f, lock.z() == 0.f);
    }

    //-------------------------------------------------------------------------
    void BulletBody::LockRotation(const glm::bvec3& lock_axes)
    {
      rigid_body_->setAngularFactor(btVector3(lock_axes.x == false,
                                              lock_axes.y == false,
                                              lock_axes.z == false));
    }

    //-------------------------------------------------------------------------
    glm::bvec3 BulletBody::GetRotationLock() const
    {
      btVector3 lock = rigid_body_->getAngularFactor();
      return glm::bvec3(lock.x() == 0.f, lock.y() == 0.f, lock.z() == 0.f);
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetShape(PhysicsShape* shape)
    {
      // Similar to switching the body type between static and dynamic, 
      // it seems mass/inertia/collisions don't get updated properly if the body 
      // exists in the world, so it's briefly removed here.
      dynamics_world_->removeRigidBody(rigid_body_);

      btCollisionShape* bt_shape = nullptr;
      if (shape != nullptr)
      {
        bt_shape = reinterpret_cast<btCollisionShape*>(shape->GetInternalShape());
      }
      rigid_body_->setCollisionShape(bt_shape);

      // Update inertia tensor. This will override a manually set inertia tensor
      SetMass(GetMass());

      dynamics_world_->addRigidBody(rigid_body_);
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetPhysicsBodyType(PhysicsBodyType type)
    {
      if (GetPhysicsBodyType() == type)
      {
        return;
      }

      // Switching from static to dynamic requires the body to be
      // temporarily removed from the simulation. Couldn't find it documented 
      // right away, but it proved to be the solution for a number of people.
      // TODO: Further testing when kinematic state is properly implemented.
      dynamics_world_->removeRigidBody(rigid_body_);
      switch (type)
      {
      case PhysicsBodyType::kStatic:
        SetMass(0);
        break;
      case PhysicsBodyType::kDynamic:
      case PhysicsBodyType::kKinematic: //TODO Implement kinematic.
        SetMass(1.0f);
        ForceWake();
        break;
      default:
        PS_LOG(Assert, "Undefined physics body type in BulletBody::SetPhysicsBodyType.");
      }
      dynamics_world_->addRigidBody(rigid_body_);
    }

    //-------------------------------------------------------------------------
    PhysicsBodyType BulletBody::GetPhysicsBodyType() const
    {
      if (GetMass() == 0.0f)
      {
        return PhysicsBodyType::kStatic;
      }
      else
      {
        return PhysicsBodyType::kDynamic;
      }
      //TODO: Kinematic check.
    }
  }
}
