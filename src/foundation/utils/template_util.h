#pragma once
#include "EASTL/type_traits.h"

namespace sulphur
{
  namespace foundation
  {
    template<typename... Args>
    struct and_cond : eastl::true_type {};

    template<typename Cond, typename... Rest>
    struct and_cond<Cond, Rest...>
      : eastl::conditional<Cond::value, and_cond<Rest...>, eastl::false_type>::type {};

    /**
    * @struct sulphur::foundation::is_convertible_to_function_ptr
    * @brief Helper type to check if the functor object is convertible to a function pointer
    * @author Kenneth Buijssen
    * @tparam Functor (typename) The functor type to check
    * @tparam Callable (typename) The callable type, this gets resolved automatically
    *
    * @details
    * The way this works is that we inherit from the same type however replace
    * the callable with a reference to its operator. This has as an effect that
    * the second definition will be a better match for lamdas. Which allows us
    * to use template argument deduction to extract the lamdas internal state,
    * return type and arguments.
    */
    template <typename Functor, typename Callable = Functor>
    struct is_convertible_to_function_ptr : is_convertible_to_function_ptr<Functor, decltype(&Callable::operator())>
    {};


    /**
    * @struct sulphur::foundation::is_convertible_to_function_ptr
    * @brief Helper type to check if the functor object is convertible to a function pointer
    * @author Kenneth Buijssen
    * @tparam Functor (typename) The functor type to check
    * @tparam LambdaState (typename) The type for the internal lambda state
    * @tparam ReturnType (typename) The return tyep of the lambda
    * @tparam Args (typename...) The types of the arguments the lamba accepts
    */
    template <typename Functor, typename LambdaState, typename ReturnType, typename... Args>
    struct is_convertible_to_function_ptr<Functor, ReturnType(LambdaState::*)(Args...) const>
    {
      /**
      * @brief value indicating if the Function is convertible to a function pointer
      */
      static const bool value = eastl::is_convertible<Functor, ReturnType(*)(Args...)>::value;
    };

    /**
    * @struct sulphur::foundation::is_non_capturing_lambda
    * @brief Helper type for checking if a lambda is capturing
    * @author Kenneth Buijssen
    * @tparam Lambda (typename) The lambda to check
    */
    template<typename Lambda>
    struct is_non_capturing_lambda
    {
      static const bool value = foundation::is_convertible_to_function_ptr<Lambda>::value;
    };

    /**
    * @struct sulphur::foundation::index_type
    * @brief Helper to access variadic types with an index
    * @tparam Index (size_t) The functor type to check
    * @tparam T (typename) First type
    * @tparam T (typename) Variadic types
    * @author Raymi Klingers
    */
    template<size_t Index, typename T, typename... Types>
    struct index_type
    {
      using type = typename index_type<Index - 1, Types...>::type;
    };
    /**
    * @struct sulphur::foundation::index_type
    * @brief Helper specialization to get the first type
    * @tparam T (typename) First type
    * @tparam T (typename) Unused variadic types
    * @author Raymi Klingers
    */
    template<typename T, typename... Types>
    struct index_type<0, T, Types...>
    {
      using type = T;
    };

    template<int Index, typename... Types>
    using indexed_type = typename index_type<Index, Types...>::type;//!< Helper alias for convenience
    
    /**
    * @struct sulphur::foundation::index_of_type
    * @brief Helper specialization 
    * @tparam T (typename) First type
    * @tparam T (typename...) Following types
    * @author Raymi Klingers
    */
    template <typename T, typename... Ts>
    struct index_of_type;
    /**
    * @struct sulphur::foundation::index_of_type
    * @brief Base specialization
    * @tparam T (typename) Type to obtain the index from
    * @tparam T (typename...) Following types
    * @author Raymi Klingers
    */
    template <typename T, typename... Ts>
    struct index_of_type<T, T, Ts...> : eastl::integral_constant<std::size_t, 0> {};
    /**
    * @struct sulphur::foundation::index_of_type
    * @brief Helper specialization
    * @tparam T (typename) First type
    * @tparam T (typename...) Following types
    * @author Raymi Klingers
    */
    template <typename T, typename U, typename... Ts>
    struct index_of_type<T, U, Ts...> : eastl::integral_constant<std::size_t, 1 + index_of_type<T, Ts...>::value> {};

    template <typename T, typename... Ts>
    constexpr std::size_t Index_v = index_of_type<T, Ts...>::value;//<! Create templated variable alias for convenience


  }
}