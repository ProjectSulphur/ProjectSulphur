#include "constraint_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"

#include "engine/application/application.h"
#include "engine/assets/asset_system.h"
#include "engine/graphics/debug_render_system.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>
#include <graphics/platform/pipeline_state.h>

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
    void ConstraintComponent::SetFrameAMat(const glm::mat4x4& frame)
    {
      system_->GetConstraint(*this)->SetFrameA(frame);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::SetFrameBMat(const glm::mat4x4& frame)
    {
      system_->GetConstraint(*this)->SetFrameB(frame);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::AttachEntity(Entity* entity)
    {
      system_->AttachEntity(*this, *entity);
    }

    //-------------------------------------------------------------------------
    void ConstraintComponent::DetachEntity()
    {
      // TODO: Will be implemented in the near future.
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
    void HingeConstraintComponent::SetMaxAngle(float angle)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetMax(angle);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetMinAngle(float angle)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetMin(angle);
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetMinAngle() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetMin();
    }

    //-------------------------------------------------------------------------
    float HingeConstraintComponent::GetMaxAngle() const
    {
      return reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->GetMax();
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
    void HingeConstraintComponent::SetPivotA(const glm::vec3 & pivot)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetPivotA(pivot);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetPivotB(const glm::vec3 & pivot)
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
    void HingeConstraintComponent::SetFrameA(const glm::vec3& pivot, const glm::vec3& axis)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetFrameA(pivot, axis, false);
    }

    //-------------------------------------------------------------------------
    void HingeConstraintComponent::SetFrameB(const glm::vec3& pivot, const glm::vec3& axis)
    {
      reinterpret_cast<physics::HingeConstraint*>
        (system_->GetConstraint(*this))->SetFrameB(pivot, axis);
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
      foundation::Memory::Destruct(GetConstraint(handle));
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
      // Hard-coded for hinge constraint, for now.
      // Will be refactored in the near future.
      component_data_.data.template
        Get<static_cast<size_t>(ConstraintComponentElements::kConstraint)>(handle) =
        physics_service_->AddConstraint<physics::HingeConstraint>(GetEntityA(handle), entity); 
    }
  }
}
