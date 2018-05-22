#pragma once
#include "foundation/utils/type_definitions.h"
#include "foundation/containers/string.h"
#include "foundation/job/access_type.h"
#include "foundation/job/execution_context.h"

namespace sulphur 
{
  namespace foundation 
  {
    class BaseResource;

    /**
     * @struct sulphur::foundation::ResourcePolicy
     * @brief Helper struct for passing along resources together with their 
     *        access type.
     * @author Kenneth Buijssen
     */
    struct ResourcePolicy
    {
      const BaseResource& resource; //!< The resource the policy refers to
      AccessType access_type;       //!< The access type for the resource

      /**
       * @brief Check if two policies refer to overlapping and conflicting memory
       * @param[in] a (const ResourcePolicy&) The first resource to check
       * @param[in] b (const ResourcePolicy&) The second resource to check
       * @return True if the resouces overlap and access patterns conflict.
       */
      static bool AreOverlapping(const ResourcePolicy& a, const ResourcePolicy& b);
    };

    /**
     * @struct sulphur::foundation::BaseResource
     * @brief Base class for defining resources to be used by the job system and 
     *        its validation.
     * @remark This is the low level interface and only recommended when dealing with
     *         nesting. In most cases the use of Resource<T> is preferred.
     * @author Kenneth Buijssen
     */
    class BaseResource
    {
    public:
      /**
       * @see name_
       */
      const char* name() const;

      /**
       * @see resource_id_
       */
      uint resource_id() const;

      /**
       * @see parent_
       */
      const BaseResource* parent() const;

    protected:
      /**
       * @brief BaseResource constructor
       * @param[in] name (const char*)
       *            The name of the resource
       */
      explicit BaseResource(const char* name);

      /**
       * @brief BaseResource constructor
       * @param[in] name (const char*)
       *            The name of the resource
       * @param[in] parent (const sulphur::foundation::BaseResource&)
       *            The parent resource, used to keep track of nested resources
       */
      BaseResource(const char* name, const BaseResource& parent);

    protected:
      static uint next_free_resource_id_; //!< The id of the next resource that will be generated

      const char* name_;         //!< The name of the resource, used for debugging
      uint resource_id_;           //!< The unique runtime id of the resource
      const BaseResource* parent_; //!< Parent resource, nullptr if none
    };

    /**
     * @struct sulphur::foundation::ResourceWrapperHelper
     * @brief Simpel helper struct to mark the Resource wrapper. This is used in 
     *        the implementation of bind_x to create the correct data policy.
     */
    struct ResourceWrapperHelper {};
    
    /**
     * @struct sulphur::foundation::Resource <type>
     * @brief A convencience wrapper, allowing for any type to be wrapped inside 
     *        a resource while automatically performing access checks.
     * @tparam type (typename) The type the resource wrapper encapsulates
     */
    template<typename type>
    class Resource : public BaseResource, ResourceWrapperHelper
    {
    public:
      using value_type = type; //!< The type referenced by the resource

    public:
      /**
       * @brief Resource constructor
       * @param[in] name (const char*)
       *            The name of the resource for debugging purposes
       */
      explicit Resource(const char*);
      
      /**
      * @brief Resource constructor
      * @param[in] name (const char*)
      *            The name of the resource for debugging purposes
      * @param[in] parent (const sulphur::foundation::BaseResource&)
      *            The parent resource, used for validating nested resources
      */
      Resource(const char* name, const BaseResource& parent);

      /**
      * @brief Resource constructor
      * @param[in] name (const char*)
      *            The name of the resource for debugging purposes
      * @param[in] val (value_type&&)
      *            The value to store in the resource, used for easy initialization
      */
      Resource(const char* name, value_type&& val);

      /**
      * @brief Resource constructor
      * @param[in] name (sulphur::foundation::String)
      *            The name of the resource for debugging purposes
      * @param[in] parent (const sulphur::foundation::BaseResource&)
      *            The parent resource, used for validating nested resources
      * @param[in] val (value_type&&)
      *            The value to store in the resource, used for easy initialization
      */
      Resource(const char* name, const BaseResource& parent, value_type&& val);

      /**
       * @brief Get a pointer to the wrapped object
       * @return (value_type*) A pointer to the wrapped object
       */
      value_type* Get();
      
      /**
      * @brief Get a const pointer to the wrapped object
      * @return (const value_type*) A const pointer to the wrapped object
      */
      const value_type* Get() const;

      /**
       * @brief Dereference operator, providing access to the wrapped type
       * @return (value_type&) The type wrapped by the resource
       */
      value_type& operator*();
      
      /**
      * @brief Dereference operator, providing access to the wrapped type
      * @return (const value_type&) The type wrapped by the resource
      */
      const value_type& operator*() const;

      /**
      * @brief Arrow operator, providing access to the wrapped type
      * @return (value_type*) The type wrapped by the resource
      */
      value_type* operator->();
      
      /**
      * @brief Arrow operator, providing access to the wrapped type
      * @return (const value_type*) The type wrapped by the resource
      */
      const value_type* operator->() const;

    private:
      value_type val_; // Instance of the type wrapped by the resource
    };

    //--------------------------------------------------------------------------
    template <typename value_type>
    Resource<value_type>::Resource(const char* name) :
      BaseResource(std::move(name)),
      val_{}
    {

    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    Resource<value_type>::Resource(const char* name, const BaseResource& parent) :
      BaseResource(std::move(name), parent),
      val_{}
    {
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    Resource<value_type>::Resource(const char* name, value_type&& val) :
      BaseResource(std::move(name)),
      val_{val}
    {
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    Resource<value_type>::Resource(const char* name, const BaseResource& parent, value_type&& val) :
      BaseResource(std::move(name), parent),
      val_{val}
    {
    }

    //--------------------------------------------------------------------------
    template <typename type>
    typename Resource<type>::value_type* Resource<type>::Get()
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kWrite);
      return &val_;
    }

    //--------------------------------------------------------------------------
    template <typename type>
    const typename Resource<type>::value_type* Resource<type>::Get() const
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kRead);
      return &val_;
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    value_type& Resource<value_type>::operator*()
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kWrite);
      return val_;
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    const value_type& Resource<value_type>::operator*() const
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kRead);
      return val_;
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    value_type* Resource<value_type>::operator->()
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kWrite);
      return &val_;
    }

    //--------------------------------------------------------------------------
    template <typename value_type>
    const value_type* Resource<value_type>::operator->() const
    {
      ExecutionContext::AssertResourceAccessAllowed(*this, AccessType::kRead);
      return &val_;
    }
  }
}
