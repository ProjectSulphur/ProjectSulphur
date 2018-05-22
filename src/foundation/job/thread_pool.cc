#include "foundation/job/thread_pool.h"

#include "foundation/job/task_ext.h"
#include <algorithm>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    TaskHandle ThreadPool::Submit(Task&& task)
    {
      TaskHandle task_id = TaskHandle{ ++last_task_id_ };

      work_queue_.push_back(Memory::ConstructShared<TaskExt>(task, task_id));

      // Inform the parent that it now has a sub task
      if (SharedPointer<TaskExt> parent = GetTask(task.parent()))
      {
        parent->AddSubTask(task_id);
      }

      return task_id;
    }

    //--------------------------------------------------------------------------
    void ThreadPool::RunAllTasks()
    {
      while(SharedPointer<TaskExt> current_task = TryPop())
      {
        Run(current_task);
      }
    }

    //--------------------------------------------------------------------------
    TaskHandle ThreadPool::GetCurrentTaskId()
    {
      return execution_stack_.size() == 0 ? TaskHandle{ 0 } : execution_stack_.back()->id();
    }

    //--------------------------------------------------------------------------
    void ThreadPool::WaitFor(TaskHandle task_id)
    {
      // Run other tasks until our target is done
      while (!IsDone(task_id))
      {
        if (SharedPointer<TaskExt> task = TryPop())
        {
          Run(task);
        }
      }
    }

    //--------------------------------------------------------------------------
    void ThreadPool::Run(SharedPointer<TaskExt> task)
    {
      execution_stack_.push_back(task);

      // Tasks need to wait for their blocker before running
      WaitFor(task->blocker());
      

      // Run task code
      task->Run();

      // Run other tasks until all children are done
      while (task->num_sub_tasks() != 0)
      {
        if (SharedPointer<TaskExt> t = TryPop())
        {
          Run(t);
        }
      }

      //Now all children are done, we can mark the task as complete
      MarkAsComplete(task);

      execution_stack_.pop_back();
    }

    //--------------------------------------------------------------------------
    void ThreadPool::MarkAsComplete(SharedPointer<TaskExt> task)
    {
      if (SharedPointer<TaskExt> parent = GetTask(task->parent()))
      {
        parent->RemoveSubTask();
      }
    }

    //--------------------------------------------------------------------------
    SharedPointer<TaskExt> ThreadPool::TryPop()
    {
      Vector<SharedPointer<TaskExt>>::iterator it = std::find_if(
        work_queue_.begin(),
        work_queue_.end(),
        [this](const SharedPointer<TaskExt>& t) { return IsDone(t->blocker()); }
      );

      SharedPointer<TaskExt> task = nullptr;

      if (it != work_queue_.end())
      {
        task = *it;
        work_queue_.erase(it);
      }

      return task;
    }

    //--------------------------------------------------------------------------
    SharedPointer<TaskExt> ThreadPool::TryPop(TaskHandle task_id)
    {
      Vector<SharedPointer<TaskExt>>::iterator it = std::find_if(
        work_queue_.begin(),
        work_queue_.end(),
        [this, task_id](const SharedPointer<TaskExt>& t)
          { return t->id().id ==task_id.id && IsDone(t->blocker()); }
      );

      SharedPointer<TaskExt> task = nullptr;

      if (it != work_queue_.end())
      {
        task = *it;
        work_queue_.erase(it);
      }

      return task;
    }

    //--------------------------------------------------------------------------
    SharedPointer<TaskExt> ThreadPool::GetTask(TaskHandle task_id)
    {
      Vector<SharedPointer<TaskExt>>::iterator queue_it = std::find_if(
        work_queue_.begin(),
        work_queue_.end(),
        [task_id](const SharedPointer<TaskExt>& t) { return t->id().id == task_id.id; }
      );

      if (queue_it != work_queue_.end())
      {
        return nullptr;
      }

      Vector<SharedPointer<TaskExt>>::iterator stack_it = std::find_if(
        execution_stack_.begin(),
        execution_stack_.end(),
        [task_id](SharedPointer<TaskExt>& t) { return task_id.id == t->id().id; }
      );

      return stack_it != execution_stack_.end() ? *stack_it : nullptr;
    }

    //--------------------------------------------------------------------------
    bool ThreadPool::IsDone(TaskHandle task_id)
    {
      return GetTask(task_id) == nullptr;
    }
  }
}
