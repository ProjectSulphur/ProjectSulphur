#include "engine/systems/components/collider_system.h"


namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    BoxColliderComponent::BoxColliderComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    BoxColliderComponent::BoxColliderComponent(System& system, size_t handle) :
      ColliderComponentBase(system, handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void BoxColliderComponent::SetExtents(const glm::vec3& e)
    {
      system_->SetExtents(*this, e);
    }

    //-------------------------------------------------------------------------
    glm::vec3 BoxColliderComponent::Extents() const
    {
      return system_->Extents(*this);
    }

    //-------------------------------------------------------------------------
    void BoxColliderSystem::SetExtents(BoxColliderComponent handle, const glm::vec3& e)
    {
      GetShape(handle)->SetExtents(e);
    }

    //-------------------------------------------------------------------------
    glm::vec3 BoxColliderSystem::Extents(BoxColliderComponent handle) const
    {
      return GetShape(handle)->GetExtents();
    }

    //-------------------------------------------------------------------------
    SphereColliderComponent::SphereColliderComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    SphereColliderComponent::SphereColliderComponent(System& system, size_t handle) :
      ColliderComponentBase(system, handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void SphereColliderComponent::SetRadius(float r)
    {
      system_->SetRadius(*this, r);
    }

    //-------------------------------------------------------------------------
    float SphereColliderComponent::Radius() const
    {
      return system_->Radius(*this);
    }

    //-------------------------------------------------------------------------
    void SphereColliderSystem::SetRadius(SphereColliderComponent handle, float r)
    {
      GetShape(handle)->SetRadius(r);
    }

    //-------------------------------------------------------------------------
    float SphereColliderSystem::Radius(SphereColliderComponent handle) const
    {
      return GetShape(handle)->GetRadius();
    }

    //-------------------------------------------------------------------------
    CapsuleColliderComponent::CapsuleColliderComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    CapsuleColliderComponent::CapsuleColliderComponent(System& system, size_t handle) :
      ColliderComponentBase(system, handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void CapsuleColliderComponent::SetRadius(float radius)
    {
      system_->SetRadius(*this, radius);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderComponent::GetRadius() const
    {
      return system_->GetRadius(*this);
    }

    //-------------------------------------------------------------------------
    void CapsuleColliderComponent::SetHeight(float height)
    {
      system_->SetHeight(*this, height);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderComponent::GetHeight() const
    {
      return system_->GetHeight(*this);
    }

    //-------------------------------------------------------------------------
    void CapsuleColliderSystem::SetRadius(CapsuleColliderComponent handle, float radius)
    {
      GetShape(handle)->SetRadius(radius);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderSystem::GetRadius(CapsuleColliderComponent handle) const
    {
      return GetShape(handle)->GetRadius();
    }

    //-------------------------------------------------------------------------
    void CapsuleColliderSystem::SetHeight(CapsuleColliderComponent handle, float height)
    {
      GetShape(handle)->SetHeight(height);
    }

    //-------------------------------------------------------------------------
    float CapsuleColliderSystem::GetHeight(CapsuleColliderComponent handle) const
    {
      return GetShape(handle)->GetHeight();
    }
  }
}