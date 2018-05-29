#include "engine/core/editor/editor_world_provider.h"

#include "engine/networking/editor/editor_messaging_system.h"
#include "engine/core/world.h"
#include "engine/core/editor/world_decorator.h"

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      //--------------------------------------------------------------------------------
      EditorWorldProvider::EditorWorldProvider(WorldProviderSystem* world, EditorMessagingSystem* messaging_system) :
        IServiceSystem("editor_world_provider_system"),
        current_world_(nullptr),
        world_provider_system_(world),
        messaging_system_(messaging_system)
      {
        messaging_system_->Subscribe(this, EditorMessageID::kCacheChanged);
      }

      //--------------------------------------------------------------------------------
      void EditorWorldProvider::OnTerminate()
      {
        foundation::Memory::Destruct(current_world_);
      }

      //--------------------------------------------------------------------------------
      void EditorWorldProvider::CreateEmptyWorld(Application& app, foundation::JobGraph& job_graph)
      {
        current_world_->Release();
        world_provider_system_->CreateEmptyWorld(app, job_graph);
        current_world_->set_world(&world_provider_system_->GetWorld());
        current_world_->Initialize(app, job_graph);
      }

      //--------------------------------------------------------------------------------
      EditorWorldProvider::~EditorWorldProvider()
      {
        OnTerminate();
      }

      //--------------------------------------------------------------------------------
      void EditorWorldProvider::OnReceive(EditorMessageID, const MessagePayload&)
      {
        
      }

      //--------------------------------------------------------------------------------
      void EditorWorldProvider::OnInitialize(Application& app, foundation::JobGraph& job_graph)
      {
        current_world_ = foundation::Memory::Construct<WorldDecorator>(&world_provider_system_->GetWorld());
        current_world_->Initialize(app, job_graph);
      }
    }
  }
}
