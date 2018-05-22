#include "foundation/job/job.h"

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    String Job::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    void Job::set_name(const String& name)
    {
      name_ = name;
      name_.make_lower();
    }

    //--------------------------------------------------------------------------
    String Job::parent() const
    {
      return parent_;
    }

    //--------------------------------------------------------------------------
    String Job::blocker() const
    {
      return blocker_;
    }

    //--------------------------------------------------------------------------
    void Job::set_blocker(const String& blocker)
    {
      blocker_ = blocker;
      blocker_.make_lower();
    }

    //--------------------------------------------------------------------------
    Task Job::task() const
    {
      return task_;
    }

    //--------------------------------------------------------------------------
    void Job::AddPolicy(ResourcePolicy policy)
    {
      policies_.push_back(policy);
    }

    const Vector<ResourcePolicy>& Job::policies() const
    {
      return policies_;
    }

    //--------------------------------------------------------------------------
    bool Job::AreOverlapping(const Job & a, const Job & b)
    {
      bool ret = false;

      for (const ResourcePolicy& pol_a : a.policies())
      {
        for (const ResourcePolicy& pol_b : b.policies())
        {
          ret |= ResourcePolicy::AreOverlapping(pol_a, pol_b);
        }
      }

      return ret;
    }

    //--------------------------------------------------------------------------
    Job::Job(const String& name, const String& parent, const Task& task) :
      name_(name),
      parent_(parent),
      blocker_(),
      task_(task)
    {
      name_.make_lower();
      parent_.make_lower();
      blocker_.make_lower();
    }
  }
}