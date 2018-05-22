#pragma once

#include "foundation/job/task.h"

#include "foundation/memory/memory.h"
#include "foundation/containers/vector.h"

namespace sulphur
{
  namespace foundation
  {
    class TaskExt;

    /**
     * @class sulphur::foundation::ThreadPool
     * @brief The ThreadPool allows for scheduling and executing of tasks on multiple threads
     * @see sulphur::foundation::Task
     * @todo Actually make this multithreaded
     */
    class ThreadPool
    {
    public:
      /**
       * @brief Submit a new task for execution by the thread pool
       * @param[in] task (sulphur::foundation::Task&&) The task submitted
       */
      TaskHandle Submit(Task&& task);

      /**
       * @brief Execute all tasks on the current thread
       */
      void RunAllTasks();

      /**
       * @brief Get the handle of the currently executing task
       * @return (sulphur::foundation::TaskHandle)
       *         Handle to the currently execution task, or a zero handle if no task is executing
       */
      TaskHandle GetCurrentTaskId();

    private:
      /**
       * @brief Busy wait for the specified task. 
       *        Immediately returns if the task is already done executing
       * @param[in] task_handle (sulphur::foundation::TaskHandle) The task to wait for
       */
      void WaitFor(TaskHandle task_handle);

      /**
       * @brief Execute the specified task on the current thread
       * @param[in] task 
       *            (sulphur::foundation::SharedPointer <sulphur::foundation::TaskExt>)
       *            The task to execute
       */
      void Run(SharedPointer<TaskExt> task);
      
      /**
      * @brief Mark the specified task as complete, automatically removes it from its parent.
      * @param[in] task
      *            (sulphur::foundation::SharedPointer <sulphur::foundation::TaskExt>)
      *            The task to mark complete
      */
      void MarkAsComplete(SharedPointer<TaskExt> task);
      
      /**
       * @brief Try to pop a task from the work queue
       * @return (sulphur::foundation::SharedPointer <sulphur::foundation::TaskExt>)
       *         Shared pointer to the task, or a nullptr if the task is not on the queue
       */
      SharedPointer<TaskExt> TryPop();

      /**
      * @brief Try to pop a specific task from the work queue
      * @param[in] task_id (sulphur::foundation::TaskHandle) The handle of the task
      * @return (sulphur::foundation::SharedPointer <sulphur::foundation::TaskExt>)
      *         Shared pointer to the task, or a nullptr if the task is not on the queue
      */
      SharedPointer<TaskExt> TryPop(TaskHandle task_id);
      

      /**
      * @brief Get a pointer to a non completed task
      * @param[in] task_id (sulphur::foundation::TaskHandle) The handle of the task
      * @return (sulphur::foundation::SharedPointer <sulphur::foundation::TaskExt>)
      *          Shared pointer to the task, or a nullptr if the task is not on the queue
      */
      SharedPointer<TaskExt> GetTask(TaskHandle task_id);

      /**
       * @brief Check if the given task is done
       * @param[in] task_id (sulphur::foundation::TaskHandle) The handle of the task
       * @return (bool) False if the task is in the work queue or execution stack, otherwise false
       */
      bool IsDone(TaskHandle task_id);

    private:
      /**
       * @brief First in first out work queue
       */
      Vector<SharedPointer<TaskExt>> work_queue_;      

      /**
       * @brief Stack used to keep track of which tasks are currently executing
       */
      Vector<SharedPointer<TaskExt>> execution_stack_; 

      /**
       * @brief The last task id which was given out by the thread pool
       */
      size_t last_task_id_ = 0;
    };
  }
}
