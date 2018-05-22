#include "foundation/job/execution_context.h"

#include "foundation/job/job.h"

#include "foundation/logging/logger.h"
#include "foundation/job/resource.h"

namespace sulphur
{
  namespace foundation
  {
    thread_local const Job* ExecutionContext::current_job_ = nullptr;

    //--------------------------------------------------------------------------
    void ExecutionContext::SetCurrentJob(const Job* job)
    {
      current_job_ = job;
    }

    //--------------------------------------------------------------------------
    void ExecutionContext::AssertResourceAccessAllowed(
      const BaseResource& resource, 
      AccessType access)
    {
      PS_LOG_IF(
        !IsAllowed(resource, access), 
        Assert, 
        "Invalid access for resource %s: ", 
        resource.name()
      );
    }

    //--------------------------------------------------------------------------
    bool ExecutionContext::IsAllowed(
      const BaseResource& resource, 
      AccessType access)
    {
      // If we are not execution a job everything is allowed
      if (current_job_ == nullptr) 
      {
        return true;
      }

      // Look for a matching policy which allows for the access requested
      for (const ResourcePolicy& policy : current_job_->policies()) 
      {
        if (policy.resource.resource_id() != resource.resource_id()) 
        {
          continue;
        }

        // If the policy gives write access, or they match we the operation is allowed
        if (policy.access_type == AccessType::kWrite || policy.access_type == access) 
        {
          return true;
        }
      }

      // Couldn't find a matching policy
      return false;
    }
  }
}
