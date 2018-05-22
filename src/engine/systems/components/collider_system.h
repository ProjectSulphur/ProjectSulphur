#pragma once

#include "engine/application/application.h"
#include "engine/core/world.h"
#include "engine/systems/components/transform_system.h"
#include "engine/physics/physics_system.h"

#include <physics/platform_physics_shape.h>

namespace sulphur 
{
  namespace engine 
  {
    template <typename T>
    class ColliderComponentBase;
    template <typename T>
    class ColliderSystemBase;

    /**
    * @brief An enumerator that is an index of the element. 
    * Should be in sync with @see sulphur::engine::LightData::ColliderData<T>
    */
    enum struct ColliderComponentElements : size_t
    {
      kShape, //!< The shape the component owns.
      kEntity //!< The owning Entity.
    };

    /**
    * @struct sulphur::engine::ColliderData <T>
    * @brief The data contained by base collider components
    * @author Daniel Konings
    */
    template <typename T>
    struct ColliderData
    {
      /**
      * @brief The component system data of a collider
      */
      using ComponentSystemData = SystemData<foundation::SharedPointer<T>, Entity>; 

      /**
      * @brief Default constructor.
      */
      ColliderData() :
        data((void**)(&shape))
      {
      }

      foundation::SharedPointer<T>* shape; //!< Pointer to the physics shape
      Entity* entity; //!< Simple direct access to the entity array data
      ComponentSystemData data; //!< System data of the component
    };

    /**
    * @class sulphur::engine::ColliderComponentBase : public sulphur::engine::ComponentHandleBase
    * @brief Wraps the base functionality of a collider for the other collider components to use
    * @author Daniel Konings
    */
    template <typename T>
    class ColliderComponentBase : public ComponentHandleBase
    {
    public:
      using BaseSystem = ColliderSystemBase<T>; //!< The collider system to use.

      /**
      * @brief Default constructor (creates and empty/invalid handle)
      */
      ColliderComponentBase();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::ColliderComponentBase::BaseSystem&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      ColliderComponentBase(BaseSystem& system, size_t handle);

    private:
      BaseSystem* base_system_; //!< Pointer to this collider component's system.

    };

    /**
    * @class sulphur::engine::ColliderSystemBase : public sulphur::engine::IComponentSystem<T, sulphur::engine::ColliderData<T::type>>
    * @tparam The collider type to use
    * @brief Used to wrap default functionalities of the multiple existing collider systems
    * @author Daniel Konings
    */
    template <typename T>
    class ColliderSystemBase : public IComponentSystem<T, ColliderData<typename T::type>>
    {
    public:
      /**
      * @brief Default constructor
      * @param[in] name (const char*)
      *            The resource name for the system
      */
      ColliderSystemBase(const char* name);

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
      * @brief Create a new collider component for this entity
      * @remarks Attaches a transform component if it wasn't added yet
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for
      * @see sulphur::engine::IComponentSystem::Create
      */
      T Create(Entity entity) override;

      /**
      * @brief Destroys the collider component
      * @param[in] handle (T) The component to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(T handle) override;

      /**
      * @param[in] handle (T) The component handle to retrieve the shape from
      * @return (T::type) The shape retrieved from the handle data
      */
      typename T::type* GetShape(T handle) const;

    protected:
      PhysicsSystem* physics_service_; //!< The physics service

    };

    class BoxColliderSystem;

    /**
    * @class sulphur::engine::BoxColliderComponent : public sulphur::engine::ColliderComponentBase
    * @brief Used to attach box collider components to the entities to send over to the physics layer
    * @author Daniel Konings
    */
    class BoxColliderComponent : public ColliderComponentBase<BoxColliderComponent>
    {

    public:
      using System = BoxColliderSystem; //!< The associated system
      
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      BoxColliderComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::BoxColliderComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      BoxColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the extents from the center of this box collider component
      * @param[in] e (const glm::vec3&) The extents
      */
      void SetExtents(const glm::vec3& e);

      /**
      * @return (glm::vec3) The extents from the center of this box collider component
      */
      glm::vec3 Extents() const;

      using type = physics::PhysicsBoxShape; //!< Used to construct the appropriate physics shape in the collider system base

    private:
      System* system_; //!< Pointer to this collider component's system.

    };

    /**
    * @class sulphur::engine::BoxColliderSystem : public sulphur::engine::ColliderSystemBase<sulphur::engine::BoxColliderComponent>>
    * @brief Used to manage all the box collider components owned by the engine's entities
    * @author Daniel Konings
    */
    class BoxColliderSystem : public ColliderSystemBase<BoxColliderComponent>
    {

    public:
      /**
      * @brief Default constructor
      */
      BoxColliderSystem() :
        ColliderSystemBase<BoxColliderComponent>("BoxColliderSystem")
      {
      }

      /**
      * @brief Sets the extents of a specific box collider component by handle
      * @param[in] handle (sulphur::engine::BoxColliderComponent) The component handle
      * @param[in] e (const glm::vec3&) The new extents
      */
      void SetExtents(BoxColliderComponent handle, const glm::vec3& e);

      /**
      * @brief Retrieves the extents of a specific box collider component by handle
      * @param[in] handle (sulphur::engine::BoxColliderComponent) The component handle
      * @return (glm::vec3) The extents of the box collider component
      */
      glm::vec3 Extents(BoxColliderComponent handle) const;
      
    };

    class SphereColliderSystem;

    /**
    * @class sulphur::engine::SphereColliderComponent : public sulphur::engine::ColliderComponentBase
    * @brief Used to attach sphere collider components to the entities to send over to the physics layer
    * @author Daniel Konings
    */
    class SphereColliderComponent : public ColliderComponentBase<SphereColliderComponent>
    {
    public:
      using System = SphereColliderSystem; //!< The associated system
      
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      SphereColliderComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::SphereColliderComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      SphereColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component
      * @param[in] r (float) The radius to set
      */
      void SetRadius(float r);

      /**
      * @return (float) The radius from the center of this collider component
      */
      float Radius() const;

      using type = physics::PhysicsSphereShape; //!< Used to construct the appropriate physics shape in the collider system base

    private:
      System* system_; //!< Pointer to this collider component's system.
    };

    /**
    * @class sulphur::engine::SphereColliderSystem : public sulphur::engine::ColliderSystemBase<sulphur::engine::SphereColliderComponent>>
    * @brief Used to manage all the sphere collider components owned by the engine's entities
    * @author Daniel Konings
    */
    class SphereColliderSystem : public ColliderSystemBase<SphereColliderComponent>
    {

    public:
      /**
      * @brief Default constructor
      */
      SphereColliderSystem() :
        ColliderSystemBase<SphereColliderComponent>("SphereColliderSystem")
      {
      }

      /**
      * @brief Sets the radius of a specific sphere collider component by handle
      * @param[in] handle (sulphur::engine::SphereColliderComponent) The component handle
      * @param[in] r (float) The new radius
      */
      void SetRadius(SphereColliderComponent handle, float r);

      /**
      * @brief Retrieves the radius of a specific sphere collider component by handle
      * @param[in] handle (sulphur::engine::SphereColliderComponent) The component handle
      * @return (float) The radius of the sphere collider component
      */
      float Radius(SphereColliderComponent handle) const;

    };

    class CapsuleColliderSystem;

    /**
    * @class sulphur::engine::CapsuleColliderComponent : public sulphur::engine::ColliderComponentBase
    * @brief Used to attach capsule collider components to the entities to send over to the physics layer
    * @author Daniel Konings, Angelo van der Mark
    */
    class CapsuleColliderComponent : public ColliderComponentBase<CapsuleColliderComponent>
    {
    public:
      using System = CapsuleColliderSystem; //!< The associated system

      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      CapsuleColliderComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::CapsuleColliderComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      CapsuleColliderComponent(System& system, size_t handle);

      /**
      * @brief Sets the radius from the center of this collider component.
      * @param[in] r (float) The radius to set.
      */
      void SetRadius(float radius);

      /**
      * @return (float) The radius from the center of this collider component.
      */
      float GetRadius() const;

      /**
      * @brief Sets the total height of this collider component.
      * @param[in] height (float) The height to set.
      */
      void SetHeight(float height);

      /**
      * @brief Returns the total height of this collider component.
      * @return (float) The height of the collider component.
      */
      float GetHeight() const;

      using type = physics::PhysicsCapsuleShape; //!< Used to construct the appropriate physics shape in the collider system base

    private:
      System* system_; //!< Pointer to this collider component's system.
    };

    /**
    * @class sulphur::engine::CapsuleColliderSystem : public sulphur::engine::ColliderSystemBase<sulphur::engine::CapsuleColliderComponent>>
    * @brief Used to manage all the capsule collider components owned by the engine's entities.
    * @author Daniel Konings, Angelo van der Mark
    */
    class CapsuleColliderSystem : public ColliderSystemBase<CapsuleColliderComponent>
    {

    public:
      /**
      * @brief Default constructor.
      */
      CapsuleColliderSystem() :
        ColliderSystemBase<CapsuleColliderComponent>("CapsuleColliderSystem")
      {
      }

      /**
      * @brief Sets the radius of a specific capsule collider component by handle.
      * @param[in] handle (sulphur::engine::CapsuleColliderComponent) The component handle.
      * @param[in] radius (float) The new radius.
      */
      void SetRadius(CapsuleColliderComponent handle, float radius);

      /**
      * @brief Retrieves the radius of a specific capsule collider component by handle.
      * @param[in] handle (sulphur::engine::CapsuleColliderComponent) The component handle.
      * @return (float) The radius of the capsule collider component.
      */
      float GetRadius(CapsuleColliderComponent handle) const;

      /**
      * @brief Sets the height of a specific capsule collider component by handle.
      * @param[in] handle (sulphur::engine::CapsuleColliderCompoennt) The component handle.
      * @param[in] height (float) The new height.
      */
      void SetHeight(CapsuleColliderComponent handle, float height);

      /**
      * @brief Retrieves the height of a specific capsule collider component by handle.
      * @param[in] handle (sulphur::engine::CapsuleColliderCompoennt) The component handle.
      * @return (float) The height of the capsule collider component.
      */
      float GetHeight(CapsuleColliderComponent handle) const;

    };


    //-------------------------------------------------------------------------
    template<typename T>
    inline ColliderSystemBase<T>::ColliderSystemBase(const char* name) :
      IComponentSystem<T, ColliderData<typename T::type>>(name)
    {
    }

    //-------------------------------------------------------------------------
    template <typename T>
    inline void ColliderSystemBase<T>::OnInitialize(Application& app, foundation::JobGraph&)
    {
      physics_service_ = &app.GetService<PhysicsSystem>();
    }

    //-------------------------------------------------------------------------
    template <typename T>
    inline void ColliderSystemBase<T>::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    template <typename T>
    inline T ColliderSystemBase<T>::Create(Entity entity)
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

      foundation::SharedPointer<typename T::type> ptr = 
        foundation::Memory::ConstructShared<typename T::type>();

      ptr->AttachToBody(body);

      return T(static_cast<typename T::System&>(*this), this->component_data_.data.Add(
        ptr,
        entity));
    }

    //-------------------------------------------------------------------------
    template <typename T>
    inline void ColliderSystemBase<T>::Destroy(T handle)
    {
      this->component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    inline typename T::type* ColliderSystemBase<T>::GetShape(T handle) const
    {
      return this->component_data_.data.template Get<static_cast<size_t>(ColliderComponentElements::kShape)>(handle).get();
    }

    //-------------------------------------------------------------------------
    template<typename T>
    inline ColliderComponentBase<T>::ColliderComponentBase() :
      base_system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    template<typename T>
    inline ColliderComponentBase<T>::ColliderComponentBase(BaseSystem& system, size_t handle) :
      ComponentHandleBase(handle),
      base_system_(&system)
    {
    }
  }
}