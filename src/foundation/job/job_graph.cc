#include "foundation/job/job_graph.h"
#include "foundation/logging/logger.h"
#include "foundation/job/thread_pool.h"

#include <algorithm>

namespace sulphur
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------
    JobGraph::JobGraph()
    {
      // Create an empty root job
      jobs_.push_back(make_job("", "", []() {}));
    }

    //--------------------------------------------------------------------------
    void JobGraph::Add(Job&& job)
    {
      PS_LOG_IF(FindJob(job.name()) != nullptr, Assert, 
        "Duplicate submit of job with name %s", job.name().c_str()
      );

      jobs_.push_back(job);
    }

    //--------------------------------------------------------------------------
    Job* JobGraph::FindJob(const String& name)
    {
      using ItT = Vector<Job>::iterator;
      ItT it = std::find_if(jobs_.begin(), jobs_.end(), [&name](Job& job)
      {
        return name == job.name();
      });

      return it != jobs_.end() ? it : nullptr;
    }

    //--------------------------------------------------------------------------
    const Job* JobGraph::FindJob(const String& name) const
    {
      using ItT = Vector<Job>::const_iterator;
      ItT it = std::find_if(jobs_.begin(), jobs_.end(), [&name](const Job& job)
      {
        return name == job.name();
      });

      return it != jobs_.end() ? it : nullptr;
    }
  
    //--------------------------------------------------------------------------
    TaskHandle JobGraphExt::SubmitSubTreeToPool(const String& job_name, ThreadPool& pool) const
    {
      Map<String, TaskHandle> task_map;
      TaskHandle handle = SubmitJobToPool(job_name, pool, task_map);

      PS_LOG_IF(handle.id == 0, Error,
        "Could not submit sub tree of job %s: job not found", job_name.c_str());

      return handle;
    }

    //--------------------------------------------------------------------------
    bool JobGraphExt::Validate() const
    {
      bool valid = true;

      for (const Job& job : jobs_)
      {
        for (const Job& target : jobs_)
        {
          // Do not check against the root job
          if (job.name() == "" || target.name() == "")
          {
            continue;
          }

          // We are only interested in jobs that can run at the same time
          if (job.name() == target.name() || !CanRunAtTheSameTime(job, target))
          {
            continue;
          }

          // Check if the jobs overlap
          if (Job::AreOverlapping(job, target))
          {
            valid = false;

            PS_LOG(Debug, "Data contention detected between job %s and %s",
              job.name().c_str(),
              target.name().c_str()
            );
          }

        }
      }

      return valid;
    }

    //!-------------------------------------------------------------------------
    TaskHandle JobGraphExt::SubmitJobToPool(const String& name, 
                                            ThreadPool& pool, 
                                            Map<String, TaskHandle>& task_map) const
    {
      // If we ever encounter the root job skip adding it, this is because the 
      // root job is also used to mark the leaves of the job tree
      if (name == "")
      {
        return TaskHandle{0};
      }

      using It = Map<String, TaskHandle>::iterator;
      const Job* job = FindJob(name);

      if (job == nullptr)
      {
        PS_LOG(Warning, "Skip submitting job %s to job pool: no such job found", name.c_str());
        return TaskHandle{ 0 };
      }

      It job_it = task_map.find(name);
      if (job_it != task_map.end())
      {
        return job_it->second;
      }
      
      // Create a copy of the task to fill out and submit
      Task task = job->task();

      // Make sure that there are tasks for the parent and blocker and submit them
      if (job->parent() != "")
      {
        TaskHandle parent_handle = SubmitJobToPool(job->parent(), pool, task_map);
        PS_LOG_IF(parent_handle.id == 0, Assert, "Job %s has invalid parent %s", job->name().c_str(), job->parent().c_str());
        task.set_parent(parent_handle);
      }

      if (job->blocker() != "")
      {
        TaskHandle blocker_handle = SubmitJobToPool(job->blocker(), pool, task_map);
        PS_LOG_IF(blocker_handle.id == 0, Assert, "Job %s has invalid blocker %s", job->name().c_str(), job->parent().c_str());
        task.set_blocker(blocker_handle);
      }
      
      const TaskHandle task_handle = pool.Submit(std::move(task));
      task_map[name] = task_handle;

      // Make tasks for all children
      for (const Job& potential_child : jobs_)
      {
        if (potential_child.parent() == name)
        {
          SubmitJobToPool(potential_child.name(), pool, task_map);
        }
      }
      
      return task_handle;
    }

    //--------------------------------------------------------------------------
    void JobGraphExt::FindDependenciesRecursively(
      const Job& job, 
      Map<String, uint8_t>& dependency_buffer, 
      uint8_t mask) const
    {
      // Figure out all the jobs that have to run before this job

      // Stop infinit recursion
      if (job.name() == "") 
      {
        return;
      }
      dependency_buffer[job.name()] |= mask;

      // todo(Kenneth) Add check to skip searching sub trees multiple times
      // this is currently done to keep the code simple, in the future however 
      // we can skip over entire sub trees when we encounter a node which is 
      // already marked with the current mask

      // Optimization, if job already in buffer with correct mask, skip
      const Job* parent = FindJob(job.parent());
      const Job* blocker = FindJob(job.blocker());

      PS_LOG_IF(!parent, Fatal, "Could not find parent '%s' for '%s'",
        job.parent().c_str(), job.name().c_str());

      PS_LOG_IF(!blocker, Fatal, "Could not find blocker '%s' for '%s'",
        job.parent().c_str(), job.name().c_str());

      FindDependenciesRecursively(*parent,  dependency_buffer, mask);
      FindDependenciesRecursively(*blocker, dependency_buffer, mask);
    }

    //--------------------------------------------------------------------------
    bool JobGraphExt::CanRunAtTheSameTime(const Job& job, const Job& target) const
    {
      constexpr uint8_t job_mask = 0x01;
      constexpr uint8_t target_mask = 0x02;
      constexpr uint8_t overlap_mask = job_mask | target_mask;

      // If the job is the same as the target they can't run at the same time
      if (job.name() == target.name())
      {
        return false;
      }

      // We create a buffer to keep track of the dependencies of both jobs
      // this is done by setting a flag to indicate which job is dependent 
      // on the job. We then check if we can find any dependencies which are 
      // shared by checking for the combined mask. If this is the case we know 
      // that jobs cannot overlap.
      Map<String, uint8_t> dependency_buffer;
      dependency_buffer[job.name()]    |= job_mask;
      dependency_buffer[target.name()] |= target_mask;

      FindDependenciesRecursively(job, dependency_buffer, job_mask);
      FindDependenciesRecursively(target, dependency_buffer, target_mask);
      
      // Search through the dependency buffer to see if there is any overlap
      bool overlap_found = false;
      for (auto& element : dependency_buffer)
      {
        if (element.second == overlap_mask) overlap_found = true;
      }

      return !overlap_found;
    }
  }
}
