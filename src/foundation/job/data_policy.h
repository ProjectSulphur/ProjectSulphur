#pragma once

#include "foundation/job/resource.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @struct sulphur::foundation::DataPolicy
     * @brief Data policies are used to markup how jobs reference the engine
     * @tparam ResourceType (typename)
     *         The type of resource referenced by the data policy
     * @tparam ValueType (typename)
     *         The value type contained inside of the resource
     * @author Kenneth Buijssen
     */
    template<typename ResourceType, typename ValueType>
    class DataPolicy
    {
    public:
      using resource_type = ResourceType; //!< The type of the resource
      using value_type = ValueType;       //!< The type of the value inside the resource

      /**
       * @brief DataPolicy constructor
       * @param[in] resource (resource_type&) 
       *            The resource for which the data policy is created
       * @param[in] value (value_type&)
       *            The value contained in the resource
       * @param[in] access (AccessType)
       *            The type of access the policy specifies
       */
      DataPolicy(resource_type& resource, value_type& value, AccessType access) :
        resource_(resource),
        value_(value),
        access_(access)
      {

      }

      /**
       * @see resource_
       */
      const resource_type& resource() const
      {
        return resource_;
      }

      /**
       * @see access_
       */
      AccessType access() const
      {
        return access_;
      }

    protected:
      resource_type& resource_; //!< The resource referenced by the data policy
      value_type& value_;       //!< The value contained in the specified resource
      AccessType access_;       //!< The access type defined by the data policy
    };

    /**
    * @struct sulphur::foundation::ReadDataPolicy
    * @brief Data policies are used to markup how jobs reference the engine
    * @tparam ResourceType (typename)
    *         The type of resource referenced by the data policy
    * @tparam ValueType (typename)
    *         The value type contained inside of the resource
    * @author Kenneth Buijssen
    */
    template<typename ResourceType, typename ValueType>
    class ReadDataPolicy : public DataPolicy<ResourceType, ValueType>
    {
    public:
      /**
      * @brief DataPolicy constructor
      * @param[in] resource (ResourceType&)
      *            The resource for which the data policy is created
      * @param[in] value (ValueType&)
      *            The value contained in the resource
      */
      ReadDataPolicy(ResourceType& resource, ValueType& value) :
        DataPolicy<ResourceType, ValueType>(resource, value, AccessType::kRead)
      {
      }

      /**
       * @see value_
       */
      const ValueType& value() const
      {
        return this->value_;
      }
    };

    /**
    * @struct sulphur::foundation::WriteDataPolicy
    * @brief Data policies are used to markup how jobs reference the engine
    * @tparam ResourceType (typename)
    *         The type of resource referenced by the data policy
    * @tparam ValueType (typename)
    *         The value type contained inside of the resource
    * @author Kenneth Buijssen
    */
    template<typename ResourceType, typename ValueType>
    class WriteDataPolicy : public DataPolicy<ResourceType, ValueType>
    {
    public:
      /**
      * @brief DataPolicy constructor
      * @param[in] resource (ResourceType&)
      *            The resource for which the data policy is created
      * @param[in] value (ValueType&)
      *            The value contained in the resource
      */
      WriteDataPolicy(ResourceType& resource, ValueType& value) :
        DataPolicy<ResourceType, ValueType>(resource, value, AccessType::kWrite)
      {
      }

      /**
      * @see value_
      */
      ValueType& value()
      {
        return this->value_;
      }
    };

    /**
    * @brief Helper function for creating a ReadDataPolicy
    * @tparam T (typename) The type for which to create a data policy
    * @tparam std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0 
    *         (typename)
    *         Template meta programming type to only enable this implementation 
    *         if T is a ResourceWrapper
    * @param[in] resource (T&) The resource for which to create the policy
    * @return (sulphur::foundation::ReadDataPolicy<T, T::value_type>)
    *         The newly create data policy
    * @author Kenneth Buijssen
    */
    template<typename T, typename std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0>
    ReadDataPolicy<T, typename T::value_type> bind_read(T& resource)
    {
      static_assert(std::is_base_of<BaseResource, T>::value, "Only resources can be bound");
      return ReadDataPolicy<T, typename T::value_type>(resource, *resource);
    }

    /**
    * @brief Helper function for creating a ReadDataPolicy
    * @tparam T (typename) The type for which to create a data policy
    * @tparam std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0 
    *         (typename)
    *         Template meta programming type to only enable this implementation 
    *         if T is not a ResourceWrapper
    * @param[in] resource (T&) The resource for which to create the policy
    * @return (sulphur::foundation::ReadDataPolicy<BaseResource, T>)
    *         The newly create data policy
    * @author Kenneth Buijssen
    */
    template <typename T, typename std::enable_if<!std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0>
    ReadDataPolicy<BaseResource, T> bind_read(T& resource)
    {
      static_assert(std::is_base_of<BaseResource, T>::value, "Only resources can be bound");
      return ReadDataPolicy<BaseResource, T>(resource, resource);
    }

    /**
    * @brief Helper function for creating a WriteDataPolicy
    * @tparam T (typename) The type for which to create a data policy
    * @tparam std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0 
    *         (typename)
    *         Template meta programming type to only enable this implementation 
    *         if T is a ResourceWrapper
    * @param[in] resource (T&) The resource for which to create the policy
    * @return (sulphur::foundation::WriteDataPolicy<T, T::value_type>) 
    *         The newly create data policy
    * @author Kenneth Buijssen
    */
    template<typename T, typename std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0>
    WriteDataPolicy<T, typename T::value_type> bind_write(T& resource)
    {
      static_assert(std::is_base_of<BaseResource, T>::value, "Only resources can be bound");
      return WriteDataPolicy<T, typename T::value_type>(resource, *resource);
    }

    /**
    * @brief Helper function for creating a WriteDataPolicy
    * @tparam T (typename) The type for which to create a data policy
    * @tparam std::enable_if<std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0 
    *         (typename)
    *         Template meta programming type to only enable this implementation 
    *         if T is not a ResourceWrapper
    * @param[in] resource (T&) The resource for which to create the policy
    * @return (sulphur::foundation::WriteDataPolicy<BaseResource, T>) 
    *         The newly create data policy
    * @author Kenneth Buijssen
    */
    template<typename T, typename std::enable_if< !std::is_base_of<ResourceWrapperHelper, T>::value, bool>::type = 0>
    WriteDataPolicy<BaseResource, T> bind_write(T& resource)
    {
      static_assert(std::is_base_of<BaseResource, T>::value, "Only resources can be bound");
      return WriteDataPolicy<BaseResource, T>(resource, resource);
    }
  }
}