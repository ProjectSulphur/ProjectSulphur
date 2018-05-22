#pragma once

#include "foundation/job/access_type.h"

namespace sulphur 
{
  namespace foundation 
  {
    class BaseResource;
    class Job;

    /**
     * @class sulphur::foundation::ExecutionContext
     * @brief The execution context allows for specifying which job is running
     *        on the current thread, and checking if resources are accessible 
     *        by the current job.
     * @author Kenneth Buijssen
     */
    class ExecutionContext
    {
    public:
      /**
       * @brief Sets the given job as the current job for the calling thread
       * @param[in] job (const Job*) 
       *            The job to set as the current job
       */
      static void SetCurrentJob(const Job* job);

      /**
       * @brief Assert to ensure that the current thread has the correct access
       *        to the specified resource.
       * @param[in] resource (const sulphur::foundation::BaseResource&)
       *            The resource to check access to
       * @param[in] access (sulphur::foundation::AccessType
       *            The access level to check for
       */
      static void AssertResourceAccessAllowed(const BaseResource& resource, AccessType access);

      /**
       * @brief Checks if the current thread is allowed to access the given resource
       *        in the specified way.
       * @param[in] resource (const sulphur::foundation::BaseResource&)
       *            The resource to check access to
       * @param[in] access (sulphur::foundation::AccessType
       *            The access level to check for
       * @return (bool) True if the thread has accesss to the provided resource
       */
      static bool IsAllowed(const BaseResource& resource, AccessType access);

    private:
      /**
       * @brief Reference to the job assigned to the current thread
       */
      static thread_local const Job* current_job_;
    };
  }
}