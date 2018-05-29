#pragma once
#include <foundation/utils/type_definitions.h>
namespace sulphur
{
  namespace engine
  {
    struct FrameStorage;
    /**
    * @class sulphur::engine::RewindStorageBase
    * @brief A base class for storage that needs to be rewinded
    * @author Raymi Klingers
    */
    class RewindStorageBase
    {
    public:
      /**
      * @brief Constructor
      * @param[in] element_list_ (void**) A list that will contain pointers to the element buffers for easy access
      * @param[in] element_sizes (uint64_t*) A pointer to the buffer sizes array
      * @param[in] num_elements (uint64_t) Number of elements stored in the rewindable storage
      */
      RewindStorageBase(void** element_list, uint64_t* element_sizes, uint64_t num_elements)
        :
        size_(0),
        capacity_(0),
        element_sizes_(element_sizes),
        element_list_(element_list),
        num_elements_( num_elements )
      {}

      /**
      * @brief Default constructor
      */
      virtual ~RewindStorageBase()
      {}
      /**
      * @brief A function to prepare the data  in the storage for restoring an older state.
      */
      virtual void PrepareStore() = 0;
      /**
      * @brief A function to prepare the data  in the storage for restoring an older state.
      */
      virtual void PrepareRestore( const FrameStorage& storage ) = 0;
    public:
      uint64_t size_;//!< Size of the current storage.
      uint64_t capacity_;//!< Capacity of the current storage.
      uint64_t* element_sizes_;//!< The size of the elements
      void** element_list_;//!< Pointer to an external list which gives simple access to the data.
      const uint64_t num_elements_;//!< Number of elements
    };
  }
}