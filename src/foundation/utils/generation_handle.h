#pragma once
#include <limits>
namespace sulphur
{
  namespace foundation
  {    
    /**
    * @class sulphur::foundation::GenerationHandle <T,size_t>
    * @brief A handle that has a few bits to store its generation in.
    * @author Raymi Klingers
    */
    template<typename T, size_t GenerationBits>
    class GenerationHandle
    {
    public:
      static constexpr  size_t kIndexBits = std::numeric_limits<size_t>::digits - GenerationBits;//!< Number bits used to store the index for handle
      static constexpr  size_t kIndexMask = (1ull << kIndexBits) - 1ull;//!< A mask to get the index from the handle

      static constexpr  size_t kGenerationBits = GenerationBits;//!< Number of bits used to store the generation
      static constexpr  size_t kGenerationMask = (1ull << kGenerationBits) - 1ull;//!< Mask to obtain the generation from the handle
      

      /**
      * @brief Constructor that creates a handle from an integral value.
      * @param handle (size_t) The value to initialize the handle with.
      * @todo Refactor temporary code
      */
      explicit GenerationHandle(size_t handle) :
        handle(handle)
      {}
      /**
      * @brief Default constructor
      * @todo Refactor temporary code
      */
      GenerationHandle()
      {}
      /**
      * @brief Gets the index from the handle data
      * @return (T) Index of the handle
      */
      T GetIndex() const
      {
        return handle & kIndexMask;
      }
      /**
      * @brief Gets the generation from the handle
      * @return (T) Generation of the handle
      */
      T GetGeneration() const
      {
        return handle >> kGenerationBits;
      }
      /**
      * @brief Creates an invalid handle
      * @return (sulphur::foundation::GenerationHandle) An invalid handle
      * @todo Refactor temporary code
      */
      static GenerationHandle InvalidHandle()
      {
        return ComponentHandleBase((T)-1);
      }

      bool operator==(const GenerationHandle<T, GenerationBits>& rhs)
      {
        return handle == rhs.handle;
      };

      friend bool operator<(const GenerationHandle& a, const GenerationHandle& b)
      {
        return a.handle < b.handle;
      }

      T handle;//!< The data of the handle where the generation and index of the handle is stored.
    };
  }
}