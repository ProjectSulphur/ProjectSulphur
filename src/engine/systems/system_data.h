#pragma once

#include "engine/core/handle_base.h"

#include <foundation/utils/type_definitions.h>
#include <foundation/utils/template_util.h>
#include <foundation/memory/memory.h>
#include <foundation/containers/array.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/deque.h>
#include <foundation/logging/logger.h>

#include <EASTL/tuple.h>

namespace sulphur
{
  namespace engine
  {
    /*
    * @struct sulphur::engine::SparseHandle
    * @brief Strongly typed index to the sparse array
    * @todo Merge these with a template?
    * @author Raymi Klingers
    */
    struct SparseHandle
    {
      static constexpr size_t kInvalidHandle = PS_UINT_MAX;//!< An invalid index
      /*
      * @brief Creates a handle.
      * @param[in] handle (size_t) The index that the handle indexes to.
      * @todo This should only be used in the system data.
      * @return (sulphur::engine::SparseHandle) A handle with the given index.
      */
      explicit SparseHandle(size_t handle) :inner_handle(handle) {}
      /*
      * @brief Compares two handles
      * @param[in] other (const sulphur::engine::SparseHandle& other) The other handle to compare to.
      * @todo Figure out if this is still required
      * @return (bool) Are the handles identical?
      */
      bool operator==(const SparseHandle& other) const { return inner_handle == other.inner_handle; }

      size_t inner_handle;//!< The index   
    };
    /*
    * @struct sulphur::engine::DenseHandle
    * @brief Strongly typed index from a data index to its the sparse array index
    * @todo Merge these with a template?
    * @author Raymi Klingers
    */
    struct DenseHandle
    {
      static constexpr size_t kInvalidHandle = PS_UINT_MAX;//!< An invalid index
      /*
      * @brief Creates a handle.
      * @param[in] handle (size_t) The index that the handle indexes to.
      * @todo This should only be used in the system data.
      * @return (sulphur::engine::DenseHandle) A handle with the given index.
      */
      explicit DenseHandle(size_t handle) :to_sparse_handle(handle) {}

      /*
      * @brief Compares two handles
      * @param[in] other (const sulphur::engine::DenseHandle& other) The other handle to compare to.
      * @todo Figure out if this is still required
      * @return (bool) Are the handles identical?
      */
      bool operator==(const DenseHandle& other) const { return to_sparse_handle == other.to_sparse_handle; }

      size_t to_sparse_handle;//!< The index   
    };

    /*
    * @class sulphur::engine::ArrayPtr <T>
    * @brief A pointer to a buffer with it size.
    * @todo Move this into its own file
    * @author Raymi Klingers
    */
    template<typename T>
    class ArrayPtr
    {
    public:
      /*
      * @brief Array index operator
      * @param[in] index (size_t) The index.
      * @todo Bounds checking in debug
      * @return (T&) The element and the index.
      */
      T& operator[](size_t index) const
      {
        return buffer[index];
      }
      /*
      * @brief Sets the buffer and the size of it.
      * @param[in] ptr (T*) The buffer.
      * @param[in] new_size (size_t) The size of the buffer.
      * @todo Only allow ptr to be nullptr if new_size == 0
      */
      void set_buffer(T* ptr, size_t new_size)
      {
        capacity_ = new_size;
        buffer = ptr;
      }
      /*
      * @brief Sets the size of the buffer.
      * @remarks Only use this if you store the actual size somewhere else.
      * @param[in] new_size (size_t) The size of the buffer.
      */
      void set_capacity(size_t new_size)
      {
        capacity_ = new_size;
      }
      /*
      * @brief Gets the size of the buffer
      */
      void get_capacity()
      {
        return capacity_;
      }
    public:
      T* buffer;//!< The buffer of the array
    protected:
      size_t capacity_;//!< The capacity of the buffer
    };
    template <typename T, typename... Ts>
    struct Index;

    template <typename T, typename... Ts>
    struct Index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

    template <typename T, typename U, typename... Ts>
    struct Index<T, U, Ts...> : std::integral_constant<std::size_t, 1 + Index<T, Ts...>::value> {}; 

    template <typename T, typename... Ts>
    constexpr std::size_t Index_v = Index<T, Ts...>::value;

    /*
    * @class sulphur::engine::SystemData <...Types>
    * @brief A vector like container that stores the types in SoA format and uses a slot map to access the elements. Currently tuned towards the entity systems so it should be templatized on the variables of a component
    * @remarks Assumes POD like elements.
    * @author Raymi Klingers
    */
    template<typename ... Types>
    class SystemData
    {
    private:
      constexpr const static int kReuseThreshold = 1024;//!< How many empty slots we need before we start reusing
    public:
      template<size_t Index>
      using element_type = foundation::indexed_type<Index, Types...>;//!< Alias to get an element at index

      constexpr const static int ElementCount = sizeof...(Types);//!< Number of elements

      /*
      * @brief Constructor of the systemdata that also updates a given pointer list for easy access to the internal data.
      * @param[in] element_list_ (void**) A list that will contain pointers to the element buffers for easy access
      * @return (sulphur::engine::SystemData) The system data container.
      */
      SystemData(void** element_list_ = nullptr) :
        size_(0),
        capacity_(0),
        buffer_(nullptr),
        element_list_(element_list_)
      {}

      /*
      * @brief Destructor of the systemdata that releases the data buffer
      */
      ~SystemData()
      {
        if (buffer_ != nullptr)
        {
          foundation::Memory::Deallocate(buffer_);
          buffer_ = nullptr;
        }
      }

      /*
      * @brief Fills a given struct with the elements at the given handle index.
      * @tparam T (typename) The user defined struct that will be used to store the data in.
      * @return T (typename) The struct with the data.
      */
      template<typename T>
      T GetObjectAsStruct(ComponentHandleBase handle)
      {
        return T{ ElementBuffer<Index_v<Types,Types...>>()[sparse_array_[handle.GetIndex()].inner_handle]... };
      }
      /*
      * @brief Gets a pointer to one of the element buffers
      * @tparam Index (size_t) Index of the element
      * @return (sulphur::engine::SystemData <typename...>::element_type <size_t>*) The element buffer
      */
      template<size_t Index>
      element_type<Index>* ElementBuffer() const
      {
        return eastl::get<Index>(elements_).buffer;
      }
      /*
      * @brief Gets the number of elements stored
      * @return (size_t) The number of elements stored
      */
      size_t size() const
      {
        return size_;
      }
      /*
      * @brief Gets the number of elements thast can be stored before a reallocation is needed
      * @return (size_t) The capacity size
      */
      size_t capacity() const
      {
        return capacity_;
      }
      /*
      * @brief Gets the data index from the handle
      * @param handle (sulphur::engine::ComponentHandleBase) A handle to obtain the data index from
      * @return (size_t) The data index
      */
      size_t GetDataIndex(ComponentHandleBase handle)
      {
        return sparse_array_[handle.GetIndex()].inner_handle;
      }
      /*
      * @brief Gets the data index from the handle
      * @param data_index (size_t) A data index to obtain the sparse handle from
      * @todo Look if the return type should be a sparse handle
      * @return (size_t) The data index
      */
      size_t GetSparseFromDataIndex(size_t data_index)
      {
        return dense_to_sparse_array_[data_index].to_sparse_handle;
      }
      /*
      * @brief Adds a new component to a free slot in the array.
      * @tparam args (const Types&...) All the elements required to construct the component with constant values.
      * @todo Grow factor could be changed to something better and less drastic.
      * @return (size_t) The index to the newly added component which can be used to construct the component type.
      */
      size_t Add(const Types&... args)
      {
        if (size_ == capacity_)
        {
          // Reallocation should be refactored.
          Reallocate(capacity_ * 2 + 1);
        }
        size_t ret;
        if (free_list_.size() >= kReuseThreshold)
        {
          ret = free_list_.front();
          free_list_.pop_front();
          dense_to_sparse_array_.emplace_back(ret);
        }
        else
        {
          generation_.push_back(0);
          ret = sparse_array_.size();
          dense_to_sparse_array_.emplace_back(sparse_array_.size());
          sparse_array_.emplace_back(size_);
        }
        ForEachElementVariadic<add_element>(args...);
        return (ret | ((size_t)generation_[ret] << ComponentHandleBase::kIndexBits));
      }
      /*
      * @brief Adds a new component to a free slot in the array.
      * @tparam args (MyTypes&&...) All the elements required to construct the component.
      * @todo Grow factor could be changed to something better and less drastic.
      * @return (size_t) The index to the newly added component which can be used to construct the component type.
      */
      template<typename ...MyTypes>
      size_t Add(MyTypes&&...args)
      {
        if (size_ == capacity_)
        {
          // Reallocation should be refactored.
          Reallocate(capacity_ * 2 + 1);
        }
        size_t ret;
        if (free_list_.size() >= kReuseThreshold)
        {
          ret = free_list_.front();
          free_list_.pop_front();
          dense_to_sparse_array_.emplace_back(ret);
        }
        else
        {
          generation_.push_back(0);
          ret = sparse_array_.size();
          dense_to_sparse_array_.emplace_back(sparse_array_.size());
          sparse_array_.emplace_back(size_);
        }

        ForEachElementVariadic<add_element>(eastl::forward<MyTypes>(args)...);
        return ret | ((size_t)generation_[ret] << ComponentHandleBase::kIndexBits);
      }
      /*
      * @brief Removes the component that the handle indexes to.
      * @param remove_index (ComponentHandleBase) A handle that indexes to the component to remove.
      */
      void Remove(ComponentHandleBase remove_index)
      {
        size_t index = remove_index.GetIndex();
        Remove(sparse_array_[index]);
        ++generation_[index];
        free_list_.emplace_back(index);
      }
      /*
      * @brief Get the component variable from an index and a handle.
      * @tparam Index (size_t) Index of the variable that needs to be accessed.
      * @param component_handle (sulphur::engine::ComponentHandleBase) A handle that indexes to the component to get the variable from.
      * @return (sulphur::engine::SystemData <typename...>::element_type <size_t>&) A reference to the variable of the component.
      */
      template<size_t Index>
      element_type<Index>& Get(ComponentHandleBase component_handle) const 
      {
        return eastl::get<Index>(elements_)
          .buffer[sparse_array_[component_handle.GetIndex()].inner_handle];
      }
      /*
      * @brief Checks if the handle is valid.
      * @param component_handle (sulphur::engine::ComponentHandleBase) The handle to be checked.
      * @return (bool) Is it valid?
      */
      bool IsValid(ComponentHandleBase component_handle) const
      {
        return generation_[component_handle.GetIndex()] == component_handle.GetGeneration();
      }
      /*
      * @brief Swaps the two components from two data indices
      * @param component0 (uint32_t) Component to swap with
      * @param component1 (uint32_t) Component to swap with
      * @todo Look at a way to make this less error prone. Like making a hardtyped data index type
      * @todo Add error checking
      */
      void Swap(uint32_t component0, uint32_t component1)
      {
        //assert(element0 < m_size, "First element index out of range in Swap function");
        //assert(element1 < m_size, "Second element index out of range in Swap function");
        ForEachElement<swap_element>(component0, component1);
        // Swap the sparse handles
        eastl::swap(
          sparse_array_[dense_to_sparse_array_[component0]],
          sparse_array_[dense_to_sparse_array_[component1]]);
        // Swap the dense handles
        eastl::swap(dense_to_sparse_array_[component0], dense_to_sparse_array_[component1]);
      }
      /*
      * @brief Removes the component with a sparse handle
      * @param index (sulphur::engine::SparseHandle) Remove a component with a sparse handle
      * @todo Position removing functions more logically.
      * @todo Error checking.
      */
      void Remove(SparseHandle index)
      {
        // Update the affected element position
        SparseHandle pos(size() - 1);
        // Remove it from sparse array
        sparse_array_[dense_to_sparse_array_[pos.inner_handle].to_sparse_handle] = index;
        // Move it in the dense to sparse array
        dense_to_sparse_array_[index.inner_handle] = dense_to_sparse_array_[pos.inner_handle];
        dense_to_sparse_array_.pop_back();
        // Remove the component data by swapping it to the end
        RemoveData(index.inner_handle);
      }
      /*
      * @brief Removes the component with a data index
      * @param index (size_t) Remove a component with a sparse handle
      * @todo Position removing functions more logically.
      * @todo Error checking.
      * @todo Data index hard type.
      */
      void RemoveData(size_t index)
      {
        //assert(index + 1 <= m_size, "Index out of range in Remove function");
        if (index < size_)
        {
          ForEachElement<replace_element_with_last>(index);
          return;
        }

        RemoveLast();
      }
      /*
      * @brief Removes the last component
      * @todo Position removing functions more logically.
      * @todo Error checking in case of 0 elements.
      */
      void RemoveLast()
      {
        ForEachElement<remove_last_element>();
      }
      /*
      * @brief Resizes the buffer and default constructs new elements
      * @param new_size (size_t) new size of the buffer
      */
      void Resize(size_t new_size)
      {
        if (new_size < size_)
        {
          size_t delta = size_ - new_size;
          for (size_t i = 0; i < delta; ++i)
          {
            RemoveLast();
          }
          return;
        }
        Reallocate(new_size);
        // Default construct new elements
        for (size_t i = size_; i < new_size; ++i)
        {
          ForEachElement<add_default_construct_element>(i);
        }
      }
      /*
      * @brief Clears the buffer
      */
      void Clear()
      {
        Resize(0);
      }

    private:
      /*
      * @struct sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Implements the End function for types that do not need it. Others will hide this
      * @author Raymi Klingers
      */
      struct for_each_element_method_base
      {
        /*
        * @brief Implements the End function for types that do not need it. Others will hide this
        * @param[in] (sulphur::engine::SystemData<Types...>*) Unused variable
        * @author Raymi Klingers
        */
        inline void End(SystemData<Types...>*) const {};
      };
      /*
      * @struct sulphur::foundation::SystemData::add_element : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Assigns an element after constructing a new slot.
      * @author Raymi Klingers
      */
      struct add_element : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively assign values to the component elements.
        * @tparam Index (size_t) Current index
        * @tparam T (typename) Current type
        * @tparam Args (typename...) Next types
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] arg (T&) The current argument that is going to be assigned.
        * @param[in]  (Args&&...) Next arguments
        */
        template<size_t Index, typename T, typename ... Args>
        inline void Iterate(SystemData<Types...>* this_ptr, T& arg, Args&&...) const
        {
          new (this_ptr->ElementBuffer<Index>() + this_ptr->size_) element_type<Index>;
          this_ptr->ElementBuffer<Index>()[this_ptr->size_] = arg;
        }
        /*
        * @brief Used to iteratively move assign values to the component elements.
        * @tparam Index (size_t) Current index
        * @tparam T (typename) Current type
        * @tparam Args (typename...) Next types
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] arg (T&) The current argument that is going to be assigned.
        * @param[in]  (Args&&...) Next arguments
        */
        template<size_t Index, typename T, typename ... Args>
        inline void Iterate(SystemData<Types...>* this_ptr, T&& arg, Args&&...) const
        {
          new (this_ptr->ElementBuffer<Index>() + this_ptr->size_) element_type<Index>;
          this_ptr->ElementBuffer<Index>()[this_ptr->size_] = eastl::move(arg);
        }
        /*
        * @brief Increments the size of the data as we just have added a component to it.
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) The container that this is being called on
        */
        inline void End(SystemData<Types...>* this_ptr) const
        {
          ++this_ptr->size_;
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::default_construct_element : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Defaults constructs elements
      * @author Raymi Klingers
      */
      struct add_default_construct_element : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively assign values to the component elements.
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] num (size_t) The number of elements to add.
        */
        template<size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr, size_t index) const
        {
          auto test = new (this_ptr->ElementBuffer<Index>() + index) element_type<Index>;
          test = test;
        }
        /*
        * @brief Increments the size of the data as we just have added a component to it.
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) The container that this is being called on
        */
        inline void End(SystemData<Types...>* this_ptr) const
        {
          ++this_ptr->size_;
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::replace_element_with_last : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Replaces an element with the last and deconstructs the replaced element.
      * @author Raymi Klingers
      */
      struct replace_element_with_last : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively assign values to the component elements.
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] index_to_replace (size_t) The index to replace.
        */
        template<size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr, size_t index_to_replace) const
        {
          using Type = element_type<Index>;
          ArrayPtr<Type>& elements = this_ptr->GetElements<Index>();
          elements.buffer[index_to_replace] = std::move( elements.buffer[this_ptr->size_ - 1] );
          elements.set_capacity(this_ptr->size_ - 1);
        }
        /*
        * @brief Removes the last index as we just moved it somewhere else.
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) The container that this is being called on
        */
        inline void End(SystemData<Types...>* this_ptr) const { this_ptr->RemoveLast(); }
      };
      /*
      * @struct sulphur::foundation::SystemData::remove_last_element : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Removes the last element.
      * @author Raymi Klingers
      */
      struct remove_last_element : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively call the destructor on the removed element.
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        */
        template<size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr) const
        {
          using Type = element_type<Index>;
          ArrayPtr<Type>& elements = this_ptr->GetElements<Index>();
          (static_cast<Type*>(elements.buffer + (this_ptr->size_ - 1)))->~Type();
          elements.set_capacity(this_ptr->size_ - 1);
        }
        /*
        * @brief Removes the last index as we just moved it somewhere else.
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) The container that this is being called on
        */
        inline void End(SystemData<Types...>* this_ptr) const 
        {
          --this_ptr->size_;
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::offset_element_buffer : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Replaces an element with the last.
      * @author Raymi Klingers
      */
      struct offset_element_buffer : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively fixup the buffer pointers
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] offset (void*) The current offset of the pointer from the buffer.
        * @todo Look at using a different type for offset.
        */
        template<size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr, void* offset) const
        {
          element_type<Index>*& elements = this_ptr->GetElement<Index>();
          elements = (element_type<Index>*)((size_t)elements + (size_t)offset);
          this_ptr->element_list_[Index] = elements;
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::calculate_element_buffer_size : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Calculates the buffer size by looking at all the alignment requirements of the element buffers.
      * @author Raymi Klingers
      */
      struct calculate_element_buffer_size : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively fixup the buffer pointers
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] size (size_t) The new size of the buffer
        * @param[in] buffer_size (size_t&) The total size of all element buffers
        */
        template<size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr, size_t size, size_t& buffer_size) const
        {
          this_ptr->GetElements<Index>()
            .set_buffer((element_type<Index>*)buffer_size, size);
          
          buffer_size += 
            AlignUp((const void*)(size * sizeof(element_type<Index>)), 64);
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::copy_over_element : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Calculates the buffer size by looking at all the alignment requirements of the element buffers.
      * @author Raymi Klingers
      */
      struct copy_over_element : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively copy over the elements buffers to the new buffer
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] old_elements (eastl::tuple<sulphur::engine::ArrayPtr<Types>...>) The old buffer to copy from
        */
        template<size_t Index>
        inline void Iterate(
          SystemData<Types...>* this_ptr,
          eastl::tuple<ArrayPtr<Types>...>old_elements)
        {
          for (int i = 0; i < this_ptr->size_; ++i)
          {
            new (&(this_ptr->ElementBuffer<Index>()[i])) 
              element_type<Index>(eastl::move(eastl::get<Index>(old_elements).buffer[i]));
          }
        }
      };
      /*
      * @struct sulphur::foundation::SystemData::swap_element : public sulphur::foundation::SystemData::for_each_element_method_base
      * @brief Used to iteratively swap elements of components
      * @author Raymi Klingers
      */
      struct swap_element : public for_each_element_method_base
      {
        /*
        * @brief Used to iteratively swap the elements of components
        * @tparam Index (size_t) Current index
        * @param[in] this_ptr (sulphur::engine::SystemData<Types...>*) A pointer to the system data on which this is called
        * @param[in] element0 (size_t) Element to swap
        * @param[in] element1 (size_t) Element to swap
        */
        template <size_t Index>
        inline void Iterate(SystemData<Types...>* this_ptr, size_t element0, size_t element1)
        {
          eastl::swap(this_ptr->ElementBuffer<Index>()[element0],
            this_ptr->ElementBuffer<Index>()[element1]);
        }
      };
      /*
      * @brief The starting function to iteratively execture a functor on each element buffer.
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] args...(Args&&) The arguments to iteratively pass to the functor
      */
      template <typename Method, typename ... Args>
      void ForEachElement(Args&&...args)
      {
        ForEachElement<0, Method, Args...>
          (eastl::bool_constant<(ElementCount != 0)>(), eastl::forward<Args>(args)...);
      }
      /*
      * @brief Used to iteratively execture a functor on each element buffer.
      * @tparam Index (typename) The current index
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] (eastl::true_type) Argument to specialize on, if the loop ends it will be false and it will pick the ending function
      * @param[in] args (Args&&...) The arguments to iteratively pass to the functor
      */
      template <size_t Index, typename Method, typename ... Args>
      void ForEachElement(eastl::true_type, Args&&... args)
      {
        Method().template Iterate<Index>(this, eastl::forward<Args>(args)...);
        ForEachElement<Index + 1, Method, Args...>
          (eastl::bool_constant<(Index + 1 < ElementCount)>(), eastl::forward<Args>(args)...);
      }
      /*
      * @brief Ends the foreach element loop
      * @tparam Index (typename) The current index
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] (eastl::false_type) Argument to specialize on
      * @param[in] (Args&&) Unused and likely should not contain any args.
      */
      template <size_t Index, typename Method, typename ... Args>
      void ForEachElement(eastl::false_type, Args&&...)
      {
        Method().End(this);
      }
      /*
      * @brief Start the foreach loop with variadic arguments that needs to be passed iteratively as well
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] args (Args&&...) Variadic arguments that need to be split up per iteration
      */
      template <typename Method, typename ... Args>
      void ForEachElementVariadic(Args&&...args)
      {
        ForEachElementVariadic<0, Method, Args...>(eastl::forward<Args>(args)...);
      }
      /*
      * @brief Body of the foreach loop with variadic arguments
      * @tparam Index (typename) The current index
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] arg  (T&&) The current argument
      * @param[in] args (Args&&...) Variadic arguments that need to be split up per iteration
      */
      template <size_t Index, typename Method, typename T, typename ... Args>
      void ForEachElementVariadic(T&& arg, Args&&... args)
      {
        Method().template Iterate<Index>(this, eastl::forward<T>(arg), eastl::forward<Args>(args)...);
        ForEachElementVariadic<Index + 1, Method, Args...>(eastl::forward<Args>(args)...);
      }
      /*
      * @brief End of the foreach loop with variadic arguments
      * @tparam Index (typename) The current index
      * @tparam Method (typename) The functor to use
      * @tparam Args (typename...) The args to pass to the functor
      * @param[in] arg  (T&&) The last argument
      */
      template <size_t Index, typename Method, typename T>
      void ForEachElementVariadic(T&& arg)
      {
        Method().template Iterate<Index>(this, eastl::forward<T>(arg));
        Method().End(this);
      }
      /*
      * @brief Gets a pointer to the first element in the array.
      * @tparam Index (typename) The element index
      * @return (sulphur::engine::SystemData <typename...>::element_type <size_t>*&)  Pointer to element array
      */
      template<size_t Index>
      element_type<Index>*& GetElement()
      {
        return eastl::get<Index>(elements_).buffer;
      }
      /*
      * @brief Gets the element array
      * @tparam Index (typename) The element index
      * @return (sulphur::engine::ArrayPtr <sulphur::engine::SystemData <typename...>::element_type <size_t>>&)  Pointer to element array
      */
      template<size_t Index>
      ArrayPtr<element_type<Index>>& GetElements()
      {
        return eastl::get<Index>(elements_);
      }
      private:
      /*
      * @brief Reallocates the memory buffer with the given size.
      * @param new_size (size_t) The new size of the buffer.
      */
      void Reallocate(size_t new_size)
      {
        // Allocate the new data
        size_t buffer_size = 0;
        eastl::tuple<ArrayPtr<Types>...> old_elements = elements_;
        elements_ = eastl::tuple<ArrayPtr<Types>...>();

        // Initialize the new buffer
        ForEachElement<calculate_element_buffer_size>(new_size, buffer_size);
        void* new_buffer = foundation::Memory::Allocate(buffer_size);
        ForEachElement<offset_element_buffer>(new_buffer);

        // Copy over the data
        ForEachElement<copy_over_element>(old_elements);

        // Set the new data
        capacity_ = new_size;
        if (buffer_ != nullptr)
        {
          foundation::Memory::Deallocate(buffer_);
        }
        buffer_ = new_buffer;
      }
    private:
      size_t size_;//!< Size of the current storage.
      size_t capacity_;//!< Capacity of the current storage.
      void* buffer_;//!< The memory buffer that contains all the data.
      eastl::tuple<ArrayPtr<Types>...> elements_;//!< Typed element list.
      void** element_list_;//!< Pointer to an external list which gives simple access to the data.
      foundation::Vector<SparseHandle> sparse_array_;//!< Array with indices that maps to the actual data indices.
      foundation::Vector<DenseHandle> dense_to_sparse_array_;//!< Array with indices that maps from the actual data index to its sparse array index.
      foundation::Vector<unsigned char> generation_;//!< generation_ of the sparse handles.
      foundation::Deque<size_t> free_list_;//!< Free list of sparse handles.
      
    };
  }
}