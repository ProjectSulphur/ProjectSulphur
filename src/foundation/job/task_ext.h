#pragma once

#include "foundation/job/task.h"

namespace sulphur
{
  namespace foundation
  {
    /**
     * @class sulphur::foundation::TaskExt : public sulphur::foundation::Task
     * @brief Extended task which can store metadata required by the thread pool
     * @author Kenneth Buijssen
     */
    class TaskExt : public Task
    {
    public:
      /**
       * @brief Create an empty extended task
       */
      TaskExt();

      /**
       * @brief Create an extended task 
       * @param[in] task (sulphur::foundation::Task) The task to base the extended task from
       * @param[in] handle (sulphur::foundation::TaskHandle) The handle for the task
       */
      TaskExt(Task task, TaskHandle handle);

      /**
       * @brief Add a new sub task to the task, this increments the number of sub tasks.
       * @param[in] sub_task (sulphur::foundation::TaskHandle) The sub task to add to the task
       */
      void AddSubTask(TaskHandle sub_task);

      /**
       * @brief Indicate that one of the sub tasks no longer exists or is completed
       */
      void RemoveSubTask();

      /**
       * @brief Run the task
       */
      void Run() const;

      /**
       * @see id_
       */
      TaskHandle id() const;

      /**
      * @see num_sub_tasks_
      */
      size_t num_sub_tasks() const;

    private:
      TaskHandle id_;        //!< The internal id for the task used by the thread pool
      size_t num_sub_tasks_; //!< The number of uncompleted sub tasks for the task
    };
  }
}