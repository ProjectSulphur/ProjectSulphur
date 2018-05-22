#include "foundation/job/task_ext.h"

#include "foundation/logging/logger.h"

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    TaskExt::TaskExt() : TaskExt(Task(), TaskHandle{ 0 })
    {

    }

    //--------------------------------------------------------------------------
    TaskExt::TaskExt(Task task, TaskHandle id) :
      Task(task),
      id_(id),
      num_sub_tasks_(0)
    {

    }

    //--------------------------------------------------------------------------
    void TaskExt::AddSubTask(TaskHandle sub_task)
    {
      PS_LOG_IF(id_.id >= sub_task.id, Assert, "Parent needs to be defined before the child");

      ++num_sub_tasks_;
    }

    //--------------------------------------------------------------------------
    void TaskExt::RemoveSubTask()
    {
      --num_sub_tasks_;
    }

    //--------------------------------------------------------------------------
    void TaskExt::Run() const
    {
      function_();
    }

    //--------------------------------------------------------------------------
    TaskHandle TaskExt::id() const
    {
      return id_;
    }

    //--------------------------------------------------------------------------
    size_t TaskExt::num_sub_tasks() const
    {
      return num_sub_tasks_;
    }
  }
}
