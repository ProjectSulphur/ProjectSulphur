#include "foundation/job/task.h"

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    Task::Task() : Task(nullptr)
    {
      
    }
    
    //--------------------------------------------------------------------------
    Task::Task(std::function<void()> function) :
      function_(function),
      blocker_(),
      parent_()
    {
      
    }

    //--------------------------------------------------------------------------
    TaskHandle Task::blocker() const
    {
      return blocker_;
    }

    //--------------------------------------------------------------------------
    void Task::set_blocker(TaskHandle blocker)
    {
      blocker_ = blocker;
    }

    //--------------------------------------------------------------------------
    TaskHandle Task::parent() const
    {
      return parent_;
    }

    //--------------------------------------------------------------------------
    void Task::set_parent(TaskHandle parent)
    {
      parent_ = parent;
    }
  }
}