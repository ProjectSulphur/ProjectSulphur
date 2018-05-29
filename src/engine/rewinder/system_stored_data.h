#pragma once
#include <foundation/utils/template_util.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    class RewindStorageBase;
    struct FrameStorage;

    using StoreFunctionPointer = void*(*)(void*, size_t);//<! Alias for the stored function ptrs
    using RestoreFunctionPointer = void(*)(void*, void*, size_t);//<! Alias for the stored function ptrs

    /*
    * @brief Storage function
    * @tparam T (typename) The type to specialize on
    * @param[in] buffer (T*) A buffer of the type to store
    * @param[in] size (size_t) The num elements in the buffer
    */
    template<typename T>
    inline void* Store(T* buffer, size_t size);
    /*
    * @brief Restoring function
    * @tparam T (typename) The type to specialize on
    * @param[in] buffer (T*) A buffer to restore
    * @param[in] size (size_t) The num elements in the buffer
    */
    template<typename T>
    inline void Restore(T* buffer,void* old, size_t size);


    /*
    * @struct sulphur::engine::rewinder_typed_function
    * @brief A helper struct to combine function ptrs to store and restore and their type
    * @author Raymi Klingers
    */
    template<typename T, void*(*StoreFunc)(T*, size_t), void(*RestoreFunc)(T*,void*, size_t)>
    struct rewinder_typed_function
    {
      using Type = T;
      inline static void* Store(void* ptr, size_t size)
      {
        return eastl::move(StoreFunc(reinterpret_cast<T*>(ptr), size));
      }
      inline static void Restore(void* ptr, void* old, size_t size)
      {
        return RestoreFunc(reinterpret_cast<T*>(ptr),old, size);
      }
      template<typename T2>
      void CheckType()
      {
        static_assert(eastl::is_same_v<T, T2>, "Function type does not match container element type. Order of typed functions might be messed up or wrong typed function is passed");
      }
    };

    template<typename T>
    using StoreFunc = rewinder_typed_function<T, Store, Restore>;//<! Alias of the function ptrs pairs

    /**
    * @class sulphur::engine::RewindStorage
    * @brief System to obtain the storage from the containers
    * @author Raymi Klingers
    */
    class RewindStorage
    {
    public:

      /*
      * @brief Constructor which is not used currently
      * @todo Make this work for the system data
      */
      template<typename T, typename ...Funcs>
      RewindStorage(T& system_data, Funcs&&... store_functions)
        :
        storage_data_(&system_data)
      {
        static_assert(T::ElementCount == sizeof...(Funcs), "Not every element and buffer has a storage function");
        TypeCheckedAdd<0,T, Funcs...>(eastl::forward<Funcs>(store_functions)...);
        // Add system data storage functions
        AddSystemStorageFunction();
      }
      /*
      * @brief Restoring function
      * @tparam Funcs (typename...) The functions pairs for storage and restoring
      * @param[in] storage_base (sulphur::engine::RewindStorageBase*) The storage container
      * @param[in] store_functions (Funcs&&...) The functions that it needs to use to store and restore the data
      */
      template<typename ...Funcs>
      RewindStorage(RewindStorageBase* storage_base, Funcs&&... store_functions)
        :
        storage_data_(storage_base)
      {
        AddFunction<0, Funcs...>(eastl::forward<Funcs>(store_functions)...);
      }
      /*
      * @brief Invokes all the store function ptrs with the container to store the data of a frame.
      * @return (FrameStorage) Returns the stored frame
      */
      FrameStorage Store();

      /*
      * @brief Invokes all the restore function ptrs with the container to restore the data of a frame.
      * @param[in] storage (const sulphur::engine::FrameStorage&) The frame to restore
      */
      void Restore(const FrameStorage& storage);

    private:
      /*
      * @brief Templated loop to add the variadic number of function ptrs
      * @tparam Index (size_t) Iterator
      * @tparam T (typename) The current function pair type
      * @tparam Args (typename...) The following function pair types
      * @param[in] typed_func (T&&) The current function pair
      * @param[in] args (Args&&...) The following function pairs
      */
      template<size_t Index, typename T, typename ... Args>
      void AddFunction(T&& typed_func, Args&& ...args)
      {
        AddFunction<Index>( eastl::forward<T>( typed_func ) );
        AddFunction<Index + 1>(eastl::forward<Args>(args)...);
      }
      /*
      * @brief Templated loop end
      * @tparam Index (size_t) Iterator
      * @tparam T (typename) The current function pair type
      * @param[in] typed_func (T&&) The current function pair
      */
      template<size_t Index, typename T>
      void AddFunction(T&& /*typed_func*/)
      {
        store_functions_.push_back(&T::Store);
        restore_functions_.push_back(&T::Restore);
      }
      /*
      * @brief Templated loop to add the variadic number of function ptrs and check the types with the system data types
      * @tparam Index (size_t) Iterator
      * @tparam SD (typename) The system data type for the type information
      * @tparam T (typename) The current function pair type
      * @tparam Args (typename...) The following function pair types
      * @param[in] typed_func (T&&) The current function pair
      * @param[in] args (Args&&...) The following function pairs
      */
      template<size_t Index, typename SD, typename T, typename ... Args>
      void TypeCheckedAdd( T&& typed_func, Args&& ...args )
      {
        AddFunction<Index>( eastl::forward<T>( typed_func ) );
        TypeCheckedAdd<Index + 1, SD, Args...>( eastl::forward<Args>( args )... );
      }
      /*
      * @brief Templated loop end
      * @tparam Index (size_t) Iterator
      * @tparam SD (typename) The system data type for the type information
      * @tparam T (typename) The current function pair type
      * @param[in] typed_func (T&&) The current function pair
      */
      template<size_t Index, typename SD, typename T>
      void TypeCheckedAdd(T&& typed_func)
      {
        AddFunction<Index>( eastl::forward<T>( typed_func ) );
      }

      /*
      * @brief Adds a specialized storage function for the system data container
      */
      void AddSystemStorageFunction();

      void** element_list_;//<! Local element list might also be deprecated
      RewindStorageBase* storage_data_;//<! A pointer to the storage data used to get the data info
      foundation::Vector<StoreFunctionPointer> store_functions_;//<! List of store funcitons
      foundation::Vector<RestoreFunctionPointer> restore_functions_;//<! List of restore functions
    };
  }
}