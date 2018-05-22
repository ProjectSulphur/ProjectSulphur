#pragma once

#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/graphics/irenderer.h"

#include "engine/assets/mesh.h"
#include "engine/assets/material.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/hash_map.h>
#include <foundation/memory/memory.h>

namespace sulphur 
{
  namespace engine 
  {
    class CameraSystem;
    class TransformSystem;
    class MeshRenderSystem;

    /**
    * @class MeshRenderComponent
    * @brief The component type used by the MeshRenderSystem to link meshes to materials and any additional render settings
    * @author Jelle de Haan
    */
    class MeshRenderComponent : public ComponentHandleBase
    {
    public:
      using System = MeshRenderSystem; //!< System type define
      
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      MeshRenderComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::MeshRendererComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      MeshRenderComponent(System& system, size_t handle);

      /**
      * @brief Links a mesh to this component
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The mesh to link
      * @return (sulphur::engine::MeshRenderComponent) The caller node (for chaining)
      */
      MeshRenderComponent SetMesh(const MeshHandle& mesh);

      /**
      * @brief Returns the handle to the mesh used by this component
      * @return (sulphur::engine::MeshHandle) The mesh
      */
      MeshHandle GetMesh() const;

      /**
      * @brief Links a material to this component 
      * @param[in] material (const sulphur::engine::MaterialHandle&) The material to link
      * @param[in] index (size_t) Unused, can't use until we figure out to do multi-material meshes
      * @return (sulphur::engine::MeshRenderComponent) The caller node (for chaining)
      */
      MeshRenderComponent SetMaterial(const MaterialHandle& material, size_t index = 0);

      /**
      * @brief Returns the handle to the material used by this component
      * @param[in] index (const sulphur::engine::MaterialHandle&) Unused, can't use until we figure out to do multi-material meshes
      * @return (sulphur::engine::MaterialHandle) The material
      */
      MaterialHandle GetMaterial(size_t index = 0) const;

      /**
      * @brief Replace all materials linked to this component
      * @param[in] materials (const foundation::Vector <sulphur::engine::MaterialHandle>& materials) The new vector of materials
      * @return (sulphur::engine::MeshRenderComponent) The caller node (for chaining)
      */
      MeshRenderComponent SetMaterials(const foundation::Vector<MaterialHandle>& materials);

      /**
      * @brief Returns a copy of all materials linked to this component
      * @return (foundation::Vector <sulphur::engine::MaterialHandle> materials) The vector of materials
      */
      foundation::Vector<MaterialHandle> GetMaterials() const;

      /**
      * @brief Define if this component is rendered or not
      * @param[in] value (bool) True if the component should be visible
      * @return (sulphur::engine::MeshRenderComponent) The caller node (for chaining)
      */
      MeshRenderComponent SetVisible(bool value);

      /**
      * @brief Returns if this component is rendered or not
      * @return (bool) Is the component visible?
      */
      bool IsVisible() const;

      /**
      * @brief Define if this component casts shadows or not
      * @param[in] value (bool) True if the component should cast shadows
      * @return (sulphur::engine::MeshRenderComponent) The caller node (for chaining)
      */
      MeshRenderComponent SetCastShadows(bool value);

      /**
      * @brief Returns if this component casts shadows or not
      * @return (bool) Does the component cast shadows?
      */
      bool CastsShadows() const;

    private:
      MeshRenderSystem* system_;

    };
    enum struct MeshRenderComponentElements
    {
      kMesh,
      kMaterials,
      kCastShadows,
      kVisible,
      kOpaque,
      kEntity
    };
    /*
    * @brief The data of the sulphur::engine::MeshRenderSystemData component.
    * @author Raymi Klingers
    */
    class MeshRenderSystemData
    {
    public:
      using ComponentSystemData = SystemData<MeshHandle, foundation::Vector<MaterialHandle>, bool, bool, bool, Entity>;//<! Alias of the system data. The order of the member variables should be in sync with @see sulphur::engine::LightComponentElements and the pointers in this struct

      /*
      * @brief Constructor of the light data that initializes the system data by passing it the list of pointers. It uses the first element for this and assumes that the others follow
      */
      MeshRenderSystemData()
        :
        data((void**)&mesh)
      {
      }

      MeshHandle* mesh;//!< Simple direct access to the mesh array data.
      foundation::Vector<MaterialHandle>* materials;//!< Simple direct access to the materials vector data.
      bool* casts_shadows;//!< Simple direct access to the casts_shadows array data.
      bool* visible;//!< Simple direct access to the visible array data.
      bool* opaque;//!< Simple direct access to the opaque array data.
      Entity* entity;//!< Simple direct access to the entity array data.
      ComponentSystemData data;//!< System data of the component.
    };
    /**
    * @class sulphur::engine::MeshRenderSystem : public IComponentSystem< MeshRenderComponent, MeshRenderSystemData >
    * @brief Manages MeshRenderComponent's and prepares the renderer for a frame
    * @see sulphur::engine::MeshRenderComponent
    * @author Jelle de Haan
    */
    class MeshRenderSystem : public IComponentSystem<MeshRenderComponent, MeshRenderSystemData>
    {
    public:
      /**
      * @brief Default constructor
      */
      MeshRenderSystem();

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
      * @brief Create a new mesh renderer component for this entity and also creates a TransformComponent if it wasn't attached yet
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for
      * @see sulphur::engine::IComponentSystem::Create
      */
      MeshRenderComponent Create(Entity entity) override;

      /**
      * @brief Destroys the mesh renderer component
      * @param[in] handle (MeshRenderComponent) The component to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(MeshRenderComponent handle) override;
      
      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnPreRender
      */
      void OnPreRender() override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnRender
      */
      void OnRender() override;

      /**
      * @brief Links a mesh to this component handle
      * @param[in] handle (MeshRenderComponent) The handle the mesh will be linked to
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The mesh to link
      */
      void SetMesh(MeshRenderComponent handle, const MeshHandle& mesh);

      /**
      * @brief Returns the handle to the mesh used by this component handle
      * @param[in] handle (MeshRenderComponent) The handle the mesh is linked to
      * @return (sulphur::engine::MeshHandle) The mesh
      */
      MeshHandle GetMesh(MeshRenderComponent handle) const;

      /**
      * @brief Links a material to this component handle
      * @param[in] handle (MeshRenderComponent) The handle the material will be linked to
      * @param[in] material (const sulphur::engine::MaterialHandle&) The material to link
      * @param[in] index (size_t) Unused, can't use until we figure out to do multi-material meshes
      */
      void SetMaterial(MeshRenderComponent handle, const MaterialHandle& material, size_t index = 0);

      /**
      * @brief Returns the handle to the material used by this component handle
      * @param[in] handle (MeshRenderComponent) The handle the material is linked to
      * @param[in] index (const sulphur::engine::MaterialHandle&) Unused, can't use until we figure out to do multi-material meshes
      * @return (sulphur::engine::MaterialHandle) The material
      */
      MaterialHandle GetMaterial(MeshRenderComponent handle, size_t index = 0) const;

      /**
      * @brief Replace all materials linked to this component handle
      * @param[in] handle (MeshRenderComponent) The handle these materials will be linked to
      * @param[in] materials (const foundation::Vector <sulphur::engine::MaterialHandle>& materials) The new vector of materials
      */
      void SetMaterials(MeshRenderComponent handle, const foundation::Vector<MaterialHandle>& materials);

      /**
      * @brief Returns a copy of all materials linked to this component
      * @param[in] handle (MeshRenderComponent) The handle these materials are linked to
      * @return (foundation::Vector <sulphur::engine::MaterialHandle> materials) The vector of materials
      */
      foundation::Vector<MaterialHandle> GetMaterials(MeshRenderComponent handle) const;

      /**
      * @brief Define if this component is rendered or not
      * @param[in] handle (MeshRenderComponent) The handle to the component to set visible
      * @param[in] value (bool) True if the component should be visible
      */
      void SetVisible(MeshRenderComponent handle, bool value);

      /**
      * @brief Returns if this component is rendered or not
      * @param[in] handle (MeshRenderComponent) The handle to the component to check
      * @return (bool) Is the component visible?
      */
      bool IsVisible(MeshRenderComponent handle) const;

      /**
      * @brief Define if this component casts shadows or not
      * @param[in] handle (MeshRenderComponent) The handle to the component to set
      * @param[in] value (bool) True if the component should cast shadows
      */
      void SetCastShadows(MeshRenderComponent handle, bool value);

      /**
      * @brief Returns if this component casts shadows or not
      * @param[in] handle (MeshRenderComponent) The handle to the component to check
      * @return (bool) Does the component cast shadows?
      */
      bool CastsShadows(MeshRenderComponent handle) const;

    private:
      CameraSystem* camera_system_;
      TransformSystem* tranform_system_;
      IRenderer* renderer_;

      foundation::HashMap<size_t, foundation::Vector<Entity>> camera_entity_map_;
    };
  }
}