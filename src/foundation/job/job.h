#pragma once
#include "foundation/containers/string.h"
#include "foundation/job/task.h"
#include "foundation/utils/template_util.h"
#include "foundation/job/resource.h"
#include "foundation/containers/vector.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @class sulphur::foundation::Job
     * @brief Definition of an executable job
     * @author Kenneth Buijssen
     */
    class Job
    {
      template<typename Function>
      Job friend make_job(String name, String parent, Function func);

      template<typename Function, typename ...DataPolicy>
      Job friend make_job(String name, String parent, Function func, DataPolicy... data_policy);

    public:
      /**
       * @see name_
       */
      String name() const;

      /**
      * @see name_
      */
      void set_name(const String& name);

      /**
      * @see parent_
      */
      String parent() const;

      /**
      * @see parent_
      */
      String blocker() const;

      /**
      * @see parent_
      */
      void set_blocker(const String& blocker);
      
      /**
      * @see task_
      */
      Task task() const;

      /**
       * @brief Add a policy to the job, marking a resource as being used
       * @param[in] policy (sulphur::foundation::ResourcePolicy)
       *            The resource policy to add to the job
       */
      void AddPolicy(ResourcePolicy policy);
      
      /**
       * @see policies_
       */
      const Vector<ResourcePolicy>& policies() const;

      /**
       * @brief Check if two jobs have overlapping resources
       * @param[in] a (const sulphur::foundation::Job&)
       *            The first job to check overlap against
       * @param[in] b (const sulphur::foundation::Job&)
       *            The second job to check overlap against
       * @return (bool) True if the jobs have overlapping resources
       */
      static bool AreOverlapping(const Job& a, const Job& b);

    private:
      /**
       * @brief Constructor of the job
       * @param[in] name (const sulphur::foundation::String&) Name used to identify the job
       * @param[in] parent (const sulphur::foundation::String&) Name used to identify the parent job
       * @param[in] task (const sulphur::foundation::Task&) The task to execute
       */
      Job(const String& name, const String& parent, const Task& task);

    private:
      String name_;     //!< The name of the job
      String parent_;   //!< The name of the parent job
      String blocker_;  //!< The name of the blocker
      Task task_;       //!< The name of the task the job will execute

      Vector<ResourcePolicy> policies_; //!< List of access policies for the job
    };

    namespace detail
    {
      /**
       * @brief Helper function for recursivly adding policies to a job
       * @tparam DataPolicy (typename)
       *         The data policy to add to the job
       * @param  job (sulphur::foundation::Job&)
       *         The job the policy will be added to
       * @param  data_policy (DataPolicy)
       *         The data policy to add to the job
       */
      template<typename DataPolicy>
      void add_data_policy_to_job(Job& job, DataPolicy data_policy)
      {
        job.AddPolicy(ResourcePolicy{ data_policy.resource(), data_policy.access() });
      }

      /**
      * @brief Helper function for recursivly adding policies to a job
      * @tparam DataPolicy (typename)
      *         The data policy to add to the job
      * @tparam OtherPolicies (typename...)
      *         Parameter pack containting the other data policies which are 
      *         yet to be unpacked.
      * @param  job (sulphur::foundation::Job&)
      *         The job the policy will be added to
      * @param  data_policy (DataPolicy)
      *         The data policy to add to the job
      * @param  other_policies (OtherPolicies...)
      *         Param pack of other policies to add to the job, these are all
      *         added automatically.
      */
      template<typename DataPolicy, typename... OtherPolicies>
      void add_data_policy_to_job(Job& job, DataPolicy data_policy, OtherPolicies... other_policies)
      {
        job.AddPolicy(ResourcePolicy{ data_policy.resource(), data_policy.access() });

        // Use recursion to ensure that all policies are added
        add_data_policy_to_job(job, other_policies...);
      }
    }

    /**
    * @brief Helper function for creating jobs
    * @tparam Function (typename) Type of the function to bind to the job
    * @param[in] name (sulphur::foundation::String) Name used to identify the job
    * @param[in] parent (sulphur::foundation::String) Name used to identify the parent job
    * @param[in] func (Function) The function the job is responsible for executing
    * @return (sulphur::foundation::Job) The newly created job
    */
    template<typename Function>
    Job make_job(String name, String parent, Function func)
    {
      static_assert(is_non_capturing_lambda<Function>::value,
                "Job function must be a non capturing lambda");

      return Job(name, parent, Task(std::bind(func)));
    }

    /**
    * @brief Helper function for creating jobs
    * @tparam Function (typename) Type of the function to bind to the job
    * @tparam DataPolicy (typename...) Vararg type of the DataPolicies to bind to the job
    * @param[in] name (sulphur::foundation::String) Name used to identify the job
    * @param[in] parent (sulphur::foundation::String) Name used to identify the parent job
    * @param[in] func (Function) The function the job is responsible for executing
    * @param[in] data_policy (sulphur::foundation::DataPolicy...)
    *            The data policies for the func paramaters
    * @return (sulphur::foundation::Job) The newly created job
    */
    template<typename Function, typename ...DataPolicy>
    Job make_job(String name, String parent, Function func, DataPolicy... data_policy)
    {
      static_assert(is_non_capturing_lambda<Function>::value,
                "Job function must be a non capturing lambda");

      Job job(name, parent, Task(std::bind(func, std::ref(data_policy.value())...)));
      
      // Add the resource policies to the job, 
      // this is used for validation during debug
      detail::add_data_policy_to_job(job, data_policy...);

      return job;
    }
  }
}
