#include "engine/systems/components/collider_system.h"

#include <physics/platform_physics_shape.h>
#include <lua-classes/collider_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    ColliderComponent::ColliderComponent() :
      system_(nullptr)
    {}

    //-------------------------------------------------------------------------
    ColliderComponent::ColliderComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {}

    //-------------------------------------------------------------------------
    void ColliderComponent::ApplyMaterial(const PhysicsMaterialHandle& material)
    {
      system_->ApplyMaterial(*this, material);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetShape(physics::PhysicsShape* shape)
    {
      system_->SetShape(*this, shape);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsShape* ColliderComponent::GetShape() const
    {
      return system_->GetShape(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetOffset(const glm::vec3& translation)
    {
      system_->SetOffset(*this, translation);
    }

    //-------------------------------------------------------------------------
    glm::vec3 ColliderComponent::GetOffset() const
    {
      return system_->GetOffset(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetLocalRotation(const glm::quat& rotation)
    {
      system_->SetLocalRotation(*this, rotation);
    }

    //-------------------------------------------------------------------------
    glm::quat ColliderComponent::GetLocalRotation() const
    {
      return system_->GetLocalRotation(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetStaticFriction(float friction)
    {
      system_->SetStaticFriction(*this, friction);
    }

    //-------------------------------------------------------------------------
    float ColliderComponent::GetStaticFriction() const
    {
      return system_->GetStaticFriction(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetDynamicFriction(float friction)
    {
      system_->SetDynamicFriction(*this, friction);
    }

    //-------------------------------------------------------------------------
    float ColliderComponent::GetDynamicFriction() const
    {
      return system_->GetDynamicFriction(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetRollingFriction(float friction)
    {
      system_->SetRollingFriction(*this, friction);
    }

    //-------------------------------------------------------------------------
    float ColliderComponent::GetRollingFriction() const
    {
      return system_->GetRollingFriction(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetFrictionCombineMode(
      physics::PhysicsCollider::MaterialCombineMode mode)
    {
      system_->SetFrictionCombineMode(*this, mode);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode
      ColliderComponent::GetFrictionCombineMode() const
    {
      return system_->GetFrictionCombineMode(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetRestitution(float restitution)
    {
      system_->SetRestitution(*this, restitution);
    }

    //-------------------------------------------------------------------------
    float ColliderComponent::GetRestitution() const
    {
      return system_->GetRestitution(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::SetRestitutionCombineMode(
      physics::PhysicsCollider::MaterialCombineMode mode)
    {
      system_->SetRestitutionCombineMode(*this, mode);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode
      ColliderComponent::GetRestitutionCombineMode() const
    {
      return system_->GetRestitutionCombineMode(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddCollisionEnterCallback(ScriptableCallback callback)
    {
      system_->AddCollisionEnterCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddCollisionStayCallback(ScriptableCallback callback)
    {
      system_->AddCollisionStayCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddCollisionLeaveCallback(ScriptableCallback callback)
    {
      system_->AddCollisionLeaveCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddOverlapEnterCallback(ScriptableCallback callback)
    {
      system_->AddOverlapEnterCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddOverlapStayCallback(ScriptableCallback callback)
    {
      system_->AddOverlapStayCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::AddOverlapLeaveCallback(ScriptableCallback callback)
    {
      system_->AddOverlapLeaveCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveCollisionEnterCallback(ScriptableCallback callback)
    {
      system_->RemoveCollisionEnterCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveCollisionStayCallback(ScriptableCallback callback)
    {
      system_->RemoveCollisionStayCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveCollisionLeaveCallback(ScriptableCallback callback)
    {
      system_->RemoveCollisionLeaveCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveOverlapEnterCallback(ScriptableCallback callback)
    {
      system_->RemoveOverlapEnterCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveOverlapStayCallback(ScriptableCallback callback)
    {
      system_->RemoveOverlapStayCallback(*this, callback);
    }

    //-------------------------------------------------------------------------
    void ColliderComponent::RemoveOverlapLeaveCallback(ScriptableCallback callback)
    {
      system_->RemoveOverlapLeaveCallback(*this, callback);
    }
    
    //-------------------------------------------------------------------------
    BoxColliderComponent::BoxColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void BoxColliderComponent::SetExtents(const glm::vec3& e)
    {
      reinterpret_cast<physics::PhysicsBoxShape*>(system_->GetShape(*this))->SetExtents(e);
    }

    //-------------------------------------------------------------------------
    glm::vec3 BoxColliderComponent::Extents() const
    {
      return reinterpret_cast<physics::PhysicsBoxShape*>(
        system_->GetShape(*this))->GetExtents();
    }

    //-------------------------------------------------------------------------
    SphereColliderComponent::SphereColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void SphereColliderComponent::SetRadius(float r)
    {
      reinterpret_cast<physics::PhysicsSphereShape*>(
        system_->GetShape(*this))->SetRadius(r);
    }

    //-------------------------------------------------------------------------
    float SphereColliderComponent::GetRadius() const
    {
      return reinterpret_cast<physics::PhysicsSphereShape*>(
        system_->GetShape(*this))->GetRadius();
    }

    //-------------------------------------------------------------------------
    CylinderColliderComponent::CylinderColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void CylinderColliderComponent::SetRadius(float radius)
    {
      reinterpret_cast<physics::PhysicsCylinderShape*>(
        system_->GetShape(*this))->SetRadius(radius);
    }

    //-------------------------------------------------------------------------
    float CylinderColliderComponent::GetRadius() const
    {
      return reinterpret_cast<physics::PhysicsCylinderShape*>(
        system_->GetShape(*this))->GetRadius();
    }

    //-------------------------------------------------------------------------
    void CylinderColliderComponent::SetHeight(float height)
    {
      reinterpret_cast<physics::PhysicsCylinderShape*>(
        system_->GetShape(*this))->SetHeight(height);
    }

    //-------------------------------------------------------------------------
    float CylinderColliderComponent::GetHeight() const
    {
      return reinterpret_cast<physics::PhysicsCylinderShape*>(
        system_->GetShape(*this))->GetHeight();
    }

    //-------------------------------------------------------------------------
    CapsuleColliderComponent::CapsuleColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void CapsuleColliderComponent::SetRadius(float radius)
    {
      reinterpret_cast<physics::PhysicsCapsuleShape*>(
        system_->GetShape(*this))->SetRadius(radius);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderComponent::GetRadius() const
    {
      return reinterpret_cast<physics::PhysicsCapsuleShape*>(
        system_->GetShape(*this))->GetRadius();
    }

    //-------------------------------------------------------------------------
    void CapsuleColliderComponent::SetHeight(float height)
    {
      reinterpret_cast<physics::PhysicsCapsuleShape*>(
        system_->GetShape(*this))->SetHeight(height);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderComponent::GetHeight() const
    {
      return reinterpret_cast<physics::PhysicsCapsuleShape*>(
        system_->GetShape(*this))->GetHeight();
    }
    //-------------------------------------------------------------------------
    ConeColliderComponent::ConeColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void ConeColliderComponent::SetRadius(float radius)
    {
      reinterpret_cast<physics::PhysicsConeShape*>(
        system_->GetShape(*this))->SetRadius(radius);
    }

    //-------------------------------------------------------------------------
    float ConeColliderComponent::GetRadius() const
    {
      return reinterpret_cast<physics::PhysicsConeShape*>(
        system_->GetShape(*this))->GetRadius();
    }

    //-------------------------------------------------------------------------
    void ConeColliderComponent::SetHeight(float height)
    {
      reinterpret_cast<physics::PhysicsConeShape*>(
        system_->GetShape(*this))->SetHeight(height);
    }

    //-------------------------------------------------------------------------
    float ConeColliderComponent::GetHeight() const
    {
      return reinterpret_cast<physics::PhysicsConeShape*>(
        system_->GetShape(*this))->GetHeight();
    }

    //-------------------------------------------------------------------------
    ConvexColliderComponent::ConvexColliderComponent(System& system, size_t handle) :
      ColliderComponent(system, handle)
    {}

    //-------------------------------------------------------------------------
    void ConvexColliderComponent::SetMesh(MeshHandle mesh)
    {
      system_->SetMesh(*this, mesh);
    }

    //-------------------------------------------------------------------------
    MeshHandle ConvexColliderComponent::GetMesh() const
    {
      return system_->GetMesh(*this);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetMesh(ColliderComponent handle, MeshHandle mesh)
    {
      if (!(handle.GetShape()->GetType() == physics::PhysicsShape::ShapeTypes::kConvex ||
          handle.GetShape()->GetType() == physics::PhysicsShape::ShapeTypes::kMesh))
      {
        PS_LOG(Warning, "Attempted to set a mesh on a primitive collider component.");
        return;
      }

      bool is_convex = true;
      if (handle.GetShape()->GetType() == physics::PhysicsShape::ShapeTypes::kMesh)
      {
        is_convex = false;
      }

      physics::PhysicsCollider* col =
        physics_service_->CreateMeshCollider(GetEntity(handle), mesh, is_convex);

      physics_service_->DestroyCollider(GetCollider(handle));
      component_data_.data.
        Get<static_cast<size_t>(ColliderComponentElements::kCollider)>(handle) = col;
      component_data_.data.
        Get<static_cast<size_t>(ColliderComponentElements::kMesh)>(handle) = mesh;
    }

    //-------------------------------------------------------------------------
    MeshHandle ColliderSystem::GetMesh(ColliderComponent handle) const
    {
      return component_data_.data.
        Get<static_cast<size_t>(ColliderComponentElements::kMesh)>(handle);
    }

    //-------------------------------------------------------------------------
    ColliderSystem::ColliderSystem() :
      IComponentSystem("ColliderSystem")
    {
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      physics_service_ = &app.GetService<PhysicsSystem>();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider* ColliderSystem::GetCollider(ComponentHandleBase handle) const
    {
      return component_data_.data.template
        Get<static_cast<size_t>(ColliderComponentElements::kCollider)>(handle);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::Destroy(ComponentHandleBase handle)
    {
      Entity ent = component_data_.data.template
        Get<static_cast<size_t>(ColliderComponentElements::kEntity)>(handle);

      physics_service_->DestroyCollider(GetCollider(handle));
      component_data_.data.Remove(handle);

      // PhysicsBody deletion hotfix. Function checks internally if it should be deleted for now.
      physics_service_->DestroyPhysicsBody(ent);
    }

    //-------------------------------------------------------------------------
    Entity ColliderSystem::GetEntity(ColliderComponent handle) const
    {
      return component_data_.data.template
        Get<static_cast<size_t>(ColliderComponentElements::kEntity)>(handle);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetShape(ColliderComponent handle,
      physics::PhysicsShape* shape)
    {
      physics_service_->DestroyCollider(GetCollider(handle));
      physics::PhysicsCollider* col = physics_service_->CreateCollider(GetEntity(handle), shape);
      component_data_.data.template
        Get<static_cast<size_t>(ColliderComponentElements::kCollider)>(handle) = col;
    }

    //-------------------------------------------------------------------------
    physics::PhysicsShape* ColliderSystem::GetShape(ColliderComponent handle) const
    {
      return GetCollider(handle)->shape();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetOffset(ColliderComponent handle,
      const glm::vec3& translation)
    {
      GetCollider(handle)->SetTranslation(translation);
    }

    //-------------------------------------------------------------------------
    const glm::vec3& ColliderSystem::GetOffset(ColliderComponent handle) const
    {
      return GetCollider(handle)->translation();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetLocalRotation(ColliderComponent handle,
      const glm::quat& rotation)
    {
      GetCollider(handle)->SetRotation(rotation);
    }

    //-------------------------------------------------------------------------
    const glm::quat& ColliderSystem::GetLocalRotation(ColliderComponent handle) const
    {
      return GetCollider(handle)->rotation();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::ApplyMaterial(ColliderComponent handle,
      const PhysicsMaterialHandle& material)
    {
      SetStaticFriction(handle, material.GetRaw()->static_friction());
      SetDynamicFriction(handle, material.GetRaw()->dynamic_friction());
      SetRollingFriction(handle, material.GetRaw()->rolling_friction());
      SetRestitution(handle, material.GetRaw()->restitution());
      SetFrictionCombineMode(handle, material.GetRaw()->friction_combine_mode());
      SetRestitutionCombineMode(handle, material.GetRaw()->restitution_combine_mode());
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetStaticFriction(ColliderComponent handle,
      float friction)
    {
      GetCollider(handle)->SetStaticFriction(friction);
    }

    //-------------------------------------------------------------------------
    float ColliderSystem::GetStaticFriction(ColliderComponent handle) const
    {
      return GetCollider(handle)->GetStaticFriction();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetDynamicFriction(ColliderComponent handle,
      float friction)
    {
      GetCollider(handle)->SetDynamicFriction(friction);
    }

    //-------------------------------------------------------------------------
    float ColliderSystem::GetDynamicFriction(ColliderComponent handle) const
    {
      return GetCollider(handle)->GetDynamicFriction();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetRollingFriction(ColliderComponent handle,
      float friction)
    {
      GetCollider(handle)->SetRollingFriction(friction);
    }

    //-------------------------------------------------------------------------
    float ColliderSystem::GetRollingFriction(ColliderComponent handle) const
    {
      return GetCollider(handle)->GetRollingFriction();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetFrictionCombineMode(ColliderComponent handle,
      physics::PhysicsCollider::MaterialCombineMode mode)
    {
      GetCollider(handle)->SetFrictionCombineMode(mode);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode
      ColliderSystem::GetFrictionCombineMode(ColliderComponent handle) const
    {
      return GetCollider(handle)->GetFrictionCombineMode();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetRestitution(ColliderComponent handle,
      float restitution)
    {
      GetCollider(handle)->SetRestitution(restitution);
    }

    //-------------------------------------------------------------------------
    float ColliderSystem::GetRestitution(ColliderComponent handle) const
    {
      return GetCollider(handle)->GetRestitution();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::SetRestitutionCombineMode(ColliderComponent handle,
      physics::PhysicsCollider::MaterialCombineMode mode)
    {
      GetCollider(handle)->SetRestitutionCombineMode(mode);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode ColliderSystem::GetRestitutionCombineMode
      (ColliderComponent handle) const
    {
      return GetCollider(handle)->GetRestitutionCombineMode();
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::AddCollisionEnterCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddCollisionEnterCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::AddCollisionStayCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddCollisionStayCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::AddCollisionLeaveCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddCollisionLeaveCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::AddOverlapEnterCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddOverlapEnterCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::AddOverlapStayCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddOverlapStayCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::AddOverlapLeaveCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->AddOverlapLeaveCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveCollisionEnterCallback(
      ColliderComponent handle, ScriptableCallback callback)
    {
      physics_service_->RemoveCollisionEnterCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveCollisionStayCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->RemoveCollisionStayCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveCollisionLeaveCallback(
      ColliderComponent handle, ScriptableCallback callback)
    {
      physics_service_->RemoveCollisionLeaveCallback(GetEntity(handle), callback);
    }
    
    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveOverlapEnterCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->RemoveOverlapEnterCallback(GetEntity(handle), callback);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveOverlapStayCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->RemoveOverlapStayCallback(GetEntity(handle), callback);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::RemoveOverlapLeaveCallback(ColliderComponent handle,
      ScriptableCallback callback)
    {
      physics_service_->RemoveOverlapLeaveCallback(GetEntity(handle), callback);
    }

    //-------------------------------------------------------------------------
    void ColliderSystem::CreatePhysicsBody(Entity& entity)
    {
      if (entity.Has<TransformComponent>() == false)
      {
        entity.Add<TransformComponent>();
      }

      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(entity);
      if (body == nullptr)
      {
        body = physics_service_->CreatePhysicsBody(entity);
        // Explicitly setting to static here, body can only have this collider at this point.
        // If the body already exists, adding this component won't change the body state.
        body->SetPhysicsBodyType(physics::PhysicsBodyType::kStatic);
      }
    }
  }
}