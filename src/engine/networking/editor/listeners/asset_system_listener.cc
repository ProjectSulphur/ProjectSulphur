#include "engine/networking/editor/listeners/asset_system_listener.h"

#include "engine/networking/editor/editor_messages.h"
#include "engine/networking/editor/editor_messaging_system.h"
#include "engine/assets/asset_system.h"

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      //--------------------------------------------------------------------------------
      AssetSystemListener::AssetSystemListener(AssetSystem& asset_system, EditorMessagingSystem& message_system) :
        system_(&asset_system)
      {
        message_system.Subscribe(this, EditorMessageID::kCacheChanged);
      }

      //--------------------------------------------------------------------------------
      void AssetSystemListener::OnReceive(EditorMessageID id, const MessagePayload&)
      {
        if (id == EditorMessageID::kCacheChanged)
        {
          system_->RefreshCache();
        }
      }
    }
  }
}