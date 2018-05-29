#pragma once 
#include "engine\application\i_editor_hook.h"
#include "engine\application\application_state.h"
#include <foundation\containers\vector.h>

namespace sulphur
{
  namespace engine
  {
    /**
    *@class sulphur::engine::Editor : sulphur::engine::IEditorHook
    *@brief Editor hook implmentation that connects to the editor and add editor specific logic to the application.
    *@author Stan Pepels
    */
    class Editor : public IEditorHook
    {
    public:
      /**
      *@brief Default Constructor.
      */
      Editor() : IEditorHook() {};

      /**
      * @brief Default Destructor.
      */
      virtual ~Editor() override {};

      /**
      * @see sulphur::engine::IEditorHook::ConstructPlatform
      */
      virtual foundation::UniquePointer<Platform> ConstructPlatform(foundation::Resource<foundation::SharedPointer<IRenderer>> renderer) override;
      
      /**
      * @see sulphur::engine::IEditorHook::AddEditorServices
      */
      virtual void AddEditorServices(SystemSet<IServiceSystemBase>& services) override;
      
      /**
      * @see sulphur::engine::IEditorHook::Release
      */
      virtual void Release() override;
      
      /**
      * @see sulphur::engine::IEditorHook::Update
      */
      virtual void Update(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler) override;
      
      /**
      * @see sulphur::engine::IEditorHook::Connected
      */
      virtual bool Connected() override;
      
      /**
      * @see sulphur::engine::IEditorHook::Intialize
      */
      virtual void Intialize(Application& app, foundation::JobGraph& job_graph) override;
      
      /**
      * @see sulphur::engine::IEditorHook::SendMessages
      */
      virtual void SendMessages() override;
      
      /**
      * @see sulphur::engine::IEditorHook::RecieveMessages
      */
      virtual void RecieveMessages() override;
    private:
      foundation::Vector<editor::IEditorListener*> listeners_; //!< Listeners that listen to messages send from the editor. 
      editor::EditorMessagingSystem* messageing_system_; //!< Messaging system used to send messages to the editor, and recieve / process incomming messages from the editor. 
      foundation::SharedPointer<editor::ConnectionManager> connection_; //!< Connection object used to manage the connection to the editor.
      ApplicationState states_; //!< Object that manages the different states of the application when connected to the editor.
    };
  }
}