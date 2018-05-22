#pragma once

#include "engine/assets/texture.h"

#include <foundation/utils/type_definitions.h>

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class DepthBuffer
    * @brief A depth buffer.
    * @author Jelle de Haan
    */
    class DepthBuffer
    {
    public:
      DepthBuffer() : format_(TextureFormat::kR24G8_TYPELESS){};

      DepthBuffer(const glm::u32vec2& size, TextureFormat format = TextureFormat::kR24G8_TYPELESS);
      
      DepthBuffer(uint width, uint height, TextureFormat format = TextureFormat::kR24G8_TYPELESS);

      TextureHandle buffer() const { return buffer_; }
      TextureFormat format() const { return format_; }

    private:
      TextureHandle buffer_;
      TextureFormat format_;
    };
  }
}