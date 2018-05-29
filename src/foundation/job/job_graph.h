#pragma once

#include "foundation/containers/string.h"
#include "foundation/containers/vector.h"
#include "foundation/containers/map.h"
#include "foundation/job/job.h"

namespace sulphur 
{
  namespace foundation 
  {
    class ThreadPool;

    /**
     * @class sulphur::foundation::JobGraph
     * @brief Definition of the job graph which will be executed by the engine
     * @author Kenneth Buijssen
     */
    class JobGraph
    {
    public:
      /**
       * @brief Constructor
       */
      JobGraph();

      /**
       * @brief Adds a new job to the job graph
       * @param[in] job (sulphur::foundation::Job&&) The job the add to the job graph
       */
      void Add(Job&& job);

    protected:
      /**
       * @brief Find the job definition for a job inside of the job graph
       * @param[in] name (const sulphur::foundation::String&) The name of the job
       * @return (sulphur::foundation::Job*)
       *         Pointer to the job if found, otherwise a nullptr
       */
      Job* FindJob(const String& name);

      /**
      * @brief Find the job definition for a job inside of the job graph
      * @param[in] name (const sulphur::foundation::String&) The name of the job
      * @return (const sulphur::foundation::Job*) 
       *         Pointer to the job if found, otherwise a nullptr
      */
      const Job* FindJob(const String& name) const;
              
    protected:
      Vector<Job> jobs_; //!< List of all the jobs in the job graph
    };

    /**
     * @class sulphur::foundation::JobGraphExt
     * @brief Extension of the job graph, exposing functionality which should 
     *        only be used internally.
     * @author Kenneth Buijssen
     */
    class JobGraphExt : public JobGraph
    {
    public:
      /**
       * @brief Submit a sub tree of the job graph to be executed by the thread pool
       * @param[in] job_name (const sulphur::foundation::String&) 
       *            The starting job to submit
       * @param[in] pool (sulphur::foundation::ThreadPool&) 
       *            The thread pool to submit the jobs to
       * @return (sulphur::foundation::TaskHandle)
       *         Handle for the task created on the job pool
       */
      TaskHandle SubmitSubTreeToPool(const String& job_name, ThreadPool& pool) const;

      /**
      * @brief Run the validation algorithm over the job graph
      * @return (bool) True if the job graph contains no data contention
      */
      bool Validate() const;

    private:
      /**
      * @brief Submit a specific job and all its children to the job pool
      * @param[in] job_name (const sulphur::foundation::String&) 
      *            The starting job to submit
      * @param[in] root (const sulphur::foundation::String&)
      *            The root of the subtree in which the job needs to be
      * @param[in] pool (sulphur::foundation::ThreadPool&) 
      *            The thread pool to submit the jobs to
      * @param[in] task_map (sulphur::foundation::Map<sulphur::foundation::String, 
      *                                               sulphur::foundation::TaskHandle>&)
      *            Mapping from job name to task handle to keep track of which 
      *            tasks have already been added.
      * @return (sulphur::foundation::TaskHandle)
      *         Handle for the task created on the job pool
      */
      TaskHandle SubmitJobToPool(const String& job_name,
                                 const String& root,
                                  ThreadPool& pool, 
                                  Map<String, TaskHandle>& task_map) const;

      /**
       * @brief Check if two jobs can run at the same time
       * @param[in] job (const Job&) The first job to check
       * @param[in] target (const Job&) The second job to check
       * @return (bool) True if the jobs can run at the same time
       */
      bool CanRunAtTheSameTime(const Job& job, const Job& target) const;

      /**
       * @brief Find all dependencies (blockers and parents) of the given job 
       *        and add the specified flag to its entry in the dependency buffer.
       *        Now entries will be created as required.
       * @param[in] job (const sulphur::foundation::Job&) 
       *            The job to find the dependencies for
       * @param[in] dependency_buffer 
       *            (sulphur::foundation::Map<sulphur::foundation::String, uint8_t>)
       *            Buffer containing the final dependencies
       * @param[in] mask (uint8_t) The mask to set in the dependency buffer
       */
      void FindDependenciesRecursively(
        const Job& job, 
        Map<String, uint8_t>& dependency_buffer,
        uint8_t mask) const;

      /**
       * @brief Check if the given job is in the subtree defined by the root
       * @param[in] job (const sulphur::foundation::Job&)
       *            The job for which to check if it is in the subtree
       * @param[in] root (const sulphur::foundation::String&)
       *            The name of the subtree root job
       * @return (bool) True if the job has the root as one of its parents
       */
      bool IsJobInSubTree(const Job& job, const String& root) const;
    };
  }
}
