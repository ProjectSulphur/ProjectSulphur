#include "engine/networking/editor/listeners/application_listener.h"

#include "engine/networking/editor/editor_messages.h"
#include "engine/networking/editor/editor_messaging_system.h"
#include "engine/networking/editor/editor_message_payloads.h"
#include "engine/application/application.h"
#include "engine/application/hooks.h"
#include "engine/scripting/script_system.h"

#include <foundation/job/job_graph.h>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {

      //--------------------------------------------------------------------------------
      ApplicationListener::ApplicationListener(Application& app, EditorMessagingSystem& message_system) :
        app_(&app)
      {
        message_system.Subscribe(this, EditorMessageID::kLoadProject);
      }

      //--------------------------------------------------------------------------------
      void ApplicationListener::OnReceive(EditorMessageID id, const MessagePayload& payload)
      {
        if (id == EditorMessageID::kLoadProject)
        {
          const LoadProjectPayload& actual_payload =
            payload.AsFormat<editor::LoadProjectPayload>();
          const char* val = reinterpret_cast<const char*>(actual_payload.path);
          foundation::Path path = val;
          app_->SetProjectDirectory(path);
        }
      }
    }
  }
}