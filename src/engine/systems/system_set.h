#pragma once
#include "engine/systems/system.h"

#include <foundation/job/resource.h>
#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

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
      * @brief
      */
      template<typename TSystem>
      void Create();

      /**
      * @brief
      * @return (size_t)
      */
      size_t Count() const;

      /**
      * @brief
      * @param[in] (size_t)
      * @return (T&)
      */
      Base& operator[](size_t id);
      /**
      * @brief
      * @param[in] (size_t)
      * @return (T&)
      */
      Base& Get(size_t id);
      /**
      * @brief
      * @return (TSystem&)
      */
      template<typename TSystem>
      TSystem& Get();

      /**
      * @brief Perform an operation (defined by Base) on all internal systems
      * @param[in] (void (ISystemBase::* func)(SigArgs...)) func
      * @param[in] (Args&&...) args
      * @remarks SigArgs are the parameters as part of the actual function signature,
      *          while Args are those that are actually passed-in. To avoid incorrect
      *          deduction these are separated here.
      */
      template<typename... SigArgs, typename... Args>
      void Execute(void (ISystemBase::* func)(SigArgs...), Args&&... args)
      {
        for (Element& system : systems_)
        {
          (system.get()->*func)(std::forward<Args>(args)...);
        }
      }

    private:
      foundation::Vector<Element> systems_; //!< The unique collection of systems in the set

      /**
      * @brief triggers a static assert if the passed type is not derived from ISystemBase
      */
      template<typename TSystem>
      constexpr void VerifyParam() const
      {
        static_assert(std::is_base_of<ISystemBase, TSystem>::value, "TSystem must derive from ISystem");
      }

    };

    //-------------------------------------------------------------------------
    template<typename Base>
    SystemSet<Base>::SystemSet() :
      BaseResource("SystemSet")
    {
    };

    //-------------------------------------------------------------------------
    template<typename Base>
    template<typename TSystem>
    void SystemSet<Base>::Create()
    {
      VerifyParam<TSystem>();

      if (systems_.size() == 0)
      {
        systems_.reserve(10);
      }

      systems_.emplace_back(
        foundation::Memory::ConstructShared<TSystem>());
    }

    //-------------------------------------------------------------------------
    template<typename Base>
    inline size_t SystemSet<Base>::Count() const
    {
      return systems_.size();
    }

    //-------------------------------------------------------------------------
    template<typename Base>
    inline Base& SystemSet<Base>::operator[](size_t id)
    {
      return *systems_[id].get();
    }
    template<typename Base>
    inline Base& SystemSet<Base>::Get(size_t id)
    {
      return *systems_[id].get();
    }
    template<typename Base>
    template<typename TSystem>
    TSystem& SystemSet<Base>::Get()
    {
      VerifyParam<TSystem>();
      return static_cast<TSystem&>(*systems_[TSystem::type_id()].get());
    }
  }
}
