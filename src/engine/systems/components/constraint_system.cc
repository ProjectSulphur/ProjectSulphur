#include "constraint_system.h"

#include <lua-classes/constraint_system.lua.cc>

namespace sulphur
{
  namespace engine
  {

    //-------------------------------------------------------------------------
    ConstraintComponent::ConstraintComponent() :
      system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    ConstraintComponent::ConstraintComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetTorqueLimit(float torque)
    {
      system_->GetConstraint(*this)->SetTorqueLimit(torque);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetForceLimit(float force)
    {
      system_->GetConstraint(*this)->SetForceLimit(force);
    }

    //-------------------------------------------------------------------------
    float ConstraintComponent::GetTorqueLimit() const
    {
      return system_->GetConstraint(*this)->GetTorqueLimit();
    }

    //-------------------------------------------------------------------------
    float ConstraintComponent::GetForceLimit() const
    {
      return system_->GetConstraint(*this)->GetForceLimit();
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 ConstraintComponent::GetFrameA() const
    {
      return system_->GetConstraint(*this)->GetFrameA();
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 ConstraintComponent::GetFrameB() const
    {
      return system_->GetConstraint(*this)->GetFrameB();
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetEnabled(bool enabled)
    {
      system_->GetConstraint(*this)->SetEnabled(enabled);
    }

    //-------------------------------------------------------------------------
    bool ConstraintComponent::IsEnabled() const
    {
      return system_->GetConstraint(*this)->IsEnabled();
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetFrameA(const glm::mat4x4& frame)
    {
      system_->GetConstraint(*this)->SetFrameA(frame);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetFrameB(const glm::mat4x4& frame)
    {
      system_->GetConstraint(*this)->SetFrameB(frame);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::AttachEntity(Entity* entity)
    {
      Entity entity2 = Entity();
      if (system_->GetEntityB(*this) != entity2 )
      {
        DetachEntity();
      }

      system_->AttachEntity(*this, *entity);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::DetachEntity()
    {
      system_->DetachEntity(*this );
    }

    //-------------------------------------------------------------------------
    Entity ConstraintComponent::GetEntityA() const
    {
      return system_->GetEntityA(*this);
    }

    //-------------------------------------------------------------------------
    Entity ConstraintComponent::GetEntityB() const
    {
      return system_->GetEntityB(*this);
    }



    //-------------------------------------------------------------------------
    FixedConstraintComponent::FixedConstraintComponent(System& system, size_t handle) :
      ConstraintComponent(system, handle)
    {
    }

    //-------------------------------------------------------------------------
    FixedConstraintComponent::FixedConstraintComponent() :
      ConstraintComponent()
    {
    }


    //-------------------------------------------------------------------------
    HingeConstraintComponent::HingeConstraintComponent(System& system, size_t handle) :
      ConstraintComponent(system, handle)
    {
    }

    //-------------------------------------------------------------------------
    HingeConstraintComponent::HingeConstraintComponent() :
      ConstraintComponent()
    {
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetMaximumAngle(float angle)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetMaximumAngle(angle);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetMinimumAngle(float angle)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetMinimumAngle(angle);
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetMinimumAngle() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetMinimumAngle();
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetMaximumAngle() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetMaximumAngle();
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetHingeAngle() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetHingeAngle();
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetAxisA(const glm::vec3& axis)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetAxisA(axis);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetAxisB(const glm::vec3 & axis)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetAxisB(axis);
    }

    //-------------------------------------------------------------------------
    glm::vec3 HingeConstraintComponent::GetAxisA() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetAxisA();
    }

    //-------------------------------------------------------------------------
    glm::vec3 HingeConstraintComponent::GetAxisB() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetAxisB();
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetPivotA(const glm::vec3& pivot)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetPivotA(pivot);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetPivotB(const glm::vec3& pivot)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetPivotB(pivot);
    }

    //-------------------------------------------------------------------------
    glm::vec3 HingeConstraintComponent::GetPivotB() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetPivotB();
    }

    //-------------------------------------------------------------------------
    glm::vec3 HingeConstraintComponent::GetPivotA() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetPivotA();
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetSoftness(float softness)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetSoftness(softness);
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetSoftness() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetSoftness();
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetBiasFactor(float bias)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetBiasFactor(bias);
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetBiasFactor() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetBiasFactor();
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetRelaxationFactor(float relaxation)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetRelaxationFactor(relaxation);
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetRelaxationFactor() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetRelaxationFactor();
    }


    //-------------------------------------------------------------------------
    ConstraintSystem::ConstraintSystem() :
      IComponentSystem("ConstraintSystem")
    {
    }

    //-------------------------------------------------------------------------
    void ConstraintSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      physics_service_ = &app.GetService<PhysicsSystem>();
    }

    //-------------------------------------------------------------------------
    void ConstraintSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    void ConstraintSystem::Destroy(ComponentHandleBase handle)
    {
      physics_service_->DestroyConstraint(GetConstraint(handle));
      this->component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    physics::IPhysicsConstraint* ConstraintSystem::GetConstraint(ComponentHandleBase handle) const
    {
      return this->component_data_.data.template
        Get<static_cast<size_t>(ConstraintComponentElements::kConstraint)>(handle);
    }

    //-------------------------------------------------------------------------
    Entity ConstraintSystem::GetEntityA(ConstraintComponent handle) const
    {
      return this->component_data_.data.template
        Get<static_cast<size_t>(ConstraintComponentElements::kEntity_a)>(handle);
    }

    //-------------------------------------------------------------------------
    Entity ConstraintSystem::GetEntityB(ConstraintComponent handle) const
    {
      return this->component_data_.data.template
        Get<static_cast<size_t>(ConstraintComponentElements::kEntity_b)>(handle);
    }

    //-------------------------------------------------------------------------
    void ConstraintSystem::AttachEntity(ConstraintComponent handle, Entity entity)
    {
      this->component_data_.data.template
        Get < static_cast<size_t>(ConstraintComponentElements::kEntity_b)>(handle) = entity;

      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(entity);

      if (body == nullptr)
      {
        body = physics_service_->CreatePhysicsBody(entity);
      }

      GetConstraint(handle)->SetBodyB(body);
    }

    //-------------------------------------------------------------------------
    void ConstraintSystem::DetachEntity(ConstraintComponent handle)
    {
      this->component_data_.data.template
        Get < static_cast<size_t>(ConstraintComponentElements::kEntity_b)>(handle) = Entity();

      GetConstraint(handle)->SetBodyB(nullptr);
    }
  }
}
