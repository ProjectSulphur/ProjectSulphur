#include "g_buffer.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    GBuffer::GBuffer(const glm::u32vec2& size) :
      GBuffer(size.x, size.y)
    {}

    //-------------------------------------------------------------------------
    GBuffer::GBuffer(uint width, uint height) :
      RenderTarget(RenderTargetType::kBackBuffer, width, height, TextureFormat::kR8G8B8A8_UNORM),
      normal_buffer_(RenderTargetType::kTexture2D, width, height, TextureFormat::kR8G8B8A8_UNORM)
    {}
  }
}