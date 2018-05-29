#pragma once

#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/graphics/irenderer.h"

#include "engine/assets/mesh.h"
#include "engine/assets/material.h"
#include "engine/assets/skeleton.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/hash_map.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    class CameraSystem;
    class TransformSystem;
    class SkinnedMeshRenderSystem;
    class AnimationState;

    /**
    * @class sulphur::engine::SkinnedMeshRenderComponent
    * @brief A component that allows you to add a skinned mesh to an entity
    * @author Riko Ophorst
    */
    class SkinnedMeshRenderComponent : public ComponentHandleBase
    {
    public:
      using System = SkinnedMeshRenderSystem; //!< System type define

      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      SkinnedMeshRenderComponent();

      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::SkinnedMeshRendererComponent::System&) A reference to 
      * the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      SkinnedMeshRenderComponent(System& system, size_t handle);

      /**
      * @brief Sets the Mesh that this SkinnedMeshRenderComponent renders.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The new Mesh.
      * @remarks This automatically resizes the Materials set stored
      *          inside of this component to be the same size as the
      *          number of submeshes stored in the new mesh. No new
      *          materials are set whatsoever, however if the new 
      *          number of submeshes is greater than the old number
      *          of submeshes, this function adds appropriate 
      *          default materials in those new places.
      */
      void SetMesh(const MeshHandle& mesh);

      /**
      * @brief Retrieves the Mesh that is set on this SkinnedMeshRenderComponent.
      * @return (const sulphur::engine::MeshHandle&) The Mesh that is set on 
      *                                              this SkinnedMeshRenderComponent.
      */
      const MeshHandle& GetMesh();

      /**
      * @brief Sets a Material at a specific index on this component.
      * @param[in] material (const sulphur::engine::MaterialHandle&) The new material.
      * @param[in] index (size_t) The index at which the new material should be slotted.
      * @remarks If the Material passed in is invalid, a default 
      *          error Material is set instead. If the index is
      *          invalid (by being larger than the number of 
      *          submeshes in the mesh set on this component)
      *          then no action will happen. If you are passing
      *          in an invalid material, it will be replaced by
      *          a default error material.
      */
      void SetMaterial(const MaterialHandle& material, size_t index = 0);

      /**
      * @brief Retrieves a Material from a specific index from this component.
      * @param[in] index (size_t) The index from which to retrieve the material.
      * @returns The Material that resides at the given index.
      * @remarks If the index is invalid (by being larger than the 
      *          number of submeshes in the mesh set on this
      *          component) this will return an invalid handle.
      */
      MaterialHandle GetMaterial(size_t index = 0);

      /**
      * @brief Sets a set of Materials on this component.
      * @param[in] materials (const sulphur::foundation::Vector<MaterialHandle>&) The set of materials.
      * @remarks Every ith material in the set of materials will be
      *          applied to every ith submesh in this component's
      *          assigned Mesh during rendering. If you pass in more
      *          materials than submeshes, that is no problem. If
      *          you pass in less materials than submeshes, a default
      *          material will be added for every missing material.
      *          If you are passing in an invalid material, it will
      *          be replaced by a default error material.
      */
      void SetMaterials(const foundation::Vector<MaterialHandle>& materials);

      /**
      * @brief Retrieves the set of Materials on this component.
      * @returns The set of Materials on this component.
      * @remarks Remember that every ith material gets applied to
      *          every ith submesh in this component's Mesh.
      */
      const foundation::Vector<MaterialHandle>& GetMaterials();

      /**
      * @brief Sets the AnimationState assigned to this component.
      * @param[in] animation_state (sulphur::engine::AnimationState*)
      *            The AnimationState to assign to this component.
      */
      void SetAnimationState(AnimationState* animation_state);

      /**
      * @brief Retrieves the AnimationState assigned to this component.
      * @returns The AnimationState assigned to this component.
      */
      AnimationState* GetAnimationState();

      /**
      * @brief Sets whether this object should be rendered.
      * @param[in] visible (bool) Whether it is visible or not.
      */
      void SetVisible(bool visible);

      /**
      * @brief Returns whether this object is being rendered or not.
      * @returns Whether this object is visible or not.
      */
      bool IsVisible();

      /**
      * @brief Sets whether this object should cast shadows or not.
      * @param[in] casts_shadows (bool) Whether this object should cast shadows or not.
      */
      void SetCastsShadows(bool casts_shadows);

      /**
      * @brief Returns whether this object is casting shadows or not.
      * @returns Whether this object is casting shadows or not.
      */
      bool CastsShadows();

    private:
      SkinnedMeshRenderSystem* system_; //!< The system that owns this component
    };

    /**
    * @enum sulphur::engine::SkinnedMeshRenderComponentElements
    */
    enum struct SkinnedMeshRenderComponentElements
    {
      kEntity,
      kMesh,
      kMaterials,
      kAnimationState,
      kVisible,
      kCastsShadows
    };

    
    /**
    * @class sulphur::engine::SkinnedMeshRenderSystemData
    * @brief Contains all the data that the SkinnedMeshRenderSystem owns for each component. It
    * outlines exactly what the data is that is stored per component. Each member is a pointer to an
    * array of structures.
    */
    class SkinnedMeshRenderSystemData
    {
    public:
      /**
      * @brief Constructor of the light data that initializes the system data by
      * passing it the list of pointers. It uses the first element for this and 
      * assumes that the others follow.
      */
      SkinnedMeshRenderSystemData() :
        data(reinterpret_cast<void**>(&entity))
      {
      }

      Entity* entity;
      MeshHandle* mesh;
      foundation::Vector<MaterialHandle>* materials;
      AnimationState** animation_state;
      bool* visible;
      bool* casts_shadows;

      /** 
      * @brief Short-hand for the system data of this component. Allows easy access of data that is 
      * contained by the system per component.
      * @remarks Note that the template arguments are exactly the same as the types of data stored
      * in the actual system data.
      */
      using ComponentSystemData = SystemData<
        Entity, 
        MeshHandle, 
        foundation::Vector<MaterialHandle>,
        AnimationState*,
        bool, 
        bool
      >;

      ComponentSystemData data; //!< System data of the component.
    };

    /**
    * @class sulphur::engine::SkinnedMeshRenderSystem
    * @brief The ComponentSystem that manages all SkinnedMeshes in the World. Sends all the
    * necessary instructions to the IRenderer so that SkinnedMeshes get rendered appropriately.
    * @see sulphur::engine::SkinnedMeshRenderComponent
    * @author Riko Ophorst
    */
    class SkinnedMeshRenderSystem : 
      public IComponentSystem
    {
    public:
      /** Default constructor */
      SkinnedMeshRenderSystem();

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Destroys the mesh renderer component
      * @param[in] handle (ComponentHandleBase) The component to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Create a new SkinnedMeshRenderComponent for this entity and also creates a 
      * TransformComponent if it wasn't attached yet.
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for.
      * @see sulphur::engine::IComponentSystem::Create
      */
      SkinnedMeshRenderComponent Create(Entity& entity);
      
    private:
      /**
      * @brief Updates all the AnimationStates in the SkinnedMeshRenderSystem.
      */
      void UpdateAnimationStates();

      /**
      * @brief Renders all the SkinnedMeshRenderComponents in the World to the screen.
      */
      void RenderMeshes();

    public:
      /**
      * @brief Sets the Mesh that the given component renders.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The new Mesh.
      * @see sulphur::engine::SkinnedMeshRenderComponent::SetMesh
      */
      void SetMesh(SkinnedMeshRenderComponent handle, const MeshHandle& mesh);

      /**
      * @brief Retrieves the Mesh that is set on the given component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @return (const sulphur::engine::MeshHandle&) 
      *         The Mesh that is set on the given component.
      * @see sulphur::engine::SkinnedMeshRenderComponent::GetMesh
      */
      const MeshHandle& GetMesh(SkinnedMeshRenderComponent handle);

      /**
      * @brief Sets a Material at a specific index on the given component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accesseds.
      * @param[in] material (const sulphur::engine::MaterialHandle&) 
      *            The new material.
      * @param[in] index (size_t) 
      *            The index at which the new material should be slotted.
      * @see sulphur::engine::SkinnedMeshRenderComponent::SetMaterial
      */
      void SetMaterial(SkinnedMeshRenderComponent handle, const MaterialHandle& material, size_t index);

      /**
      * @brief Retrieves a Material from a specific index from the given component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] index (size_t) 
      *            The index from which to retrieve the material.
      * @returns The Material that resides at the given index.
      * @see sulphur::engine::SkinnedMeshRenderComponent::GetMaterial
      */
      MaterialHandle GetMaterial(SkinnedMeshRenderComponent handle, size_t index);

      /**
      * @brief Sets a set of Materials on the given component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] materials (const sulphur::foundation::Vector<MaterialHandle>&) 
      *            The set of materials.
      * @see sulphur::engine::SkinnedMeshRenderComponent::SetMaterials
      */
      void SetMaterials(SkinnedMeshRenderComponent handle, const foundation::Vector<MaterialHandle>& materials);
      
      /**
      * @brief Retrieves the set of Materials on the given component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @returns The set of Materials on the given component.
      * @see sulphur::engine::SkinnedMeshRenderComponent::GetMaterials
      */
      const foundation::Vector<MaterialHandle>& GetMaterials(SkinnedMeshRenderComponent handle);

      /**
      * @brief Sets the AnimationState assigned to this component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] animation_state (sulphur::engine::AnimationState*)
      *            The AnimationState to assign to this component.
      */
      void SetAnimationState(SkinnedMeshRenderComponent handle, AnimationState* animation_state);

      /**
      * @brief Retrieves the AnimationState assigned to this component.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @returns The AnimationState assigned to this component.
      */
      AnimationState* GetAnimationState(SkinnedMeshRenderComponent handle);

      /**
      * @brief Sets whether the given component should be rendered.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] visible (bool) Whether the given component is visible or not.
      * @see sulphur::engine::SkinnedMeshRenderComponent::SetVisible
      */
      void SetVisible(SkinnedMeshRenderComponent handle, bool visible);

      /**
      * @brief Returns whether the given component is being rendered or not.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @returns Whether the given component is visible or not.
      * @see sulphur::engine::SkinnedMeshRenderComponent::IsVisible
      */
      bool IsVisible(SkinnedMeshRenderComponent handle);

      /**
      * @brief Sets whether the given component should cast shadows or not.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @param[in] casts_shadows (bool) 
      *            Whether the given component should cast shadows or not.
      * @see sulphur::engine::SkinnedMeshRenderComponent::SetCastsShadows
      */
      void SetCastsShadows(SkinnedMeshRenderComponent handle, bool casts_shadow);

      /**
      * @brief Returns whether the given component is casting shadows or not.
      * @param[in] handle (sulphur::engine::SkinnedMeshRenderComponent) 
      *            The handle to the component that should be accessed.
      * @returns Whether the given component is casting shadows or not.
      * @see sulphur::engine::SkinnedMeshRenderComponent::CastsShadows
      */
      bool CastsShadows(SkinnedMeshRenderComponent handle);
    private:
      CameraSystem* camera_system_;       //!< Keep a pointer to the CameraSystem.
      TransformSystem* tranform_system_;  //!< Keep a pointer to the TransformSystem.
      IRenderer* renderer_;               //!< Keep a pointer to the IRenderer.

      SkinnedMeshRenderSystemData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}