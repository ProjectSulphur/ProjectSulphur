#pragma once
#include "engine/networking/editor/editor_listener.h"
#include "engine/assets/asset_interfaces.h"

namespace sulphur
{
  namespace engine
  {
    class EntitySystem;
    class AssetSystem;
    using MeshHandle = AssetHandle<class Mesh>;
    using ModelHandle = AssetHandle<class Model>;
    class World;

    namespace editor
    {   
      class EditorMessagingSystem;

      /**
      * @class sulphur::engine::EntitySystemListener : sulphur::engine::IEditorListener
      * @brief Class that processed editor messaged related to the entity system.
      * @author Stan Pepels
      */
      class EntitySystemListener : public IEditorListener
      {
      public:
        /**
        * @brief Subscribes the listener to a message system used for processing messages from the editor.
        * @param[in] asset_system (sulphur::engine::EntitySystem&) Asset system to use when processing editor messages.
        * @param[in] message_system (sulphur::engine::EditorMessagingSystem&) Message system to recieve editor messages from.
        */
        EntitySystemListener(AssetSystem& asset_system, EntitySystem& entity_system, EditorMessagingSystem& message_system, World& world);

        /**
        * @see sulphur::engine::IEditorListener::OnReceive
        */
        void OnReceive(EditorMessageID id, const MessagePayload& payload) override;
      private:
        EntitySystem* system_; //!< system to use when an editor message comes in.
        AssetSystem* asset_system_; //!< Asset system used for loading in assets when an entity is created with an asset component.
        MeshHandle editor_default_mesh_; //!< Mesh used as defualt for editor objects.
        World* world_; //!< World the entities are added to.
        foundation::Vector<ModelHandle> editor_models_; //!< A list of models that have been instantiated by the editor
      };
    }
  }
}