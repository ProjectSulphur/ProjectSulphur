#pragma once

#include "engine/application/application.h"
#include "engine/core/world.h"
#include "engine/systems/components/transform_system.h"
#include "engine/physics/physics_system.h"
#include "engine/assets/physics_material.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/mesh.h"

namespace sulphur 
{

  namespace physics
  {
    class PhysicsCollider;
  }

  namespace engine 
  {
    class ColliderSystem;
    
    /**
    * @class sulphur::engine::ColliderComponent
    * @brief The base collider component.
    * @author Daniel Konings, Angelo van der Mark, Benjamin Waanders
    */
    class ColliderComponent : public ComponentHandleBase
    {

    public:

      using System = ColliderSystem; //!< The associated system.

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      ColliderComponent();
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      ColliderComponent(System& system, size_t handle);
    
      /**
      * @brief Applies the provided physics material to this collider.
      * @param[in] material (const sulphur::engine::PhysicsMaterialHandle&) The material to apply.
      */
      void ApplyMaterial(const PhysicsMaterialHandle& material);

      /**
      * @brief Set the collision shape of this collider directly. This allows for shape reuse.
      * @remarks The collision shape must be of the same type as the collider.
      * @param[in] shape (sulphur::physics::PhysicsShape*) The shape to use for this collider.
      */
      void SetShape(physics::PhysicsShape* shape);
      
      /**
      * @brief Returns the collision shape of this collider.
      * @return (sulphur::physics::PhysicsShape*) The pointer to the used collision shape.
      */
      physics::PhysicsShape* GetShape() const;
      
      /** 
      * @brief Sets the local translation for this collider.
      * @param[in] translation (const glm::vec3&) The local translation for this collider.
      */
      SCRIPT_FUNC_EX() void SetOffset(const glm::vec3& translation);

      /**
      * @brief Returns the local translation of this collider.
      * @return (glm::vec3) A copy of the local translation of this collider.
      */
      SCRIPT_FUNC_EX() glm::vec3 GetOffset() const;

      /**
      * @brief Sets the local rotation for this collider.
      * @param[in] rotation (const glm::quat&) The local rotation for this collider.
      */
      SCRIPT_FUNC_EX() void SetLocalRotation(const glm::quat& rotation);

      /**
      * @brief Returns the local rotation of this collider.
      * @return (glm::quat) A copy of the local rotation of this collider.
      */
      SCRIPT_FUNC_EX() glm::quat GetLocalRotation() const;

      /**
      * @brief Sets the static friction coefficient of this collider.
      * @param[in] friction (float) The static friction coefficient.
      */
      SCRIPT_FUNC_EX() void SetStaticFriction(float friction);

      /**
      * @brief Returns the static friction coefficient of this collider.
      * @return (float) The dynamic static coefficient.
      */
      SCRIPT_FUNC_EX() float GetStaticFriction() const;

      /**
      * @brief Sets the dynamic friction coefficient of this collider.
      * @param[in] friction (float) The dynamic friction coefficient.
      */
      SCRIPT_FUNC_EX() void SetDynamicFriction(float friction);

      /**
      * @brief Returns the dynamic friction coefficient of this collider.
      * @return (float) The dynamic friction coefficient.
      */
      SCRIPT_FUNC_EX() float GetDynamicFriction() const;

      /**
      * @brief Sets the rolling friction coefficient of this collider.
      * @param[in] friction (float) The rolling friction coefficient.
      */
      SCRIPT_FUNC_EX() void SetRollingFriction(float friction);

      /**
      * @brief Returns the rolling friction coefficient of this collider.
      * @return (float) The rolling friction coefficient.
      */
      SCRIPT_FUNC_EX() float GetRollingFriction() const;

      /**
      * @brief Sets the friction combine mode to use.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode to use.
      */
      void SetFrictionCombineMode(physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Returns the friction combine mode to use.
      * @return (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode in use.
      */
      physics::PhysicsCollider::MaterialCombineMode GetFrictionCombineMode() const;

      /**
      * @brief Sets the bounciness of this collider.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @param[in] restitution (float) The restitution.
      */
      SCRIPT_FUNC_EX() void SetRestitution(float restitution);

      /**
      * @brief Returns the bounciness of this collider.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @return (float) The restitution.
      */
      SCRIPT_FUNC_EX() float GetRestitution() const;

      /**
      * @brief Sets the restitution combine mode to use.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode to use.
      */
      void SetRestitutionCombineMode(physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Returns the restitution combine mode to use.
      * @return (sulphur::physics::PhysicsCollider::MaterialCombineMode) The combine mode in use.
      */
      physics::PhysicsCollider::MaterialCombineMode GetRestitutionCombineMode() const;

      
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddCollisionEnterCallback(ScriptableCallback callback);

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddCollisionStayCallback(ScriptableCallback callback);

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddCollisionLeaveCallback(ScriptableCallback callback);

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddOverlapEnterCallback(ScriptableCallback callback);

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddOverlapStayCallback(ScriptableCallback callback);

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void AddOverlapLeaveCallback(ScriptableCallback callback);


      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveCollisionEnterCallback(ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveCollisionStayCallback(ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveCollisionLeaveCallback(ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveOverlapEnterCallback(ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveOverlapStayCallback(ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      SCRIPT_FUNC_EX() void RemoveOverlapLeaveCallback(ScriptableCallback callback);
      
    protected:
      System* system_; //!< Pointer to this collider component's system.
    };

    /**
    * @class sulphur::engine::BoxColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach box collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class BoxColliderComponent : public ColliderComponent
    {

    public:

      SCRIPT_NAME(BoxColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      BoxColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      BoxColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the extents from the center of this box collider component.
      * @param[in] extents (const glm::vec3&) The extents.
      */
      SCRIPT_FUNC() void SetExtents(const glm::vec3& extents);

      /**
      * @return (glm::vec3) The extents from the center of this box collider component.
      */
      SCRIPT_FUNC() glm::vec3 Extents() const;
    };

  
    /**
    * @class sulphur::engine::SphereColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach sphere collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class SphereColliderComponent : public ColliderComponent
    {
    public:

      SCRIPT_NAME(SphereColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();
      
      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      SphereColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      SphereColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component.
      * @param[in] radius (float) The radius to set.
      */
      SCRIPT_FUNC() void SetRadius(float radius);

      /**
      * @return (float) The radius from the center of this collider component.
      */
      SCRIPT_FUNC() float GetRadius() const;
    };


    /**
    * @class sulphur::engine::CylinderColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach cylinder collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class CylinderColliderComponent : public ColliderComponent
    {
    public:

      SCRIPT_NAME(CylinderColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      CylinderColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      CylinderColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component.
      * @param[in] radius (float) The radius to set.
      */
      SCRIPT_FUNC() void SetRadius(float radius);

      /**
      * @return (float) The radius from the center of this collider component.
      */
      SCRIPT_FUNC() float GetRadius() const;

      /**
      * @brief Sets the total height of this collider component.
      * @param[in] height (float) The height to set.
      */
      SCRIPT_FUNC() void SetHeight(float height);

      /**
      * @brief Returns the total height of this collider component.
      * @return (float) The height of the collider component.
      */
      SCRIPT_FUNC() float GetHeight() const;
    };

    
    /**
    * @class sulphur::engine::CapsuleColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach capsule collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class CapsuleColliderComponent : public ColliderComponent
    {
    public:

      SCRIPT_NAME(CapsuleColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      CapsuleColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      CapsuleColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component.
      * @param[in] radius (float) The radius to set.
      */
      SCRIPT_FUNC() void SetRadius(float radius);

      /**
      * @return (float) The radius from the center of this collider component.
      */
      SCRIPT_FUNC() float GetRadius() const;

      /**
      * @brief Sets the total height of this collider component.
      * @param[in] height (float) The height to set.
      */
      SCRIPT_FUNC() void SetHeight(float height);

      /**
      * @brief Returns the total height of this collider component.
      * @return (float) The height of the collider component.
      */
      SCRIPT_FUNC() float GetHeight() const;
    };

    
    /**
    * @class sulphur::engine::ConeColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach cone collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class ConeColliderComponent : public ColliderComponent
    {
    public:

      SCRIPT_NAME(ConeColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      ConeColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      ConeColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component.
      * @param[in] radius (float) The radius to set.
      */
      SCRIPT_FUNC() void SetRadius(float radius);

      /**
      * @return (float) The radius from the center of this collider component.
      */
      SCRIPT_FUNC() float GetRadius() const;

      /**
      * @brief Sets the total height of this collider component.
      * @param[in] height (float) The height to set.
      */
      SCRIPT_FUNC() void SetHeight(float height);

      /**
      * @brief Returns the total height of this collider component.
      * @return (float) The height of the collider component.
      */
      SCRIPT_FUNC() float GetHeight() const;
    };
  
  
  /**
    * @class sulphur::engine::ConvexColliderComponent : public sulphur::engine::ColliderComponent
    * @brief Used to attach convex collider components to the entities to send over to the physics layer.
    * @author Daniel Konings, Angelo van der Mark
    */
    SCRIPT_CLASS() class ConvexColliderComponent : public ColliderComponent
    {
    public:

      SCRIPT_NAME(ConvexColliderComponent);
      SCRIPT_COMPONENT();
      SCRIPT_EXPAND();

      /**
      * @brief Default constructor (creates an empty/invalid handle).
      */
      ConvexColliderComponent() {};
      /**
      * @brief Constructor for creating a handle from an integral value.
      * @param[in] system (sulphur::engine::ColliderComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      ConvexColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the mesh to base this collider on.
      * @param[in] mesh (sulphur::engine::MeshHandle) The handle to the mesh.
      */
      void SetMesh(MeshHandle mesh);

      /**
      * @brief Returns the mesh this collider is using.
      * @return (sulphur::engine::MeshHandle) The handle to the mesh used by this collider.
      */
      MeshHandle GetMesh() const;
    };


    /**
    * @brief An enumerator that is an index of the element. 
    * Should be in sync with @see sulphur::engine::LightData::ColliderData<T>
    */
    enum struct ColliderComponentElements : size_t
    {
      kCollider, //!< The collider instance the component owns.
      kMesh, //!< The mesh handle for mesh colliders.
      kEntity //!< The owning Entity.
    };

    /**
    * @struct sulphur::engine::ColliderData
    * @brief The data contained by base collider components.
    * @author Daniel Konings, Angelo van der Mark
    */
    struct ColliderData
    {
      /**
      * @brief The component system data of a collider.
      */
      using ComponentSystemData = SystemData<physics::PhysicsCollider*, MeshHandle, Entity>; 

      /**
      * @brief Default constructor.
      */
      ColliderData() :
        data((void**)(&collider))
      {
      }

      physics::PhysicsCollider** collider; //!< Pointer to the physics collider.
      MeshHandle* mesh; //!< Optional mesh handle for mesh colliders.
      Entity* entity; //!< Simple direct access to the entity array data.
      ComponentSystemData data; //!< System data of the component.
    };


    /**
    * @class sulphur::engine::ColliderSystem : public sulphur::engine::IComponentSystem
    * @brief The collider system that manages all collider component types.
    * @author Daniel Konings, Angelo van der Mark
    */
    class ColliderSystem : public IComponentSystem
    {
    public:

      /**
      * @brief Default constructor.
      */
      ColliderSystem();

      /**
      * @see sulphur::engine::IComponentSystem::OnInitialize
      * @internal
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::IComponentSystem::OnTerminate
      * @internal
      */
      void OnTerminate() override;

      /**
      * @brief Create a new collider component for this entity.
      * @tparam The collider type to create.
      * @remarks Attaches a transform component if it wasn't added yet.
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for.
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity);

      /**
      * @brief Destroys the collider component.
      * @tparam The collider type to destroy.
      * @param[in] handle (ComponentT) The component to destroy.
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Retrieve the entity from the component by data index.
      * @param[in] handle (sulphur::engine::ColliderComponent) The handle to retrieve the entity of.
      * @return (sulphur::engine::Entity) The retrieved entity.
      */
      Entity GetEntity(ColliderComponent handle) const;
      
      /**
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle to retrieve the collider from.
      * @return (sulphur::physics::PhysicsCollider*) The collider retrieved from the handle data.
      */
      physics::PhysicsCollider* GetCollider(ComponentHandleBase handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetShape
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetShape(ColliderComponent handle, physics::PhysicsShape* shape);

      /**
      * @see sulphur::engine::ColliderComponent::GetShape
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      physics::PhysicsShape* GetShape(ColliderComponent handle) const;

      /**
      * @brief Returns the mesh handle associated with this collider.
      * @remarks For use by non-primitive colliders only. Returns an invalid handle otherwise.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @return (sulphur::engine::MeshHandle) The mesh used by the collider.
      */
      MeshHandle GetMesh(ColliderComponent handle) const;

      /**
      * @brief Sets this collider's collision shape to this mesh.
      * @remarks For use by non-primitive colliders only.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] mesh (sulphur::engine::MeshHandle) The handle to the mesh.
      */
      void SetMesh(ColliderComponent handle, MeshHandle mesh);

      /**
      * @see sulphur::engine::ColliderComponent::SetOffset
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetOffset(ColliderComponent handle, const glm::vec3& translation);

      /**
      * @see sulphur::engine::ColliderComponent::GetOffset
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      const glm::vec3& GetOffset(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetLocalRotation
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetLocalRotation(ColliderComponent handle, const glm::quat& rotation);

      /**
      * @see sulphur::engine::ColliderComponent::GetLocalRotation
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      const glm::quat& GetLocalRotation(ColliderComponent handle) const;
      
      /**
      * @see sulphur::engine::ColliderComponent::ApplyMaterial
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void ApplyMaterial(ColliderComponent handle, const PhysicsMaterialHandle& material);

      /**
      * @see sulphur::engine::ColliderComponent::SetStaticFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetStaticFriction(ColliderComponent handle, float friction);

      /**
      * @see sulphur::engine::ColliderComponent::GetStaticFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      float GetStaticFriction(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetDynamicFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetDynamicFriction(ColliderComponent handle, float friction);

      /**
      * @see sulphur::engine::ColliderComponent::GetDynamicFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      float GetDynamicFriction(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetRollingFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetRollingFriction(ColliderComponent handle, float friction);

      /**
      * @see sulphur::engine::ColliderComponent::GetRollingFriction
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      float GetRollingFriction(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetFrictionCombineMode
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetFrictionCombineMode(ColliderComponent handle,
        physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @see sulphur::engine::ColliderComponent::GetFrictionCombineMode
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      physics::PhysicsCollider::MaterialCombineMode
        GetFrictionCombineMode(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetRestitution
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetRestitution(ColliderComponent handle, float restitution);

      /**
      * @see sulphur::engine::ColliderComponent::GetRestitution
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      float GetRestitution(ColliderComponent handle) const;

      /**
      * @see sulphur::engine::ColliderComponent::SetRestitutionCombineMode
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      void SetRestitutionCombineMode(ColliderComponent handle,
        physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @see sulphur::engine::ColliderComponent::GetRestitutionCombineMode
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      */
      physics::PhysicsCollider::MaterialCombineMode
        GetRestitutionCombineMode(ColliderComponent handle) const;

      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddCollisionEnterCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddCollisionStayCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddCollisionLeaveCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddOverlapEnterCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddOverlapStayCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Adds a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void AddOverlapLeaveCallback(ColliderComponent handle, ScriptableCallback callback);

      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveCollisionEnterCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveCollisionStayCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveCollisionLeaveCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveOverlapEnterCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveOverlapStayCallback(ColliderComponent handle, ScriptableCallback callback);
      /**
      * @brief Removes a callback to a specific function.
      * @param[in] handle (sulphur::engine::ColliderComponent) The component handle.
      * @param[in] callback (sulphur::engine::ScriptableCallback) The callback to be added.
      */
      void RemoveOverlapLeaveCallback(ColliderComponent handle, ScriptableCallback callback);

    protected:
      PhysicsSystem* physics_service_; //!< The physics service system.

    private:
      /**
      * @brief Creates a physics body for the given entity if it has none.
      * @internal
      * @param[in] entity (sulphur::engine::Entity&) The entity to create a physics body for.
      */
      void CreatePhysicsBody(Entity& entity);

      ColliderData component_data_; //!< An instance of the container that stores per-component data.
    };
    
    //-------------------------------------------------------------------------
    template<>
    inline BoxColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      physics::PhysicsCollider* ptr = physics_service_->CreatePrimitiveCollider(
        entity, physics::PhysicsShape::ShapeTypes::kBox);

      return BoxColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }

    //-------------------------------------------------------------------------
    template<>
    inline SphereColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      physics::PhysicsCollider* ptr = physics_service_->CreatePrimitiveCollider(
        entity, physics::PhysicsShape::ShapeTypes::kSphere);

      return SphereColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }

    //-------------------------------------------------------------------------
    template<>
    inline ConeColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      physics::PhysicsCollider* ptr = physics_service_->CreatePrimitiveCollider(
        entity, physics::PhysicsShape::ShapeTypes::kCone);

      return ConeColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }

    //-------------------------------------------------------------------------
    template<>
    inline CapsuleColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      physics::PhysicsCollider* ptr = physics_service_->CreatePrimitiveCollider(
        entity, physics::PhysicsShape::ShapeTypes::kCapsule);

      return CapsuleColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }

    //-------------------------------------------------------------------------
    template<>
    inline CylinderColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      physics::PhysicsCollider* ptr = physics_service_->CreatePrimitiveCollider(
        entity, physics::PhysicsShape::ShapeTypes::kCylinder);

      return CylinderColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }

    //-------------------------------------------------------------------------
    template<>
    inline ConvexColliderComponent ColliderSystem::Create(Entity& entity)
    {
      CreatePhysicsBody(entity);

      // Uses fallback convex collider as placeholder for creation
      physics::PhysicsCollider* ptr = physics_service_->CreateMeshCollider(
        entity, MeshHandle(), true);

      return ConvexColliderComponent(*this, component_data_.data.Add(
        ptr,
        MeshHandle(),
        entity));
    }
  }
}
