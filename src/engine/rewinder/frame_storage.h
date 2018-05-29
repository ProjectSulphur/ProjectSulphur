#pragma once
#include <foundation/containers/vector.h>
namespace sulphur
{
  namespace engine
  {
    /**
    * @struct sulphur::engine::FrameData
    * @brief The data of an element and its number of elements
    * @author Raymi Klingers
    */
    struct FrameData
    {
      void* data;//<! The data buffer
      size_t size;//<! Number of elements stored in the buffer
    };
    /**
    * @struct sulphur::engine::FrameStorage
    * @brief The data of a system stored per element
    * @author Raymi Klingers
    */
    struct FrameStorage
    {
      foundation::Vector<FrameData> data;//<! Data accesed by the system storage data to store and restore data
    };

  }
}