#include "depth_buffer.h"
#include "engine/assets/asset_system.h"

#include <foundation/utils/type_definitions.h>

namespace sulphur
{
  namespace engine
  {
    DepthBuffer::DepthBuffer(const glm::u32vec2& size, TextureFormat format) :
      DepthBuffer(size.x, size.y, format)
    {}

    DepthBuffer::DepthBuffer(uint width, uint height, TextureFormat format) :
      format_(format)
    {
      static int idx = 0;

      Texture* texture_01 = foundation::Memory::Construct<Texture>(width, height, format);

      buffer_ = AssetSystem::Instance().AddAsset(
        texture_01,
        foundation::String("__Depth_Buffer_") + foundation::to_string(idx++));
    }
  }
}