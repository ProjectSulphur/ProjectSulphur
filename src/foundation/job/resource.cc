#include "foundation/job/resource.h"
#include "foundation/logging/logger.h"

namespace sulphur
{
  namespace foundation
  {
    uint BaseResource::next_free_resource_id_ = 0;

    //--------------------------------------------------------------------------
    const char* BaseResource::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    uint BaseResource::resource_id() const
    {
      return resource_id_;
    }

    //--------------------------------------------------------------------------
    const BaseResource* BaseResource::parent() const
    {
      return parent_;
    }

    //--------------------------------------------------------------------------
    BaseResource::BaseResource(const char* name) :
      name_(std::move(name)),
      resource_id_(++next_free_resource_id_),
      parent_(nullptr)
    {
      PS_LOG_IF(next_free_resource_id_ == 0, Fatal, "Max number of resources reached");
    }

    //--------------------------------------------------------------------------
    BaseResource::BaseResource(const char* name, const BaseResource& parent) :
      name_(std::move(name)),
      resource_id_(++next_free_resource_id_),
      parent_(&parent)
    {
      PS_LOG_IF(next_free_resource_id_ == 0, Fatal, "Max number of resources reached");
    }

    //--------------------------------------------------------------------------
    bool ResourcePolicy::AreOverlapping(const ResourcePolicy & a, const ResourcePolicy & b)
    {
      // If both resources are read only we have no write overlap
      if (a.access_type == AccessType::kRead && b.access_type == AccessType::kRead) return false;

      // When checking for overlap we check a and all of its parents against b
      // The reason for this is this way we know that there are no conflits with any 
      // parents. This does not have to be done with B, because these parents are not 
      // actually referenced

      // Check if a or one of its parents is the same resource as b
      // note that we delay returning so we can print all overlaps
      bool overlap_found = false;
      const BaseResource* res = &a.resource;
      while (res != nullptr)
      {
        // We found overlap
        if (res->resource_id() == b.resource.resource_id())
        {
          overlap_found = true;
          PS_LOG(Debug, "Found overlap between resource: %s (%s) and %s",
            a.resource.name(),
            res->name(),
            b.resource.name()
          )
        }

        res = res->parent();
      }

      return overlap_found;
    }
  }
}
