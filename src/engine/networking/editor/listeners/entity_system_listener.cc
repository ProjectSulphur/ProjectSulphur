#include "engine/networking/editor/listeners/entity_system_listener.h"

#include "engine/networking/editor/editor_message_payloads.h"
#include "engine/networking/editor/editor_messaging_system.h"
#include "engine/core/entity_system.h"
#include "engine/systems/components/mesh_render_system.h"
#include "engine/systems/components/transform_system.h"

#include "engine/assets/asset_system.h"
#include "engine/assets/model.h"
#include "engine/assets/mesh.h"
#include "tranform_system_listener.h"
namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      //--------------------------------------------------------------------------------
      EntitySystemListener::EntitySystemListener(AssetSystem& asset_system, EntitySystem& enyity_system, EditorMessagingSystem& message_system, World& world) :
        system_(&enyity_system),
        asset_system_(&asset_system),
        world_(&world)
      {
        message_system.Subscribe(this, EditorMessageID::kEntityCreated);
        message_system.Subscribe(this, EditorMessageID::kEntityDestroyed);
        message_system.Subscribe(this, EditorMessageID::kEntityReparented);
        message_system.Subscribe(this, EditorMessageID::kAssetInstantiated);
        message_system.Subscribe(this, EditorMessageID::kAssetInstantiated);

        // debug mesh to show objects
        Mesh* editor_default_mesh_data_ = foundation::Memory::Construct<Mesh>();
        editor_default_mesh_data_->AttachMesh(Mesh::CreateSphere());
        editor_default_mesh_ = asset_system_->AddAsset(
          editor_default_mesh_data_,
          "editor_default_mesh");


      }

     
      //--------------------------------------------------------------------------------
      void EntitySystemListener::OnReceive(EditorMessageID id, const MessagePayload& payload)
      {
        if (id == EditorMessageID::kEntityCreated)
        {
          const EntityCreatedPayload& actual_payload = payload.AsFormat<EntityCreatedPayload>();

          Entity new_entity = system_->Create();

          TransformSystem& transform_system = world_->GetComponent<TransformSystem>();

          TransformComponent new_transform = new_entity.Add<TransformComponent>();
          if (actual_payload.parent_index != PS_SIZE_MAX) //!< PS_SIZE_MAX is indicative of the root node
          {
            TransformComponent parent_transform = transform_system.GetByHierarchyIndex(
              actual_payload.parent_index);
            if (parent_transform.IsValid() == false)
            {
              PS_LOG(Error, "Invalid parent index (%ld) received", actual_payload.parent_index);
              return;
            }
            new_transform.SetParent(parent_transform);
          }
          else
          {
            // @note (Maarten) When the entity's parent is the root, we must ensure that the entity
            //   becomes the sibling specified by the expected index to maintain consistency in
            //   communication between the engine and editor
            new_transform.SetSiblingIndex(actual_payload.sibling_index);
          }
          glm::vec3 p(actual_payload.postion[0], actual_payload.postion[1], actual_payload.postion[2]);
          glm::quat r(actual_payload.rotation[3], actual_payload.rotation[0], actual_payload.rotation[1], actual_payload.rotation[2]);
          glm::vec3 s(actual_payload.scale[0], actual_payload.scale[1], actual_payload.scale[2]);

          new_transform.SetWorldPosition(p);
          new_transform.SetWorldRotation(r);
          new_transform.SetWorldScale(s);
          // @note (Maarten) The following code is debug-stuff to show off the functionality. This
          //   will soon be removed once gizmos have been integrated.
          MeshRenderComponent new_mesh = new_entity.Add<MeshRenderComponent>();
          new_mesh.SetMesh(editor_default_mesh_);
        }
        else if (id == EditorMessageID::kEntityDestroyed)
        {
          const EntityDestroyedPayload& actual_payload =
            payload.AsFormat<EntityDestroyedPayload>();

          TransformSystem& transform_system = world_->GetComponent<TransformSystem>();

          TransformComponent target_transform = transform_system.GetByHierarchyIndex(
            actual_payload.entity_index);

          if (target_transform.IsValid() == false)
          {
            PS_LOG(Error, "Invalid entity index (%ld) received", actual_payload.entity_index);
            return;
          }

          system_->Destroy(transform_system.GetEntity(target_transform));
        }
        else if (id == EditorMessageID::kEntityReparented)
        {
          const EntityReparentedPayload& actual_payload =
            payload.AsFormat<EntityReparentedPayload>();

          TransformSystem& transform_system = world_->GetComponent<TransformSystem>();

          TransformComponent target_transform = transform_system.GetByHierarchyIndex(
            actual_payload.entity_old_index);
          if (target_transform.IsValid() == false)
          {
            PS_LOG(Error, "Invalid entity index (%ld) received", actual_payload.entity_old_index);
            return;
          }

          if (actual_payload.new_parent_old_index == PS_SIZE_MAX) //!< PS_SIZE_MAX is indicative of the root node
          {
            target_transform.UnsetParent();
            target_transform.SetSiblingIndex(actual_payload.new_sibling_index);
          }
          else
          {
            TransformComponent parent_transform = transform_system.GetByHierarchyIndex(
              actual_payload.new_parent_old_index);
            if (parent_transform.IsValid() == false)
            {
              PS_LOG(Error, "Invalid parent index (%ld) received", actual_payload.new_parent_old_index);
              return;
            }

            target_transform.SetParent(parent_transform);
          }
        }
        else if (id == EditorMessageID::kAssetInstantiated)
        {
          const AssetInstantiatedPayload& actual_payload =
            payload.AsFormat<AssetInstantiatedPayload>();

          Entity new_entity = system_->Create();

          editor_models_.emplace_back(asset_system_->Load<Model>(actual_payload.asset_id));
          ModelHandle& new_model = editor_models_.back();

          MeshRenderComponent new_mesh = new_entity.Add<MeshRenderComponent>();
          new_mesh.SetMesh(new_model->mesh_handle());
          new_mesh.SetMaterial(new_model->material_handles()[0]);

          PS_LOG(Debug, "Asset instantiated at x:%f y:%f z:%f", 0.0f, 0.0f, 0.0f);
        }
      }
    }
  }
}