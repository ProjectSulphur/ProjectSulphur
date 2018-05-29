#pragma once
#include "engine/systems/component_system.h"
#include "engine/systems/owner_system.h"
#include "engine/networking/editor/editor_listener.h"

namespace sulphur
{
  namespace engine
  {
    // temp declarations untill graphics resource releasing is fixed
    class TranslateGizmo;
    class ScaleGizmo;
    class RotateGizmo;
    // end temp


    class GizmoBase;
    class TransformComponent;
    class TransformSystem;
    /**
    *@class sulphur::engine::GizmoSystem : sulphur::engine::IOwnerSystem<T>, sulphur::engine::editor::IMessageListener
    *@brief System responsible for managing connections between gizmos and entities.
    *@author Stan Pepels
    */
    class GizmoSystem : public IOwnerSystem<GizmoSystem>, public editor::IEditorListener
    {
    public:
      /**
      *@brief Constructor
      */
      GizmoSystem();

      /**
      *@see sulphur::engine::IOwnerSystem<T>::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) final;

      /**
      *@see sulphur::engine::IOwnerSystem<T>::OnTerminate
      */
      void OnTerminate() final;

      /**
      *@see sulphur::engine::editor::IMessageListener::OnReceive
      */
      void OnReceive(editor::EditorMessageID id, const MessagePayload& payload) final;

    protected:
      TransformSystem* transform_system_;
      foundation::Map<GizmoBase*, foundation::Vector<TransformComponent>> gizmo_map_; //!< Map of gizmos active in the scene and the entities they are linked to.
      eastl::pair<GizmoBase*, foundation::Vector<TransformComponent>> transform_gizmo_; //!< The currently active transform gizmo. Only one can be active at any given time.

      // temp declarations untill graphics resource releasing is fixed
      TranslateGizmo* translate_gizmo_;
      ScaleGizmo* scale_gizmo_;
      RotateGizmo* rotate_gizmo_;

    };
  }
}