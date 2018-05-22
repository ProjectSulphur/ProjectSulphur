#pragma once
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace builder 
  {
    class Directory;
    /**
    *@struct sulphur::builder::ShaderPipelineOptions
    *@brief set of options to specify how shader should be compiled
    *@author Stan Pepels
    */
    struct ShaderPipelineOptions
    {
      foundation::Vector<Directory> additional_include_dirs; //!< additional include directories aside from the root
      uint8_t targets; //!< bitmask of targets to compile for i.e. Vulkan, Gnm, DX12
    };
  }
}
