#include "engine/core/editor/world_decorator.h"
#include "engine/networking/editor/editor_messages.h"
#include "engine/networking/editor/editor_messaging_system.h"

#include "engine/systems/editor/gizmo_system.h"
#include "engine/core/editor/free_cam_system.h"
#include "engine/assets/asset_system.h"
#include "engine/networking/editor/listeners/entity_system_listener.h"
#include "engine/networking/editor/listeners/tranform_system_listener.h"
#include "engine/systems/components/transform_system.h"

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      //--------------------------------------------------------------------------------
      WorldDecorator::WorldDecorator(World* world) :
        world_(world)
      {
        owners_.Create<GizmoSystem>();
        owners_.Create<FreeCamSystem>();
      }

      //--------------------------------------------------------------------------------
      WorldDecorator::~WorldDecorator()
      {
        for (IEditorListener* listener : listeners_)
        {
          foundation::Memory::Destruct(listener);
        }
        listeners_.clear();
      }

      //--------------------------------------------------------------------------------
      void WorldDecorator::OnReceive(EditorMessageID , const MessagePayload& )
      {
      }

      //--------------------------------------------------------------------------------
      void WorldDecorator::Initialize(Application& app, foundation::JobGraph& job_graph)
      {
        owners_.Execute(&IOwnerSystemBase::OnInitialize, app, job_graph);
        listeners_.push_back(
          foundation::Memory::Construct<EntitySystemListener>(
            app.GetService<AssetSystem>(),
            GetWorldOwner<EntitySystem>(),
            app.GetService<EditorMessagingSystem>(),
            *world_
            ));

        listeners_.push_back(
          foundation::Memory::Construct<TransformSystemListener>(
            GetWorldComponent<TransformSystem>(),
            app.GetService<EditorMessagingSystem>()
            )
        );
      }

      void WorldDecorator::set_world(World* world)
      {
        world_ = world;
      }

      void WorldDecorator::Release()
      {
        for (IEditorListener* listener : listeners_)
        {
          foundation::Memory::Destruct(listener);
        }
        listeners_.clear();
      }
    }
  }
}