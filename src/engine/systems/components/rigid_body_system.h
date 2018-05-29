#pragma once

#include "engine/core/handle_base.h"
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/scripting/scriptable_object.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

#include <physics/platform_physics.h>


namespace sulphur 
{
  namespace engine
  {
    class RigidBodySystem;
    class PhysicsSystem;
    class RewindStorage;

    /**
    * @brief An enumerator that is an index of the element. 
    * Should be in sync with @see sulphur::engine::LightData::ComponentSystemData
    */
    enum struct RigidBodyComponentElements
    {
      kPhysicsBody, //!< The physics body this component uses.
      kEntity       //!< The owning Entity.
    };
    /**
    * @struct sulphur::engine::RigidBodyData
    * @brief The data used by sulphur::engine::RigidBodyComponent.
    * @author Angelo van der Mark
    */
    class RigidBodyData
    {
    public:
      using ComponentSystemData = SystemData<physics::PhysicsBody*, Entity>; //!< Alias of the system data. The order of the member variables should be in sync with @see sulphur::engine::RigidBodyComponentElements and the pointers in this struct
                  
      /*
      * @brief Constructor of the rigid nody data that initializes the system data by passing it the list of pointers. It uses the first element for this and assumes that the others follow
      */                                                          
      RigidBodyData() :
        data((void**)&physics_body)  
      {
      }

      physics::PhysicsBody** physics_body;//!< Pointer to the physics body instance.
      Entity* entity;//!< Simple direct access to the entity array data.
      ComponentSystemData data;//!< System data of the component.
    };

    /**
    * @class sulphur::engine::RigidBodyComponent : public sulphur::engine::ComponentHandleBase
    * @brief A component which adds dynamic physical properties to the entity.
    * @author Angelo van der Mark
    */
    SCRIPT_CLASS() class RigidBodyComponent : public ComponentHandleBase
    {
    public:

      SCRIPT_NAME(RigidBodyComponent);
      SCRIPT_COMPONENT();

      using System = RigidBodySystem; //!< The associated component system.
      
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      RigidBodyComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::RigidBodyComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      RigidBodyComponent(System& system, size_t handle);

      /**
      * @brief Sets the mass of this rigid body.
      * @remarks The mass cannot be set to less than 0.0001f.
      * @param[in] mass (float) Mass in Kg.
      */
      SCRIPT_FUNC() void SetMass(float mass);

      /**
      * @brief Returns the mass of this rigid body.
      * @return (float) Mass in Kg.
      */
      SCRIPT_FUNC() float GetMass() const;

      /**
      * @brief Returns the inverse mass of this rigid body.
      * @return (float) Inverse mass in Kg.
      */
      SCRIPT_FUNC() float GetInverseMass() const;

      /**
      * @brief Sets the inertia tensor of this rigid body.
      * @param[in] inertia (const glm::vec3&) The inertia tensor.
      */
      SCRIPT_FUNC() void SetInertiaTensor(const glm::vec3& inertia);
      /**
      * @brief Returns the inertia tensor of this rigid body.
      * @return (glm::vec3) The inertia tensor.
      */
      SCRIPT_FUNC() glm::vec3 GetInertiaTensor() const;
      /**
      * @brief Returns the inverse inertia tensor of this rigid body.
      * @return (glm::vec3) The inverse inertia tensor.
      */
      SCRIPT_FUNC() glm::vec3 GetInverseInertiaTensor() const;

      /**
      * @brief Sets the linear velocity of this rigid body.
      * @param[in] velocity (const glm::vec3&) Velocity to set it to.
      */
      SCRIPT_FUNC() void SetLinearVelocity(const glm::vec3& velocity);
      /**
      * @brief Adds a linear velocity to this rigid body.
      * @param[in] velocity (const glm::vec3&) Velocity to add.
      */
      SCRIPT_FUNC() void AddLinearVelocity(const glm::vec3& velocity);
      /**
      * @brief Returns the linear velocity of this rigid body.
      * @return (glm::vec3) The current linear velocity.
      */
      SCRIPT_FUNC() glm::vec3 GetLinearVelocity() const;

      /**
      * @brief Set the angular velocity of this rigid body.
      * @param[in] velocity (const glm::vec3&) Angular velocity to set it to.
      */
      SCRIPT_FUNC() void SetAngularVelocity(const glm::vec3& velocity);
      /**
      * @brief Adds an angular velocity to this rigid body.
      * @param[in] velocity (const glm::vec3&) Angular velocity to add.
      */
      SCRIPT_FUNC() void AddAngularVelocity(const glm::vec3& velocity);
      /**
      * @brief Returns the angular velocity of this rigid body.
      * @return (glm::vec3) The current angular velocity.
      */
      SCRIPT_FUNC() glm::vec3 GetAngularVelocity() const;

      /**
      * @brief Sets the linear damping coefficient for this rigid body.
      * @param[in] damping (float) The damping coefficient.
      */
      SCRIPT_FUNC() void SetLinearDamping(float damping);
      /**
      * @brief Returns the linear damping coefficient of this rigid body.
      * @return (float) The damping coefficient.
      */
      SCRIPT_FUNC() float GetLinearDamping() const;
      /**
      * @brief Sets the angular damping coefficient for this rigid body.
      * @param[in] damping (float) The damping coefficient.
      */
      SCRIPT_FUNC() void SetAngularDamping(float damping);
      /**
      * @brief Returns the linear damping coefficient of this rigid body.
      * @return (float) The damping coefficient.
      */
      SCRIPT_FUNC() float GetAngularDamping() const;

      /**
      * @brief Applies a linear force on the center of mass.
      * @param[in] force (const glm::vec3&) The force to apply.
      */
      SCRIPT_FUNC() void ApplyForce(const glm::vec3& force);
      /**
      * @brief Applies a torque on the rigid body. This will not apply any linear force.
      * @param[in] torque (const glm::vec3&) The torque to apply.
      */
      SCRIPT_FUNC() void ApplyTorque(const glm::vec3& torque);
      /**
      * @brief Applies a force on the rigid body from a position in world space.
      * @param[in] force (const glm::vec3&) The force to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the force is applied.
      */
      SCRIPT_FUNC() void ApplyForceAtPosition(const glm::vec3& force, const glm::vec3& position);

      /**
      * @brief Applies a linear impulse on the center of mass.
      * @param[in] impulse (const glm::vec3&) The linear impulse to apply.
      */
      SCRIPT_FUNC() void ApplyImpulse(const glm::vec3& impulse);
      /**
      * @brief Applies an angular impulse on the rigid body. This will not apply any linear impulse.
      * @param[in] impulse (const glm::vec3&) The angular impulse to apply.
      */
      SCRIPT_FUNC() void ApplyAngularImpulse(const glm::vec3& impulse);
      /**
      * @brief Applies an impulse on the rigid body from a position in world space.
      * @param[in] impulse (const glm::vec3&) Impulse to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the impulse is applied.
      */
      SCRIPT_FUNC() void ApplyImpulseAtPosition(const glm::vec3& impulse, const glm::vec3& position);

      /**
      * @brief Returns whether the rigid body is active in the simulation.
      * @return (bool) True is the rigid body is sleeping.
      */
      SCRIPT_FUNC() bool IsSleeping() const;
      /**
      * @brief Wakes the rigid body if it is sleeping.
      */
      SCRIPT_FUNC() void ForceWake();
      /**
      * @brief Makes the rigid body sleep if it is active.
      */
      SCRIPT_FUNC() void ForceSleep();

      /**
      * @brief Sets the rigid body state to kinematic or dynamic.
      * @param[in] kinematic (bool) True to set this body to kinematic, false to set this body to dynamic.
      */
      SCRIPT_FUNC() void SetKinematic(bool kinematic);

      /**
      * @brief Returns whether this body is dynamic or kinematic.
      * @return (bool) True if kinematic.
      */
      SCRIPT_FUNC() bool IsKinematic() const;

      /**
      * @brief Allows a total restriction of movement on the given axes.
      * @note This only applies to physics-based movement. Directly moving the body is unaffected.
      * @param[in] lock_axes (const glm::bvec3&) The axes to lock. True means the axis is locked.
      */
      SCRIPT_FUNC() void LockTranslation(const glm::vec3& lock_axes);

      /**
      * @brief Sets or unsets a total restriction of movement on the X axis.
      * @note This only applies to physics-based movement. Directly moving the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock movement on this axis.
      */
      SCRIPT_FUNC() void LockTranslationX(bool lock_axis);
      /**
      * @brief Sets or unsets a total restriction of movement on the Y axis.
      * @note This only applies to physics-based movement. Directly moving the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock movement on this axis.
      */
      SCRIPT_FUNC() void LockTranslationY(bool lock_axis);
      /**
      * @brief Sets or unsets a total restriction of movement on the Z axis.
      * @note This only applies to physics-based movement. Directly moving the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock movement on this axis.
      */
      SCRIPT_FUNC() void LockTranslationZ(bool lock_axis);

      /**
      * @brief Returns what axes this body is locked to, restricting movement.
      * @return (glm::bvec3) Three booleans corresponding to each axis. True means translation is locked.
      */
      SCRIPT_FUNC() glm::vec3 GetTranslationLock() const;

      /**
      * @brief Allows a total restriction of rotation on the given axes.
      * @note This only applies to physics-based movement. Directly rotating the body is unaffected.
      * @param[in] lock_axes (const glm::bvec3&) The axes to lock. True means the axis is locked.
      */
      SCRIPT_FUNC() void LockRotation(const glm::vec3& lock_axes);

      /**
      * @brief Sets or unsets a total restriction of rotation on the X axis.
      * @note This only applies to physics-based movement. Directly rotation the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock rotation on this axis.
      */
      SCRIPT_FUNC() void LockRotationX(bool lock_axis);
      /**
      * @brief Sets or unsets a total restriction of rotation on the Y axis.
      * @note This only applies to physics-based movement. Directly rotation the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock rotation on this axis.
      */
      SCRIPT_FUNC() void LockRotationY(bool lock_axis);
      /**
      * @brief Sets or unsets a total restriction of rotation on the Z axis.
      * @note This only applies to physics-based movement. Directly rotation the body is unaffected.
      * @param[in] lock_axis (bool) Whether to lock or unlock rotation on this axis.
      */
      SCRIPT_FUNC() void LockRotationZ(bool lock_axis);

      /**
      * @brief Returns what axes this body is locked to, restricting rotation.
      * @return (glm::bvec3) Three booleans corresponding to each axis. True means rotation is locked.
      */
      SCRIPT_FUNC() glm::vec3 GetRotationLock() const;

    private:
      RigidBodySystem* system_;
      
    };


    /**
    * @class sulphur::engine::RigidBodySystem : public sulphur::engine::IComponentSystem<RigidBodyComponent>
    * @brief Manages the lifecycle and all instances of sulphur::engine::RigidBodyComponent.
    * @author Angelo van der Mark
    */
    class RigidBodySystem : public IComponentSystem
    {
    public:
      /**
      * @brief Default constructor
      */
      RigidBodySystem();

      /**
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      RigidBodyComponent Create(Entity& entity);

      /**
      * @see sulphur::engine::RigidBodyComponent::SetMass
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] mass (float) Mass in Kg.
      */
      void SetMass(RigidBodyComponent handle, float mass);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetMass
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (float) Mass in Kg.
      */
      float GetMass(RigidBodyComponent handle) const;
      /**
      * @see sulphur::engine::RigidBodyComponent::GetInverseMass
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (float) Inverse mass in Kg.
      */
      float GetInverseMass(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::SetInertiaTensor
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] inertia (const glm::vec3&) The inertia tensor.
      */
      void SetInertiaTensor(RigidBodyComponent handle, const glm::vec3& inertia);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetInertiaTensor
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (glm::vec3) The inertia tensor.
      */
      glm::vec3 GetInertiaTensor(RigidBodyComponent handle) const;
      /**
      * @see sulphur::engine::RigidBodyComponent::GetInverseInertiaTensor
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (glm::vec3) The inverse inertia tensor.
      */
      glm::vec3 GetInverseInertiaTensor(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::SetLinearVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] velocity (const glm::vec3&) Velocity to set it to.
      */
      void SetLinearVelocity(RigidBodyComponent handle, const glm::vec3& velocity);
      /**
      * @see sulphur::engine::RigidBodyComponent::AddLinearVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] velocity (const glm::vec3&) Velocity to add.
      */
      void AddLinearVelocity(RigidBodyComponent handle, const glm::vec3& velocity);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetLinearVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (glm::vec3) The current linear velocity.
      */
      glm::vec3 GetLinearVelocity(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::SetAngularVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] velocity (const glm::vec3&) Angular velocity to set it to.
      */
      void SetAngularVelocity(RigidBodyComponent handle, const glm::vec3& velocity);
      /**
      * @see sulphur::engine::RigidBodyComponent::AddAngularVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] velocity (const glm::vec3&) Angular velocity to add.
      */
      void AddAngularVelocity(RigidBodyComponent handle, const glm::vec3& velocity);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetAngularVelocity
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (glm::vec3) The current angular velocity.
      */
      glm::vec3 GetAngularVelocity(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::SetLinearDamping
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] damping (float) The damping coefficient.
      */
      void SetLinearDamping(RigidBodyComponent handle, float damping);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetLinearDamping
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (float) The damping coefficient.
      */
      float GetLinearDamping(RigidBodyComponent handle) const;
      /**
      * @see sulphur::engine::RigidBodyComponent::SetAngularDamping
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] damping (float) The damping coefficient.
      */
      void SetAngularDamping(RigidBodyComponent handle, float damping);
      /**
      * @see sulphur::engine::RigidBodyComponent::GetAngularDamping
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (float) The damping coefficient.
      */
      float GetAngularDamping(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyForce
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] force (const glm::vec3&) The force to apply.
      */
      void ApplyForce(RigidBodyComponent handle, const glm::vec3& force);
      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyTorque
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] torque (const glm::vec3&) The torque to apply.
      */
      void ApplyTorque(RigidBodyComponent handle, const glm::vec3& torque);
      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyForceAtPosition
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] force (const glm::vec3&) The force to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the force is applied.
      */
      void ApplyForceAtPosition(RigidBodyComponent handle, const glm::vec3& force, const glm::vec3& position);

      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyImpulse
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] impulse (const glm::vec3&) The linear impulse to apply.
      */
      void ApplyImpulse(RigidBodyComponent handle, const glm::vec3& impulse);
      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyAngularImpulse
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] impulse (const glm::vec3&) The angular impulse to apply.
      */
      void ApplyAngularImpulse(RigidBodyComponent handle, const glm::vec3& impulse);
      /**
      * @see sulphur::engine::RigidBodyComponent::ApplyImpulseAtPosition
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] impulse (const glm::vec3&) Impulse to apply.
      * @param[in] position (const glm::vec3&) Position in world space where the impulse is applied.
      */
      void ApplyImpulseAtPosition(RigidBodyComponent handle, const glm::vec3& impulse, const glm::vec3& position);

      /**
      * @see sulphur::engine::RigidBodyComponent::IsSleeping
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (bool) True is the rigid body is sleeping.
      */
      bool IsSleeping(RigidBodyComponent handle) const;
      /**
      * @see sulphur::engine::RigidBodyComponent::ForceWake
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void ForceWake(RigidBodyComponent handle);
      /**
      * @see sulphur::engine::RigidBodyComponent::ForceSleep
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void ForceSleep(RigidBodyComponent handle);

      /**
      * @see sulphur::engine::RigidBodyComponent::SetKinematic
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @param[in] kinematic (bool) True to set this body to kinematic, false to set this body to dynamic.
      */
      void SetKinematic(RigidBodyComponent handle, bool kinematic);

      /**
      * @see sulphur::engine::RigidBodyComponent::IsKinematic
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      * @return (bool) True if kinematic.
      */
      bool IsKinematic(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::LockTranslation
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockTranslation(RigidBodyComponent handle, const glm::vec3& lock_axes);

      /**
      * @see sulphur::engine::RigidBodyComponent::LockTranslationX
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockTranslationX(RigidBodyComponent handle, bool lock_axis);
      /**
      * @see sulphur::engine::RigidBodyComponent::LockTranslationY
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockTranslationY(RigidBodyComponent handle, bool lock_axis);
      /**
      * @see sulphur::engine::RigidBodyComponent::LockTranslationZ
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockTranslationZ(RigidBodyComponent handle, bool lock_axis);

      /**
      * @see sulphur::engine::RigidBodyComponent::GetTranslationLock
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      glm::bvec3 GetTranslationLock(RigidBodyComponent handle) const;

      /**
      * @see sulphur::engine::RigidBodyComponent::LockRotation
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockRotation(RigidBodyComponent handle, const glm::bvec3& lock_axes);

      /**
      * @see sulphur::engine::RigidBodyComponent::LockRotationX
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockRotationX(RigidBodyComponent handle, bool lock_axis);
      /**
      * @see sulphur::engine::RigidBodyComponent::LockRotationY
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockRotationY(RigidBodyComponent handle, bool lock_axis);
      /**
      * @see sulphur::engine::RigidBodyComponent::LockRotationZ
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      void LockRotationZ(RigidBodyComponent handle, bool lock_axis);

      /**
      * @see sulphur::engine::RigidBodyComponent::GetRotationLock
      * @param[in] handle (sulphur::engine::RigidBodyComponent) The component to call the function on.
      */
      glm::bvec3 GetRotationLock(RigidBodyComponent handle) const;

      /**
      * @brief Returns the Entity from a given component.
      * @param[in] handle (sulphur::engine::ComponentHandleBase) The component the Entity is requested from.
      * @return (sulphur::engine::Entity&) The owning Entity.
      */
      Entity& GetEntity(ComponentHandleBase handle) const;

    private:
      PhysicsSystem* physics_service_; //!< The physics service system this system is using.
      RewindStorage* rewind_storage_;//!< Rewind storage for the physics system

      RigidBodyData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}