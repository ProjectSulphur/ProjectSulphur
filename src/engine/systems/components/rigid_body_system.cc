#include "rigid_body_system.h"

#include "engine/application/application.h"
#include "engine/physics/physics_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/collider_system.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    RigidBodySystem::RigidBodySystem() :
      IComponentSystem<RigidBodyComponent, RigidBodyData>("RigidBodySystem")
    {
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      physics_service_ = &app.GetService<PhysicsSystem>();
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::OnTerminate()
    {
      // TODO: Kill all rigid bodies.
    }

    //-------------------------------------------------------------------------
    RigidBodyComponent RigidBodySystem::Create(Entity entity)
    {
      if (entity.Has<TransformComponent>() == false)
      {
        entity.Add<TransformComponent>();
      }

      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(entity);

      if (body == nullptr)
      {
        body = physics_service_->CreatePhysicsBody(entity);
      }

      // Adding this component means the body is now dynamic.
      body->SetPhysicsBodyType(physics::PhysicsBodyType::kDynamic);


      return RigidBodyComponent(*this, component_data_.data.Add(
        body,
        entity));
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::Destroy(RigidBodyComponent handle)
    {
      assert(handle != ComponentHandleBase::InvalidHandle());

      Entity& ent = GetEntity(handle);
      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(ent);

      if ((ent.Has<BoxColliderComponent>() == false
        || ent.Has<SphereColliderComponent>() == false)
        && body != nullptr)
      {
        physics_service_->DestroyPhysicsBody(ent);
      }
      else
      {
        // No more rigid body, which used to make this entity non-static.
        body->SetPhysicsBodyType(physics::PhysicsBodyType::kStatic);
      }

      component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    RigidBodyComponent::RigidBodyComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    RigidBodyComponent::RigidBodyComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetMass(float mass)
    {
      system_->SetMass(*this, mass);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetMass(RigidBodyComponent handle, float mass)
    {
      Entity& entity = GetEntity(handle);
      mass = glm::max(0.0001f, mass); // Limit minimum to prevent 0-mass.
      physics_service_->GetPhysicsBody(entity)->SetMass(mass);
    }

    //-------------------------------------------------------------------------
    float RigidBodyComponent::GetMass() const
    {
      return system_->GetMass(*this);
    }

    //-------------------------------------------------------------------------
    float RigidBodySystem::GetMass(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetMass();
    }

    //-------------------------------------------------------------------------
    float RigidBodyComponent::GetInverseMass() const
    {
      return system_->GetInverseMass(*this);
    }

    //-------------------------------------------------------------------------
    float RigidBodySystem::GetInverseMass(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetInverseMass();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetInertiaTensor(const glm::vec3& inertia)
    {
      system_->SetInertiaTensor(*this, inertia);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetInertiaTensor(RigidBodyComponent handle, const glm::vec3& inertia)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->SetInertiaTensor(inertia);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodyComponent::GetInertiaTensor() const
    {
      return system_->GetInertiaTensor(*this);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodySystem::GetInertiaTensor(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetInertiaTensor();
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodyComponent::GetInverseInertiaTensor() const
    {
      return system_->GetInverseInertiaTensor(*this);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodySystem::GetInverseInertiaTensor(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetInverseInertiaTensor();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetLinearVelocity(const glm::vec3& velocity)
    {
      system_->SetLinearVelocity(*this, velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetLinearVelocity(RigidBodyComponent handle, const glm::vec3& velocity)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->SetLinearVelocity(velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::AddLinearVelocity(const glm::vec3& velocity)
    {
      system_->AddLinearVelocity(*this, velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::AddLinearVelocity(RigidBodyComponent handle, const glm::vec3& velocity)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->AddLinearVelocity(velocity);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodyComponent::GetLinearVelocity() const
    {
      return system_->GetLinearVelocity(*this);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodySystem::GetLinearVelocity(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetLinearVelocity();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetAngularVelocity(const glm::vec3& velocity)
    {
      system_->SetAngularVelocity(*this, velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetAngularVelocity(RigidBodyComponent handle, const glm::vec3& velocity)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->SetAngularVelocity(velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::AddAngularVelocity(const glm::vec3& velocity)
    {
      system_->AddAngularVelocity(*this, velocity);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::AddAngularVelocity(RigidBodyComponent handle, const glm::vec3& velocity)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->AddAngularVelocity(velocity);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodyComponent::GetAngularVelocity() const
    {
      return system_->GetAngularVelocity(*this);
    }

    //-------------------------------------------------------------------------
    glm::vec3 RigidBodySystem::GetAngularVelocity(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetAngularVelocity();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetLinearDamping(float damping)
    {
      system_->SetLinearDamping(*this, damping);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetLinearDamping(RigidBodyComponent handle, float damping)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->SetLinearDamping(damping);
    }

    //-------------------------------------------------------------------------
    float RigidBodyComponent::GetLinearDamping() const
    {
      return system_->GetLinearDamping(*this);
    }

    //-------------------------------------------------------------------------
    float RigidBodySystem::GetLinearDamping(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetLinearDamping();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::SetAngularDamping(float damping)
    {
      system_->SetAngularDamping(*this, damping);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::SetAngularDamping(RigidBodyComponent handle, float damping)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->SetAngularDamping(damping);
    }

    //-------------------------------------------------------------------------
    float RigidBodyComponent::GetAngularDamping() const
    {
      return system_->GetAngularDamping(*this);
    }

    //-------------------------------------------------------------------------
    float RigidBodySystem::GetAngularDamping(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetAngularDamping();
    }


    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyForce(const glm::vec3& force)
    {
      system_->ApplyForce(*this, force);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyForce(RigidBodyComponent handle, const glm::vec3& force)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyForce(force);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyTorque(const glm::vec3& torque)
    {
      system_->ApplyTorque(*this, torque);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyTorque(RigidBodyComponent handle, const glm::vec3& torque)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyTorque(torque);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyForceAtPosition(const glm::vec3& force,
      const glm::vec3& position)
    {
      system_->ApplyForceAtPosition(*this, force, position);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyForceAtPosition(RigidBodyComponent handle,
      const glm::vec3& force,
      const glm::vec3& position)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyForceAtPosition(force, position);
    }


    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyImpulse(const glm::vec3& impulse)
    {
      system_->ApplyImpulse(*this, impulse);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyImpulse(RigidBodyComponent handle,
      const glm::vec3& impulse)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyImpulse(impulse);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyAngularImpulse(const glm::vec3& impulse)
    {
      system_->ApplyAngularImpulse(*this, impulse);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyAngularImpulse(RigidBodyComponent handle,
      const glm::vec3& impulse)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyAngularImpulse(impulse);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ApplyImpulseAtPosition(const glm::vec3& impulse,
      const glm::vec3& position)
    {
      system_->ApplyImpulseAtPosition(*this, impulse, position);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ApplyImpulseAtPosition(RigidBodyComponent handle,
      const glm::vec3& impulse,
      const glm::vec3& position)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ApplyImpulseAtPosition(impulse, position);
    }


    //-------------------------------------------------------------------------
    bool RigidBodyComponent::IsSleeping() const
    {
      return system_->IsSleeping(*this);
    }

    //-------------------------------------------------------------------------
    bool RigidBodySystem::IsSleeping(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->IsSleeping();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ForceWake()
    {
      system_->ForceWake(*this);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ForceWake(RigidBodyComponent handle)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ForceWake();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::ForceSleep()
    {
      system_->ForceSleep(*this);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::ForceSleep(RigidBodyComponent handle)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->ForceSleep();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockTranslation(const glm::bvec3& lock_axes)
    {
      system_->LockTranslation(*this, lock_axes);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockTranslation(RigidBodyComponent handle, const glm::bvec3& lock_axes)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->LockTranslation(lock_axes);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockTranslationX(bool lock_axis)
    {
      system_->LockTranslationX(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockTranslationX(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetTranslationLock(handle);
      lock.x = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockTranslation(lock);
    }
    
    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockTranslationY(bool lock_axis)
    {
      system_->LockTranslationY(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockTranslationY(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetTranslationLock(handle);
      lock.y = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockTranslation(lock);
    }
    
    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockTranslationZ(bool lock_axis)
    {
      system_->LockTranslationZ(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockTranslationZ(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetTranslationLock(handle);
      lock.z = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockTranslation(lock);
    }

    //-------------------------------------------------------------------------
    glm::bvec3 RigidBodyComponent::GetTranslationLock() const
    {
      return system_->GetTranslationLock(*this);
    }

    //-------------------------------------------------------------------------
    glm::bvec3 RigidBodySystem::GetTranslationLock(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetTranslationLock();
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockRotation(const glm::bvec3& lock_axes)
    {
      system_->LockRotation(*this, lock_axes);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockRotation(RigidBodyComponent handle, const glm::bvec3& lock_axes)
    {
      Entity& entity = GetEntity(handle);
      physics_service_->GetPhysicsBody(entity)->LockRotation(lock_axes);
    }

    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockRotationX(bool lock_axis)
    {
      system_->LockRotationX(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockRotationX(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetRotationLock(handle);
      lock.x = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockRotation(lock);
    }
    
    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockRotationY(bool lock_axis)
    {
      system_->LockRotationY(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockRotationY(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetRotationLock(handle);
      lock.y = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockRotation(lock);
    }
    
    //-------------------------------------------------------------------------
    void RigidBodyComponent::LockRotationZ(bool lock_axis)
    {
      system_->LockRotationZ(*this, lock_axis);
    }

    //-------------------------------------------------------------------------
    void RigidBodySystem::LockRotationZ(RigidBodyComponent handle, bool lock_axis)
    {
      Entity& entity = GetEntity(handle);
      glm::bvec3 lock = GetRotationLock(handle);
      lock.z = lock_axis;
      physics_service_->GetPhysicsBody(entity)->LockRotation(lock);
    }

    //-------------------------------------------------------------------------
    glm::bvec3 RigidBodyComponent::GetRotationLock() const
    {
      return system_->GetRotationLock(*this);
    }

    //-------------------------------------------------------------------------
    glm::bvec3 RigidBodySystem::GetRotationLock(RigidBodyComponent handle) const
    {
      Entity& entity = GetEntity(handle);
      return physics_service_->GetPhysicsBody(entity)->GetRotationLock();
    }

    //-------------------------------------------------------------------------
    Entity& RigidBodySystem::GetEntity(RigidBodyComponent handle) const
    {
      return component_data_.data.
        Get<static_cast<size_t>(RigidBodyComponentElements::kEntity)>(handle);
    }
  }
}