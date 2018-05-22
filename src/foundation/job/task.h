#pragma once
#include <functional>

namespace sulphur
{
  namespace foundation
  {
    /**
     * @struct sulphur::foundation::TaskHandle
     * @brief Strongly typed task handle
     * @todo Generalize the handle structs
     * @author Kenneth Buijssen
     */
    struct TaskHandle
    {
      size_t id; //!< The internal task id referenced
    };

    /**
     * @class sulphur::foundation::Task
     * @brief Executable task used for scheduling execution of functions in the thread pool
     * @see sulphur::foundation::ThreadPool
     * @author Kenneth Buijssen
     */
    class Task
    {
    public:
      /**
       * @brief Create an empty task
       */
      Task();

      /**
       * @brief Create a task
       * @param[in] function (std::function<void()>) The function the task should execute
       */
      explicit Task(std::function<void()> function);

      /**
       * @see blocker_
       */
      TaskHandle blocker() const;
      
      /**
      * @see blocker_
      */
      void set_blocker(TaskHandle blocker);
      
      /**
      * @see parent_
      */
      TaskHandle parent() const;
      
      /**
      * @see parent_
      */
      void set_parent(TaskHandle parent);

    protected:
      std::function<void()> function_; //!< The function the task executes
      TaskHandle blocker_; //!< Blocking task which must execute before the current task can run 
      TaskHandle parent_;  //!< Parent task of the current task
    };
  }
}
