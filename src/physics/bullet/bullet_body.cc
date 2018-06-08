#include "physics/bullet/bullet_body.h"

#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"
#include "physics/bullet/bullet_collider.h"

#include "physics/iphysics_shape.h"

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
      : dynamics_world_(dynamics_world),
      kinematic_mass_(1.0f),
      friction_combine_mode_(PhysicsCollider::kAverage),
      restitution_combine_mode_(PhysicsCollider::kAverage),
      static_friction_(0.6f)
    {
      btTransform bt_transform;
      bt_transform.setIdentity();
      bt_transform.setOrigin(BulletConversions::ToBt(translation));
      bt_transform.setRotation(BulletConversions::ToBt(rotation));

      btTransform bt_transform2 = btTransform::getIdentity();
      bt_transform2.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

      motion_state_ = foundation::Memory::Construct<btDefaultMotionState>
                      (bt_transform, bt_transform2);
      
      btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion_state_, nullptr);
      shape_ = foundation::Memory::Construct<btCompoundShape>();
      info.m_collisionShape = shape_;

      rigid_body_ = foundation::Memory::Construct<btRigidBody>(info);
      rigid_body_->setUserPointer(reinterpret_cast<void*>(this));

      rigid_body_->setCollisionFlags(rigid_body_->getCollisionFlags() |
        btCollisionObject::CollisionFlags::CF_CUSTOM_MATERIAL_CALLBACK);
      if (gContactAddedCallback != &CustomMaterialCallback)
      {
        gContactAddedCallback = &CustomMaterialCallback;
      }

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
      btTransform transform;

      reinterpret_cast<btDefaultMotionState*>(rigid_body_->getMotionState())
                                             ->getWorldTransform(transform);
      transform.setOrigin(BulletConversions::ToBt(translation));
      
      // I still need to find out why both the rigidbody and the motionstate have to be set
      rigid_body_->getWorldTransform().setOrigin(BulletConversions::ToBt(translation));
      ForceWake();

    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletBody::GetTranslation() const
    {
      btTransform transform = rigid_body_->getWorldTransform();
      return BulletConversions::ToGlm(transform.getOrigin());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetRotation(const glm::quat& rotation)
    {
      btTransform& transform = rigid_body_->getWorldTransform();
      transform.setRotation(BulletConversions::ToBt(rotation));

      ForceWake();
    }

    //-------------------------------------------------------------------------
    glm::quat BulletBody::GetRotation() const
    {
      btTransform transform = rigid_body_->getWorldTransform();
      return BulletConversions::ToGlm(transform.getRotation());
    }

    //-------------------------------------------------------------------------
    void BulletBody::SetMass(float mass)
    {
      btVector3 local_inertia(0,0,0);
      if (rigid_body_->getCollisionShape() != nullptr &&
        rigid_body_->getCollisionShape()->getShapeType() !=
            BroadphaseNativeTypes::EMPTY_SHAPE_PROXYTYPE)
      {
        rigid_body_->getCollisionShape()->calculateLocalInertia(mass, local_inertia);
      }
      rigid_body_->setMassProps(mass, local_inertia);
      rigid_body_->updateInertiaTensor();
      ForceWake();
    }

    //-------------------------------------------------------------------------
    float BulletBody::GetMass() const
    {
      if (GetPhysicsBodyType() == PhysicsBodyType::kKinematic)
      {
        return kinematic_mass_; // Return stored value for this case, else it returns 0.
      }

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
      ForceWake();
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
      ForceWake();
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
      ForceWake();
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
      return GetPhysicsBodyType() == PhysicsBodyType::kStatic || rigid_body_->isActive() == false;
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
      ForceWake();
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
      ForceWake();
    }

    //-------------------------------------------------------------------------
    glm::bvec3 BulletBody::GetRotationLock() const
    {
      btVector3 lock = rigid_body_->getAngularFactor();
      return glm::bvec3(lock.x() == 0.f, lock.y() == 0.f, lock.z() == 0.f);
    }

    //-------------------------------------------------------------------------
    PhysicsCollider* BulletBody::AddShape(IPhysicsShape* shape)
    {
      PhysicsCollider* new_collider = foundation::Memory::Construct<BulletCollider>
        (reinterpret_cast<IPhysicsBody*>(this), shape);

      shape_->addChildShape(btTransform::getIdentity(),
        reinterpret_cast<btCollisionShape*>(shape->GetInternalShape()));
      colliders_.push_back(new_collider);

      // Update inertia tensor. This will override a manually set inertia tensor
      SetMass(GetMass());

      return new_collider;
    }

    //-------------------------------------------------------------------------
    void BulletBody::RemoveShape(PhysicsCollider* collider)
    {
      int collider_index = GetColliderIndex(collider);
      assert(collider_index != -1 &&
        "Attempted to remove collider that is not attached to this body.");

      shape_->removeChildShapeByIndex(collider_index);
      // removeChild only updates the dynamic AABB tree, so we have to call it manually too.
      shape_->recalculateLocalAabb();

      // Removing collider from the list the same way Bullet does it,
      // in order to keep the collider order in sync with Bullet. DO NOT CHANGE.
      colliders_[collider_index] = colliders_.back();
      colliders_.pop_back();

      SetMass(GetMass());

      foundation::Memory::Destruct(collider);
    }

    //-------------------------------------------------------------------------
    void BulletBody::UpdateCollider(PhysicsCollider* collider)
    {
      int collider_index = GetColliderIndex(collider);
      assert(collider_index != -1 &&
        "Attempted to update collider that is not attached to this body.");

      btTransform offset(BulletConversions::ToBt(collider->rotation()),
        BulletConversions::ToBt(collider->translation()));

      // This updates the collider offset if necessary and updates the AABBs internally.
      // Do NOT use shape_->recalculateLocalAabb().
      shape_->updateChildTransform(collider_index, offset, true);

      SetMass(GetMass()); // Inertia may have changed. This updates it.
    }


    //-------------------------------------------------------------------------
    void BulletBody::SetPhysicsBodyType(PhysicsBodyType type)
    {
      if (GetPhysicsBodyType() == type)
      {
        return;
      }

      PhysicsBodyType old_type = GetPhysicsBodyType();

      // Switching between static and non-static requires the body to be
      // temporarily removed from the simulation. I couldn't find it documented 
      // anywhere, but the physics world doesn't get notified about this change otherwise.
      bool static_switch = (type == PhysicsBodyType::kStatic || 
                          old_type == PhysicsBodyType::kStatic);

      if (static_switch == true)
      {
        dynamics_world_->removeRigidBody(rigid_body_);
      }

      switch (type)
      {
      case PhysicsBodyType::kKinematic:
        if (old_type == PhysicsBodyType::kDynamic)
        {
          kinematic_mass_ = GetMass(); //Store the mass for later.
        }
        rigid_body_->setCollisionFlags(rigid_body_->getCollisionFlags() |
          btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
        SetMass(0.0f);
        break;

      case PhysicsBodyType::kStatic:
        rigid_body_->setCollisionFlags(rigid_body_->getCollisionFlags() &
          ~btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
        SetMass(0.0f);
        break;

      case PhysicsBodyType::kDynamic:
        rigid_body_->setCollisionFlags(rigid_body_->getCollisionFlags() &
          ~btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
        SetMass(kinematic_mass_); //Restore the mass, or set to default.
        break;
      }

      if (static_switch == true)
      {
        dynamics_world_->addRigidBody(rigid_body_);
      }
      else if (type == PhysicsBodyType::kDynamic)
      {
        ForceWake();
      }
    }

    //-------------------------------------------------------------------------
    PhysicsBodyType BulletBody::GetPhysicsBodyType() const
    {
      if(rigid_body_->isKinematicObject() == true)
      {
        return PhysicsBodyType::kKinematic;
      }
      if (rigid_body_->isStaticObject() == true)
      {
        return PhysicsBodyType::kStatic;
      }
      return PhysicsBodyType::kDynamic;
    }

    //-------------------------------------------------------------------------
    void BulletBody::set_friction_combine_mode(PhysicsCollider::MaterialCombineMode mode)
    {
      friction_combine_mode_ = mode;
    }

    //-------------------------------------------------------------------------
    PhysicsCollider::MaterialCombineMode BulletBody::friction_combine_mode() const
    {
      return friction_combine_mode_;
    }

    //-------------------------------------------------------------------------
    void BulletBody::set_restitution_combine_mode(PhysicsCollider::MaterialCombineMode mode)
    {
      restitution_combine_mode_ = mode;
    }

    //-------------------------------------------------------------------------
    PhysicsCollider::MaterialCombineMode BulletBody::restitution_combine_mode() const
    {
      return restitution_combine_mode_;
    }

    //-------------------------------------------------------------------------
    void BulletBody::set_static_friction(float friction)
    {
      static_friction_ = friction;
    }

    //-------------------------------------------------------------------------
    float BulletBody::static_friction() const
    {
      return static_friction_;
    }

    //-------------------------------------------------------------------------
    btRigidBody* BulletBody::rigid_body() const
    {
      return rigid_body_;
    }

    //-------------------------------------------------------------------------
    int BulletBody::GetColliderIndex(PhysicsCollider* collider) const
    {
      for (size_t i = 0; i < colliders_.size(); ++i)
      {
        if (collider == colliders_[i])
        {
          return i;
        }
      }
      return -1;
    }

    //-------------------------------------------------------------------------
    bool BulletBody::CustomMaterialCallback(btManifoldPoint& contact_point,
      const btCollisionObjectWrapper* col_obj0_wrap, int /*part_id0*/, int /*index0*/,
      const btCollisionObjectWrapper* col_obj1_wrap, int /*part_id1*/, int /*index1*/)
    {
      PhysicsBody* body0 = reinterpret_cast<PhysicsBody*>
        (col_obj0_wrap->getCollisionObject()->getUserPointer());
      PhysicsBody* body1 = reinterpret_cast<PhysicsBody*>
        (col_obj1_wrap->getCollisionObject()->getUserPointer());

      assert(body0 != nullptr && "Unable to retrieve PhysicsBody pointer from collision object.");
      assert(body1 != nullptr && "Unable to retrieve PhysicsBody pointer from collision object.");

      float friction0 = body0->rigid_body()->getFriction();
      float friction1 = body1->rigid_body()->getFriction();
      float rollfriction0 = body0->rigid_body()->getRollingFriction();
      float rollfriction1 = body1->rigid_body()->getRollingFriction();
      float restitution0 = body0->rigid_body()->getRestitution();
      float restitution1 = body1->rigid_body()->getRestitution();


      float staticfriction0 = body0->static_friction_;
      float staticfriction1 = body1->static_friction_;

      // Check for static vs dynamic friction only if it may influence anything
      if (friction0 != staticfriction0 || friction1 != staticfriction1)
      {
        // To pick between static and dynamic friction, relative velocity of both objects
        // on that contact point is checked and difference calculated.
        // If it's below a threshold (or ideally, 0) static friction is used.

        glm::vec3 velocity0 = body0->GetLinearVelocity() +
          glm::cross(body0->GetAngularVelocity(), BulletConversions::ToGlm(
            contact_point.getPositionWorldOnA()) - body0->GetTranslation());
        glm::vec3 velocity1 = body1->GetLinearVelocity() +
          glm::cross(body1->GetAngularVelocity(), BulletConversions::ToGlm(
            contact_point.getPositionWorldOnB()) - body1->GetTranslation());

        if (glm::length(velocity0 - velocity1) <= PhysicsCollider::kStaticFrictionThreshold)
        {
          friction0 = staticfriction0;
          friction1 = staticfriction1;
        }
      }

      PhysicsCollider::MaterialCombineMode frictioncombine = 
        static_cast<PhysicsCollider::MaterialCombineMode>
        (glm::max(static_cast<int>(body0->friction_combine_mode_),
          static_cast<int>(body1->friction_combine_mode_)));

      PhysicsCollider::MaterialCombineMode restitutioncombine = 
        static_cast<PhysicsCollider::MaterialCombineMode>
        (glm::max(static_cast<int>(body0->restitution_combine_mode_),
          static_cast<int>(body1->restitution_combine_mode_)));

      switch (frictioncombine)
      {
      case PhysicsCollider::kAverage:
        contact_point.m_combinedFriction = (friction0 + friction1) * 0.5f;
        contact_point.m_combinedRollingFriction = (rollfriction0 + rollfriction1) * 0.5f;
        break;
      case PhysicsCollider::kMin:
        contact_point.m_combinedFriction = glm::min(friction0, friction1);
        contact_point.m_combinedRollingFriction = glm::min(rollfriction0, rollfriction1);
        break;
      case PhysicsCollider::kMultiply:
        contact_point.m_combinedFriction = friction0 * friction1;
        contact_point.m_combinedRollingFriction = rollfriction0 * rollfriction1;
        break;
      case PhysicsCollider::kMax:
        contact_point.m_combinedFriction = glm::max(friction0, friction1);
        contact_point.m_combinedRollingFriction = glm::max(rollfriction0, rollfriction1);
        break;
      default:
        PS_LOG(Fatal, "Enum value PhysicsCollider::MaterialCombineMode out of range.");
      }

      switch (restitutioncombine)
      {
      case PhysicsCollider::kAverage:
        contact_point.m_combinedRestitution = (restitution0 + restitution1) * 0.5f;
        break;
      case PhysicsCollider::kMin:
        contact_point.m_combinedRestitution = glm::min(restitution0, restitution1);
        break;
      case PhysicsCollider::kMultiply:
        contact_point.m_combinedRestitution = restitution0 * restitution1;
        break;
      case PhysicsCollider::kMax:
        contact_point.m_combinedRestitution = glm::max(restitution0, restitution1);
        break;
      default:
        PS_LOG(Fatal, "Enum value PhysicsCollider::MaterialCombineMode out of range.");
      }

      return true;
    }
  }
}
