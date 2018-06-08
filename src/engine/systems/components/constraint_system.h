#pragma once

#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/physics/physics_system.h"

#include <physics/iphysics_constraint.h>

namespace sulphur
{
  namespace engine
  {
    class ConstraintSystem;

    /**
    * @class sulphur::engine::ConstraintComponent : public sulphur::engine::ComponentHandleBase
    * @brief A component that allows you to add a constraint to an entity.
    * @author Daniel Konings, Benjamin Waanders, Angelo van der Mark
    */
    class ConstraintComponent : public ComponentHandleBase
    {
    public:
      using System = ConstraintSystem; //!< System type define.

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      ConstraintComponent();

      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ConstraintComponent::System&) A reference to
      * the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      ConstraintComponent(System& system, size_t handle);

      /**
      * @brief Adds an entity to the constraint component and encouples it.
      * @param[in] entity (sulphur::engine::Entity*) The entity to which the constraint is linked (entity B).
      */
      SCRIPT_FUNC_EX() void AttachEntity(Entity* entity);

      /**
      * @brief Detaches entity B from the constraint.
      */
      SCRIPT_FUNC_EX() void DetachEntity();

      /**
      * @brief Sets the torque limit of this constraint.
      * @param[in] torque (float) The torque limit.
      */
      SCRIPT_FUNC_EX() void SetTorqueLimit(float torque);

      /**
      * @brief Sets the force limit of this constraint.
      * @param[in] force (float) The force limit.
      */
      SCRIPT_FUNC_EX() void SetForceLimit(float force);

      /**
      * @brief Gets the torque limit of this constraint.
      * @return (float) The torque limit.
      */
      SCRIPT_FUNC_EX() float GetTorqueLimit() const;

      /**
      * @brief Gets the force limit of this constraint.
      * @return (float) The force limit.
      */
      SCRIPT_FUNC_EX() float GetForceLimit() const;

      /**
      * @brief Sets whether a constraint is active in the world. Can be used to re-enable broken constraints.
      * @param[in] enabled (bool) Whether the constraint should be active or not.
      */
      SCRIPT_FUNC_EX() void SetEnabled(bool enabled);

      /**
      * @brief Returns whether the constraint is active or not. Broken constraints are considered inactive.
      * @return (bool) True if the constraint is active.
      */
      SCRIPT_FUNC_EX() bool IsEnabled() const;

      /**
      * @brief Sets the reference frame of the owning entity relative to the constraint.
      * @param[in] transform (const glm::mat4x4&) The relative transform.
      */
      SCRIPT_FUNC_EX() void SetFrameA(const glm::mat4x4& transform);

      /**
      * @brief Returns the reference frame of the owning entity relative to the constraint.
      * @return (glm::mat4x4) The relative transform.
      */
      SCRIPT_FUNC_EX() glm::mat4x4 GetFrameA() const;

      /**
      * @brief Sets the reference frame of the attached entity relative to the constraint.
      * @param[in] transform (const glm::mat4x4&) The relative transform.
      */
      SCRIPT_FUNC_EX() void SetFrameB(const glm::mat4x4& transform);

      /**
      * @brief Returns the reference frame of the attached entity relative to the constraint.
      * @return (glm::mat4x4) The relative transform.
      */
      SCRIPT_FUNC_EX() glm::mat4x4 GetFrameB() const;

      /**
      * @brief Gets the A entity of this constraint (owner).
      * @return (sulphur::engine::Entity) Handle to the entity.
      */
      SCRIPT_FUNC_EX() Entity GetEntityA() const;

      /**
      * @brief Gets the B entity of this constraint if there is one.
      * @return (sulphur::engine::Entity) Handle to the entity, or invalid handle.
      */
      SCRIPT_FUNC_EX() Entity GetEntityB() const;

    protected:
      System* system_; //!< The system that owns this component
    };

    /**
    * @class sulphur::engine::FixedConstraintComponent : public sulphur::engine::ConstraintComponent
    * @brief The fixed constraint component to couple two components together via a fixed constraint.
    * @author Benjamin Waanders
    */
    SCRIPT_CLASS() class FixedConstraintComponent : public ConstraintComponent
    {
    public:

      SCRIPT_NAME(FixedConstraintComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief The default constructor.
      */
      FixedConstraintComponent();
      
      /**
      * @brief Constructor that assigns the component to the correct system.
      */
      FixedConstraintComponent(System& system, size_t handle);
    };

    /**
    * @class sulphur::engine::HingeConstraintComponent : public sulphur::engine::ConstraintComponent
    * @brief The hinge constraint component to couple two components together via a hinge constraint.
    * @author Benjamin Waarnders, Angelo van der Mark
    */
    SCRIPT_CLASS() class HingeConstraintComponent : public ConstraintComponent
    {
    public:

      SCRIPT_NAME(HingeConstraintComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief The default constructor.
      */
      HingeConstraintComponent();
      
      /**
      * @brief constructor that assigns the component to the correct system.
      */
      HingeConstraintComponent(System& system, size_t handle);

      /**
      * @brief Sets the maximum range for the hinge in radians.
      * @param[in] angle (float) The maximum angle.
      */
      SCRIPT_FUNC() void SetMaximumAngle(float angle);

      /**
      * @brief Sets the minimum range for the hinge in radians.
      * @param[in] angle (float) The minimum angle.
      */
      SCRIPT_FUNC() void SetMinimumAngle(float angle);

      /**
      * @brief Gets the minimum range for the hinge in radians.
      * @return (float) The minimum angle.
      */
      SCRIPT_FUNC() float GetMinimumAngle() const;

      /**
      * @brief Gets the maximum range for the hinge in radians.
      * @return (float) The maximum angle.
      */
      SCRIPT_FUNC() float GetMaximumAngle() const;

      /**
      * @brief Gets the current angle of the constraint.
      * @return (float) The angle in radians.
      */
      SCRIPT_FUNC() float GetHingeAngle() const;

      /**
      * @brief Sets the axis to rotate around relative to point A.
      * @param[in] axsis (const glm::vec3&) The axis.
      */
      SCRIPT_FUNC() void SetAxisA(const glm::vec3& axis);

      /**
      * @brief Sets the axis to rotate around relative to point B.
      * @param[in] axsis (const glm::vec3&) The axis.
      */
      SCRIPT_FUNC() void SetAxisB(const glm::vec3& axis);

      /**
      * @brief Gets the axis to rotate around relative to point A.
      * @return (glm::vec3) The axis.
      */
      SCRIPT_FUNC() glm::vec3 GetAxisA() const;

      /**
      * @brief Gets the axis to rotate around relative to point B.
      * @return (glm::vec3) The axis.
      */
      SCRIPT_FUNC() glm::vec3 GetAxisB() const;

      /**
      * @brief Sets the pivot point relative to point A.
      * @param[in] pivot (const glm::vec3&) The pivot.
      */
      SCRIPT_FUNC() void SetPivotA(const glm::vec3& pivot);

      /**
      * @brief Sets the pivot point relative to point B.
      * @param[in] pivot (const glm::vec3&) The pivot.
      */
      SCRIPT_FUNC() void SetPivotB(const glm::vec3& pivot);

      /**
      * @brief Gets the pivot point from B.
      * @return (glm::vec3) The pivot point.
      */
      SCRIPT_FUNC() glm::vec3 GetPivotB() const;

      /**
      * @brief Gets the pivot point from A
      * @return (glm::vec3) The pivot point
      */
      SCRIPT_FUNC() glm::vec3 GetPivotA() const;

      /**
      * @brief Sets the softness of the constraint.
      * @param[in] softness (float) Softness factor.
      */
      SCRIPT_FUNC() void SetSoftness(float softness);

      /**
      * @brief Gets the softness of the constraint.
      * @return (float) Softness factor.
      */
      SCRIPT_FUNC() float GetSoftness() const;

      /**
      * @brief Sets the bias factor of the constraint.
      * @param[in] bias (float) The bias factor.
      */
      SCRIPT_FUNC() void SetBiasFactor(float bias);

      /**
      * @brief Gets the bias factor of the constraint.
      * @return (float) The bias factor.
      */
      SCRIPT_FUNC() float GetBiasFactor() const;

      /**
      * @brief Sets the relaxation factor of the constraint.
      * @param[in] relaxation (float) The relaxation factor.
      */
      SCRIPT_FUNC() void SetRelaxationFactor(float relaxation);

      /**
      * @brief Gets the relaxation factor of the constraint.
      * @return (float) The relaxation factor.
      */
      SCRIPT_FUNC() float GetRelaxationFactor() const;
    };



    /**
    * @brief An enumerator that is an index of the element.
    * Should be in sync with @see sulphur::engine::LightData::ConstraintData<T>
    */
    enum struct ConstraintComponentElements : size_t
    {
      kConstraint,
      kEntity_a,
      kEntity_b
    };

    /**
    * @struct sulphur::engine::ConstraintData
    * @brief The data contained by constraint components.
    * @author Daniel Konings, Benjamin Waanders
    */
    struct ConstraintData
    {
      /**
      * @brief The component system data of a constraint
      */
      using ComponentSystemData = SystemData<physics::IPhysicsConstraint*, Entity, Entity>;

      /**
      * @brief Default constructor.
      */
      ConstraintData() :
        data(reinterpret_cast<void**>(&constraint))
      {
      }

      physics::IPhysicsConstraint** constraint; //!< Pointer to the constraint.
      Entity* entity_a; //!< Simple direct access to the entity array data.
      Entity* entity_b; //!< Simple direct access to the entity array data.
      ComponentSystemData data; //!< System data of the component.
    };

    /**
    * @class sulphur::engine::ConstraintSystem : public sulphur::engine::IComponentSystem
    * @brief The ComponentSystem that manages all Constraints in the World. 
    * @author Daniel Konings, Benjamin Waanders, Angelo van der Mark
    */
    class ConstraintSystem : public IComponentSystem
    {
    public:

      /**
      * @brief Default constructor.
      */
      ConstraintSystem();

      /**
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Create a new constraint component for this entity and also creates a
      * TransformComponent if it wasn't attached yet.
      * @tparam The constraint type to create.
      * @param[in] entity (sulphur::engine::Entity&) The entity to create this component for.
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity);

      /**
      * @brief Destroys the constraint component.
      * @tparam The constraint type to destroy.
      * @param[in] handle (ComponentT) The component to destroy.
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @param[in] handle (sulphur::engine::ConstraintComponent) The component handle.
      * @return (physics::IPhysicsConstraint*) The constraint retrieved from the handle data.
      */
      physics::IPhysicsConstraint* GetConstraint(ComponentHandleBase handle) const;

      /**
      * @param[in] handle (sulphur::engine::ConstraintComponent) The component handle.
      * @return (sulphur::engine::Entity) The A entity retrieved from the handle data.
      */
      Entity GetEntityA(ConstraintComponent handle) const;

      /**
      * @param[in] handle (sulphur::engine::ConstraintComponent) The component handle.
      * @return (sulphur::engine::Entity) The B entity retrieved from the handle data.
      */
      Entity GetEntityB(ConstraintComponent handle) const;

      /**
      * @brief Adds an entity to the constraint component and encouples it.
      * @param[in] handle (sulphur::engine::ConstraintComponent) The component handle.
      * @param[in] entity (sulphur::engine::Entity) The entity to which the constraint is linked (entity B).
      */
      void AttachEntity(ConstraintComponent handle, Entity entity);

      /**
      * @brief Detaches the secondary entity from the constraint component if it exists.
      * @param[in] handle (sulphur::engine::ConstraintComponent) The component handle.
      */
      void DetachEntity(ConstraintComponent handle);

    private:
      PhysicsSystem* physics_service_; //!< The physics service.
      ConstraintData component_data_; //!< The component data for this system.
    };


    //-------------------------------------------------------------------------
    template<>
    inline FixedConstraintComponent ConstraintSystem::Create(Entity& entity)
    {
      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(entity);

      if (body == nullptr)
      {
        body = physics_service_->CreatePhysicsBody(entity);
      }

      physics::IPhysicsConstraint* constraint =
        physics_service_->CreateConstraint(entity,
          physics::IPhysicsConstraint::ConstraintTypes::kFixed);

      return FixedConstraintComponent(*this,
        this->component_data_.data.Add(
        constraint,
        entity, Entity()));
     }

    //-------------------------------------------------------------------------
    template<>
    inline HingeConstraintComponent ConstraintSystem::Create(Entity& entity)
    {
      physics::PhysicsBody* body = physics_service_->GetPhysicsBody(entity);

      if (body == nullptr)
      {
        body = physics_service_->CreatePhysicsBody(entity);
      }

      physics::IPhysicsConstraint* constraint =
        physics_service_->CreateConstraint(entity,
          physics::IPhysicsConstraint::ConstraintTypes::kHinge);

      return HingeConstraintComponent(*this,
        this->component_data_.data.Add(
          constraint,
          entity, Entity()));
    }
  }
}
