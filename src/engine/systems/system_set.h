#pragma once
#include "engine/systems/system.h"

#include <foundation/job/resource.h>
#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>
#include <foundation/utils/macro.h>
#include <foundation/logging/logger.h>

#include <type_traits>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::SystemSet <Base>
    * @brief A container used for creating and iterating over a unique set of systems that share a common base class
    * @remarks the passed class (Base) bas to be derived from ISystemBase
    * @author Maarten ten Velden
    */
    template<typename Base>
    class SystemSet : public foundation::BaseResource
    {
      static_assert(std::is_base_of<ISystemBase, Base>::value, "Base must be derived from ISystemBase");

    public:
      using Element = foundation::SharedPointer<Base>; //!< The type of element stored by the set
      
      /**
      * @brief Default constructor
      */
      SystemSet();

      /**
      * @brief Create a new system of type TSystem in-place
      * @tparam[in] TSystem (typename) The type of the new system (must be unique compared to anything currently in the set)
      * @tparam[in] Args (typename...) The types of arguments to pass to the system constructor
      * @param[in] args (Args&&...) The values of the arguments to pass to the system constructor
      */
      template<typename TSystem, typename... Args>
      void Create(Args&&... args);

      /**
      * @brief Get the amount of unique systems in the set
      * @return (size_t) The unsigned amount
      */
      size_t Count() const;

      /**
      * @brief Get a system by type-id
      * @param[in] (size_t) The type-id of the system, exposed through sulphur::foundation::TypeSet
      * @return (Base&) The system with that type-id (asserts if not found)
      */
      Base& Get(size_t id);
      /**
      * @brief Get a system by type
      * @tparam[in] The type of the system, which must inherit from sulphur::foundation::TypeSet
      * @return (TSystem&) The system with that type (asserts if not found)
      */
      template<typename TSystem>
      TSystem& Get();

      /**
      * @brief Get a system by type-id
      * @param[in] (size_t) The type-id of the system, exposed through sulphur::foundation::TypeSet
      * @see sulphur::foundation::TypeSet
      * @return (Base&) The system with that type-id (asserts if not found)
      */
      Base& operator[](size_t id);

      /**
      * @brief Perform an operation (that is part of the public interface of Base) on all internal systems
      * @tparam[in] SigArgs (typename...) The argument types of the target function's signature
      * @tparam[in] Args (typename...) The argument types that are passed in
      * @param[in] func (void (Base::* func)(SigArgs...)) A member-function pointer to the target function
      * @param[in] args (Args&&...) The parameters to pass into the target function
      * @remarks SigArgs are the parameters as part of the actual function signature,
      *          while Args are those that are actually passed-in. To avoid incorrect
      *          deduction these are separated here.
      */
      template<typename... SigArgs, typename... Args, typename TBase = Base>
      void Execute(void (TBase::* func)(SigArgs...), Args&&... args)
      {
        static_assert(eastl::is_base_of<TBase, Base>::value, "Base must be derived from Base");

        using Iterator = typename foundation::Vector<Element>::value_type;
        for (Iterator& it : systems_)
        {
          ((*it).*func)(eastl::forward<Args>(args)...);
        }
      }

    private:
      /**
      * @brief triggers a static assert if TSystem is not a supported type
      * @tparam[in] TSystem (typename) the type of the system
      */
      template<typename TSystem>
      void ValidateParam()
      {
        static_assert(std::is_abstract<TSystem>::value == false, "TSystem may not be abstract");
        static_assert(std::is_base_of<Base, TSystem>::value, "TSystem must derive from Base");
      }

      foundation::Map<foundation::typeid_t, size_t> systems_lookup_; //!< Lookup table from type_id to system_ vector index
      foundation::Vector<Element> systems_; //!< The collection of systems in insertion order
    };

    //-------------------------------------------------------------------------
    template<typename Base>
    SystemSet<Base>::SystemSet() :
      BaseResource("SystemSet")
    {
    };

    //-------------------------------------------------------------------------
    template<typename Base>
    template<typename TSystem, typename... Args>
    void SystemSet<Base>::Create(Args&&... args)
    {
      ValidateParam<TSystem>();
      static_assert(eastl::is_constructible<TSystem, Args...>::value,
        "TSystem is not constructible from Args. Are you passig the correct parameters to the constructor?");

      // Construction must happen first so TSystem's type-id is valid
      foundation::SharedPointer<TSystem> data = foundation::Memory::ConstructShared<TSystem>(std::forward<Args>(args)...);

      using Iterator = typename foundation::Map<foundation::typeid_t, size_t>::iterator;
      Iterator it = systems_lookup_.find(foundation::type_id<TSystem>());

      PS_UNUSED_IN_RELEASE(it);
      PS_LOG_IF(it != systems_lookup_.end(), Fatal, "Attempted to create a duplicate system");

      // Add ID to lookup table
      systems_lookup_.emplace(eastl::make_pair(foundation::type_id<TSystem>(), systems_.size()));

      // Add system data
      systems_.emplace_back(data);
    }

    //-------------------------------------------------------------------------
    template<typename Base>
    inline size_t SystemSet<Base>::Count() const
    {
      return systems_.size();
    }

    //-------------------------------------------------------------------------
    template<typename Base>
    inline Base& SystemSet<Base>::Get(size_t id)
    {
      using Iterator = typename foundation::Map<foundation::typeid_t, size_t>::iterator;
      Iterator it = systems_lookup_.find(id);
      
      PS_UNUSED_IN_RELEASE(it);
      PS_LOG_IF(it == systems_lookup_.end(), Fatal, "System with the specified id does not exist");

      return *(systems_[it->second]);
    }
    template<typename Base>
    template<typename TSystem>
    TSystem& SystemSet<Base>::Get()
    {
      ValidateParam<TSystem>();
      return static_cast<TSystem&>(Get(foundation::type_id<TSystem>()));
    }

    template<typename Base>
    inline Base& SystemSet<Base>::operator[](size_t id)
    {
      return Get(id);
    }
  }
}
