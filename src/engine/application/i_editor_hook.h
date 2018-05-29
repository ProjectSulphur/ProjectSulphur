#pragma once 
#include "engine/application/platform.h"

#include <foundation/job/resource.h>
namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
    class JobGraphExt;
    class ThreadPool;
  }

  namespace engine
  {
    class FrameScheduler;
    class Application;
    class IServiceSystemBase;

    template<typename T>
    class SystemSet;

    /**
    * @class sulphur::engine::IEditorHooks
    * @brief Interface that defines the functions required to hook the editor into the engine application.
    * @author Stan Pepels
    */
    class IEditorHook
    {
    public:
      /**
      * @brief Constructor.
      */
      IEditorHook() {};

      /*
      * @brief Deconstructor.
      */
      virtual ~IEditorHook() {};

      /**
      * @brief Constructs the low level platformlayer with an editor connection.
      * @param[in] renderer (sulphur::foundation::SharedPointerResource<sulphur::engine::IRenderer>) The renderer to initialize when constructing the platform layer.
      * @return (sulphur::foundation::UniquePointer<sulphur::engine::Platform>) Platform layer constructed.
      */
      virtual foundation::UniquePointer<Platform> ConstructPlatform(foundation::Resource<foundation::SharedPointer<IRenderer>> renderer) = 0;
      
      /**
      * @brief Initialize the editor hook. This is where all editor only systems should be created. It is also assumed that the application class is fully initialized when this function gets called.
      * @param[in] app (sulphur::engine::Application) Application hooked into. This instance is assumed to be fully initialized.
      * @param[in] job_graph(sulphur::foundation::JobGraph&) Job graph used by the application. This can be used to add editor specific job to the application.
      */
      virtual void Intialize(Application& app, foundation::JobGraph& job_graph) = 0;

      /**
      * @brief Construct editor services and adds them to the application list of serviced. This function should be called directly after the application main services have been created.
      * @param[in] system_set (sulphur::engine::SystemSet<sulphur::engine::IServiceSystemBase>&) Services list of the application. This is where the editor services should be added to.
      */
      virtual void AddEditorServices(SystemSet<IServiceSystemBase>& system_set) = 0;

      /**
      * @brief Release the resources allocated in the initialze function.
      */
      virtual void Release() = 0;

      /**
      * @brief Runs optional editor update logic.
      * @param[in] thread_pool (sulphur::foundation::ThreadPool&) Threadpool used to execute jobs in the job graph.
      * @param[in] job_graph (sulphur::foundation::JobGraphExt& job_graph) Job graph with jobs that can be executed in a frame.
      * @param[in] frame_scheduler (sluphur::foundation::Resource<FrameScheduler>& frame_scheduler) Frame sheduler that can be used to schedule fixed update.
      */
      virtual void Update(foundation::ThreadPool& thread_pool,
        foundation::JobGraphExt& job_graph,
        foundation::Resource<FrameScheduler>& frame_scheduler) = 0;
      

      /**
      * @brief Checks if the editor is connected.
      * @return (bool) Should return true if there is a valid connection set up and false otherwise.
      */
      virtual bool Connected() = 0;

      /**
      * @brief Sends messages to the editor. This function should be called after an update has finished.
      */
      virtual void SendMessages() = 0;

      /**
      * @brief Processes messages recieved. This function should be called before an update starts.
      */
      virtual void RecieveMessages() = 0;
    };
  }
}