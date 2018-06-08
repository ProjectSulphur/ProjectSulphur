#pragma once
#include "allocators/general_allocator.h"
#include "allocators/eastl_allocator.h"
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/scoped_ptr.h>
#include <mutex>
#include <memory>

namespace sulphur
{
  namespace foundation
  {
    class Memory;
    template <typename T>
    struct MemoryDeleter;
    template<typename T>
    struct UniqueType;
    template<typename T>
    struct UniqueType<T[]>;

    template<typename T, typename Deleter = MemoryDeleter<T>>
    using UniquePointer = eastl::unique_ptr<T, Deleter>;//!< Aliasing the eastl unique pointer
    template<typename T>
    using SharedPointer = eastl::shared_ptr<T>;//!< Aliasing the eastl shared pointer

    /**
    * @class sulphur::foundation::Memory
    * @brief The memory class is an interface for memory allocations.
    * @todo The general allocator could be stored as a pointer to allow for forward declaration?
    * @author Raymi Klingers
    */
    class Memory
    {
      /**
      * @brief We don't want people to create this class
      */
      Memory() = delete;
      /**
      * @brief We don't want people to create this class
      */
      Memory(const Memory&) = delete;
      /**
      * @brief We don't want people to create this class
      */
      Memory(Memory&&) = delete;
      static constexpr size_t kDefaultAlignment = 16;//!< Default alignment
    public:
      /**
      * @brief Initializes the default allocator with a size. This doesn't do anything on Win32
      * @param[in] heap_size (size_t) The size of the memory block
      * @remarks now uses malloc for windows which actually does not need to be initialized. It might be required for other platforms however.
      */
      static void Initialize(size_t heap_size);
      /**
      * @brief Gets the default allocator
      * @return (sulphur::foundation::IAllocator&) The default allocator
      */
      static IAllocator& default_allocator();
      /**
      * @brief Creates an allocator and feeds it with a block of memory and its size
      * @param[in] memory_block (void*) The memory block
      * @param[in] heap_size (size_t) The size of the memory block
      * @return (T) The created allocator
      */
      template<typename T>
      static T CreateAllocator(void* memory_block, size_t heap_size);
      /**
      * @brief Constructs the type and calls its constructor
      * @param[in] allocator (sulphur::foundation::IAllocator*) The allocator used to allocate the type
      * @param[in] args (...Args&&) Constructor arguments
      * @return (T*) The constructed type
      */
      template<typename T, typename ...Args>
      static T* Construct(IAllocator* allocator, Args&& ...args);
      /**
      * @brief Constructs the type and calls its constructor using the default allocator
      * @param[in] args (...Args&&) Constructor arguments
      * @return (T*) The constructed type
      */
      template<typename T, typename ...Args>
      static T* Construct(Args&& ...args);
      /**
      * @brief Constructs an array of type and calls the objects constructor using the given allocator. If none is passed it will use the default allocator
      * @param[in] size (size_t) Number of array elements
      * @param[in] alignment (size_t) Alignment
      * @param[in] allocator (sulphur::foundation::IAllocator*) Allocator to allocate the array which defaults to the default allocator
      * @return (T*) The pointer to the constructed array
      */
      template<typename T>
      static T* ConstructArray(size_t size,
        size_t alignment = kDefaultAlignment,
        IAllocator* allocator = nullptr);
      /**
      * @brief Destructs the type and calls its destructor
      * @param[in] ptr (T*) Pointer to object that needs to be destructed
      */
      template<typename T>
      static void Destruct(T* ptr);
      /**
      * @brief Destructs array of type and calls its destructor
      * @param[in] ptr (T*) Pointer to the array that needs to be destructed
      */
      template<typename T>
      static void DestructArray(T* ptr);
      /**
      * @brief Allocates a block of memory with specified size and allocator.
      * @param[in] size (size_t) Size of the required memory block
      * @param[in] size (size_t) Alignment of the memory block
      * @param[in] size (sulphur::foundation::IAllocator*) Allocator to allocate the memory block which defaults to the default allocator
      * @returns (void*) The newly allocated memory
      */
      static void* Allocate(size_t size,
        size_t alignment = kDefaultAlignment,
        IAllocator* allocator = nullptr);

      /**
      * @brief Reallocates a block of memory with specified size and allocator.
      * @param[in] ptr (void*) A pointer to the source block of memory
      * @param[in] size (size_t) Size of the required memory block
      * @param[in] size (size_t) Alignment of the memory block
      * @param[in] size (sulphur::foundation::IAllocator*) Allocator to allocate the memory block which defaults to the default allocator
      * @returns (void*) The newly allocated memory
      */
      static void* Reallocate(
        void* ptr,
        size_t size,
        size_t alignment = kDefaultAlignment,
        IAllocator* allocator = nullptr);

      /**
      * @brief Deallocates a block of memory
      * @param[in] ptr (const void*) Pointer to memory that needs to be deallocated
      * @remarks Assumes that the pointer is valid
      * @return (void*) Pointer to the allocated block of memory
      */
      static void Deallocate(const void* ptr);
      /**
      * @brief Constructs the type and returns a shared pointer @see sulphur::foundation:Memory::Construct
      * @param[in] allocator (sulphur::foundation::IAllocator*) The allocator used to allocate the type and make the shared pointer
      * @param[in] args (...Args&&) Constructor arguments
      * @return (sulphur::foundation::SharedPointer <T>) The constructed type wrapped in a shared pointer
      */
      template<typename T, typename ...Args>
      static SharedPointer<T> ConstructShared(IAllocator* allocator, Args&& ...args);
      /**
      * @brief Constructs the type and returns a shared pointer using the default allocator @see sulphur::foundation:Memory::Construct
      * @param[in] args (...Args&&) Constructor arguments
      * @return (sulphur::foundation::SharedPointer <T>) The constructed type wrapped in a shared pointer
      */
      template<typename T, typename ...Args>
      static SharedPointer<T> ConstructShared(Args&& ...args);
      /**
      * @brief Constructs the type and returns a unique pointer @see sulphur::foundation:Memory::Construct
      * @param[in] allocator (sulphur::foundation::IAllocator*) Allocator to allocate memory
      * @param[in] args (...Args&&) Constructor arguments
      * @return (sulphur::foundation::UniquePointer <sulphur::foundation::UniqueType <T>::type, MemoryDeleter <T>>) The constructed type wrapped in a shared pointer
      */
      template<typename T, typename ...Args>
      static UniquePointer<typename UniqueType<T>::type, MemoryDeleter<T>> ConstructUnique(
        IAllocator* allocator
        , Args&& ...args);
      /**
      * @brief Constructs the type and returns a unique pointer using the default allocator @see sulphur::foundation:Memory::Construct
      * @param[in] args (...Args&&) Constructor arguments
      * @return (sulphur::foundation::UniquePointer <sulphur::foundation::UniqueType <T>::type, MemoryDeleter <T>>) The constructed type wrapped in a unique pointer
      */
      template<typename T, typename ...Args>
      static UniquePointer<typename UniqueType<T>::type, MemoryDeleter<T>> ConstructUnique(
        Args&& ...args);
      /**
      * @brief Constructs the array type and returns a unique pointer using the default allocator @see sulphur::foundation:Memory::Construct
      * @param[in] size (size_t) Number of elements in array
      * @param[in] alignment (size_t) Alignment of the array
      * @param[in] allocator (sulphur::foundation::IAllocator*) Allocator to allocate the memory for the array
      * @return (sulphur::foundation::UniquePointer <sulphur::foundation::UniqueType <T>::array_type, MemoryDeleter <sulphur::foundation::UniqueType <T>::array_type>>) The constructed type wrapped in a unique pointer
      */
      template<typename T>
      static UniquePointer<typename UniqueType<T>::array_type,
        MemoryDeleter<typename UniqueType<T>::array_type>> ConstructUnique(
        size_t size, 
        size_t alignment = kDefaultAlignment, 
        IAllocator* allocator = nullptr);
      /**
      * @brief Makes a shared pointer from the given pointer
      * @param[in] ptr (T*) The pointer to wrap in a shared pointer
      * @return (sulphur::foundation::SharedPointer <T>) The pointer wrapped in a shared pointer
      */
      template<typename T>
      static SharedPointer<T> MakeShared(T* ptr);

      /**
      * @return (bool) Is the memory system intialized?
      */
      static bool IsInitialized();

      /**
      * @brief Shutsdown the memory and checks for leaks
      * @todo Check for leaks and design a system to catch other allocator leaks too
      */
      static void Shutdown();

    private:
      /**
      * @brief Checks if the allocator is a nullptr and replaces it with the default allocator
      * @param[in] allocator (sulphur::foundation::IAllocator*&) The allocator pointer to be checked
      */
      static void FallBack(IAllocator*& allocator);

      /**
      * @brief Logs a simple warning message
      * @remarks This function is only here to avoid having to include the logger in the memory class
      * @param[in] message (const char*) The message
      */
      static void LogWarning(const char* message);
      /**
      * @struct sulphur::foundation::Memory::MemoryHeader
      * @brief A header that is attached to allocators to store the used allocator and the size of the user allocated block
      * @author Raymi Klingers
      */
      struct MemoryHeader
      {
        IAllocator* allocator;//!< The allocator that was used to allocate the block of memory to which this header is attached
        size_t size;//!< The size of the block of memory attached to this header
        size_t alignment; //!< The alignment of this header
      };

      static GeneralAllocator default_allocator_; //!< The default allocator
      static bool initialized_; //!< Is the memory system initialized?
      static std::mutex alloc_mutex_; //!< mutex for allocation and deallocation
    };
    /**
    * @struct sulphur::foundation::MemoryDeleter <T>
    * @brief A class that contains the destructor behaviour for smart pointers
    * @author Raymi Klingers
    */
    template <typename T>
    struct MemoryDeleter
    {
      /**
      * @brief Default constructor
      */
      MemoryDeleter() = default;
      /**
      * @brief Allowing conversion between derived and base classes
      * @param[in] other (const sulphur::foundation::MemoryDeleter <U>&) The deleter of probably another
      * smart pointer that we're trying to assign to a base version of the smart pointer
      * @param[in] test (typename eastl::enable_if <eastl::is_convertible <U*, T*>::value>::type*) A test U is convertible to T
      */
      template <typename U>  // Enable if T* can be constructed with U* (i.e. U* is convertible to T*).
      MemoryDeleter(
        const MemoryDeleter<U>&, 
        typename eastl::enable_if<eastl::is_convertible<U*, T*>::value>::type* = nullptr)
      {}
      /**
      * @brief Destructs the passed pointer
      * @param[in] ptr (T*) A pointer to an object that has to be destruct
      */
      void operator()(T* ptr);
    };
    /**
    * @struct sulphur::foundation::MemoryDeleter <T[]>
    * @brief A class that contains the destructor behaviour for smart array pointers
    * @author Raymi Klingers
    */
    template <typename T>
    struct MemoryDeleter<T[]>
    {
      /**
      * @brief Default constructor
      */
      MemoryDeleter() = default;
      /**
      * @brief Allowing conversion between derived and base classes
      * @param[in] other (const sulphur::foundation::MemoryDeleter <U>&) The deleter of probably another
      * smart pointer that we're trying to assign to a base version of the smart pointer
      * @param[in] test (typename eastl::enable_if <eastl::Internal::is_array_cv_convertible <U*, T*>::value>::type*) A test U is convertible to from T
      */
      template <typename U>  // Enable if T* can be constructed with U* (i.e. U* is convertible to T*).
      MemoryDeleter(
        const MemoryDeleter<U[]>& other,
        typename eastl::enable_if<eastl::Internal::is_array_cv_convertible<U*, T*>::value>::type* test = nullptr)
      {}
      /**
      * @brief Destructs the passed pointer
      * @param[in] ptr (T*) A pointer to an object that has to be destruct
      */
      void operator()(T* ptr);
    };

    /**
    * @struct sulphur::foundation::UniqueType <T>
    * @brief A template class to specialize between pointers to objects and arrays
    * @author Raymi Klingers
    */
    template<typename T>
    struct UniqueType
    {
      using type = T;
    };
    /**
    * @struct sulphur::foundation::UniqueType <T[]>
    * @brief A template class to specialize between pointers to objects and arrays
    * @author Raymi Klingers
    */
    template<typename T>
    struct UniqueType<T[]>
    {
      using array_type = T[];
    };

    //--------------------------------------------------------------------------
    template<typename Allocator >
    inline Allocator  Memory::CreateAllocator(void* memory_block, size_t heap_size)
    {
      return Allocator(memory_block, heap_size);
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline T* Memory::Construct(IAllocator* allocator, Args&& ...args)
    {
      FallBack(allocator);
      void* mem = Allocate(sizeof(T), kDefaultAlignment, allocator);
      return new(mem) T(eastl::forward<Args>(args)...);
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline T* Memory::Construct(Args&& ...args)
    {
      return Construct<T>(&default_allocator(), eastl::forward<Args>(args)...);
    }
    template<typename T>
    inline T* Memory::ConstructArray(size_t size, size_t alignment, IAllocator* allocator)
    {
      FallBack(allocator);
      T* mem = reinterpret_cast<T*>(Allocate(sizeof(T)*size, alignment, allocator));
      for (size_t i = 0; i < size; ++i)
      {
        new(&mem[i]) T();
      }
      return mem;
    }
    //--------------------------------------------------------------------------
    template<typename T>
    inline void Memory::Destruct(T* ptr)
    {
      if (ptr == nullptr)
      {
        return;
      }

      //TODO (Raymi): This does not allow inheritance since the inherited class size might be bigger than the base class size.
      //MemoryHeader* header = reinterpret_cast<MemoryHeader*>(
      //  OffsetBytes(ptr, -static_cast<int>(sizeof(MemoryHeader))));
      //size_t num_elements = header->size / sizeof(T);
      /*if (num_elements > 1)
      {
        LogWarning("Arrays should be destructed with DestructArray");
      }*/
      ptr->~T();
      Deallocate(ptr);
    }
    template<typename T>
    inline void Memory::DestructArray(T * ptr)
    {
      if (ptr == nullptr)
      {
        return;
      }

      MemoryHeader* header = reinterpret_cast<MemoryHeader*>(
        OffsetBytes(ptr,-static_cast<int>(sizeof(MemoryHeader))));
      size_t num_elements = header->size / sizeof(T);
      if (header->size % sizeof(T) != 0)
      {
        LogWarning("Array destructed is of different type which has a different size");
      }
      for (size_t i = 0; i < num_elements; ++i)
      {
        ptr[i].~T();
      }
      Deallocate(ptr, sizeof(T));
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline SharedPointer<T> Memory::ConstructShared(IAllocator* allocator, Args&& ...args)
    {
      return MakeShared<T>(Construct<T>(allocator, eastl::forward<Args>(args)...));
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline SharedPointer<T> Memory::ConstructShared(Args&& ...args)
    {
      return ConstructShared<T>(&default_allocator(), eastl::forward<Args>(args)...);
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline UniquePointer<typename UniqueType<T>::type, MemoryDeleter<T>> Memory::ConstructUnique(
      IAllocator* allocator,
      Args&& ...args)
    {
      return UniquePointer<T, MemoryDeleter<T>>(
        Construct<T>(allocator, eastl::forward<Args>(args)...), MemoryDeleter<T>());
    }
    //--------------------------------------------------------------------------
    template<typename T, typename ...Args>
    inline UniquePointer<typename UniqueType<T>::type, MemoryDeleter<T>> Memory::ConstructUnique(
      Args&& ...args)
    {
      return ConstructUnique<T>(&default_allocator(), eastl::forward<Args>(args)...);
    }
    //--------------------------------------------------------------------------
    template<typename T>
    inline UniquePointer<typename UniqueType<T>::array_type,
      MemoryDeleter<typename UniqueType<T>::array_type>> Memory::ConstructUnique(
        size_t size, 
        size_t alignment, 
        IAllocator* allocator)
    {
      FallBack(allocator);
      return UniquePointer<typename UniqueType<T>::array_type,
        MemoryDeleter<typename UniqueType<T>::array_type>>(
          Construct<typename eastl::remove_extent<T>::type>(size, alignment, allocator));
    }
    //--------------------------------------------------------------------------
    template<typename T>
    inline SharedPointer<T> Memory::MakeShared(T* ptr)
    {
      return SharedPointer<T>(ptr, MemoryDeleter<T>(), EASTLAllocator());
    }
    //--------------------------------------------------------------------------
    template <typename T>
    inline void MemoryDeleter<T>::operator()(T* ptr)
    {
      Memory::Destruct(ptr);
    }
    //--------------------------------------------------------------------------
    template <typename T>
    inline void MemoryDeleter<T[]>::operator()(T* ptr)
    {
      Memory::DestructArray(ptr);
    }
  }
}
