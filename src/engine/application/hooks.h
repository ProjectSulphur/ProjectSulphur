#pragma once

#ifdef PS_EDITOR
#include "engine/application/editor/editor_hook.h"
namespace sulphur
{
  namespace engine
  {
    using EditorHook = Editor;
  }
}

#else
#include "engine/application/i_editor_hook.h"
namespace sulphur
{
  namespace engine
  {
    /**
    *@class sulphur::engine::Editor : sulphur::engine::IEditorHook
    *@brief NoEditor Empty editor hook implementation this does not add any logic to the main application.
    *@author Stan Pepels
    */
    class NoEditor : public IEditorHook
    {
    public:
      /**
      *@brief Default Constructor.
      */
      NoEditor() : IEditorHook() {};
      
      /**
      * @brief Default Destructor.
      */
      virtual ~NoEditor() override {};
      
      /**
      * @see sulphur::engine::IEditorHook::ConstructPlatform
      */
      virtual foundation::UniquePointer<Platform> ConstructPlatform(foundation::Resource<foundation::SharedPointer<IRenderer>> renderer) override { return nullptr; };
      
      /**
      * @see sulphur::engine::IEditorHook::AddEditorServices
      */
      virtual void AddEditorServices(SystemSet<IServiceSystemBase>& services) override {};
      
      /**
      * @see sulphur::engine::IEditorHook::Release
      */
      virtual void Release() override {};
      
      /**
      * @see sulphur::engine::IEditorHook::Update
      */
      virtual void Update(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler) override {};
      
      /**
      * @see sulphur::engine::IEditorHook::Connected
      */
      virtual bool Connected() override { return false; };
      
      /**
      * @see sulphur::engine::IEditorHook::Intialize
      */
      virtual void Intialize(Application& app, foundation::JobGraph& job_graph) override {};
      
      
      /**
      * @see sulphur::engine::IEditorHook::SendMessages
      */
      virtual void SendMessages() override {};
      
      /**
      * @see sulphur::engine::IEditorHook::RecieveMessages
      */
      virtual void RecieveMessages() override {};
    };

    using EditorHook = NoEditor;
  }
}
#endif // DEBUG