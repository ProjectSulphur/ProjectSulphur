#include "render_target.h"
#include <engine/assets/asset_system.h>

namespace sulphur
{
  namespace engine
  {
    RenderTarget::RenderTarget() :
      type_(RenderTargetType::kBackBuffer)
    {
    }

    RenderTarget::RenderTarget(RenderTargetType type, const glm::u32vec2& size, TextureFormat format) :
      RenderTarget(type, size.x, size.y, format)
    {
    }

    RenderTarget::RenderTarget(RenderTargetType type, uint width, uint height, 
        TextureFormat format) :
      type_(type)
    {
      static int idx = 0;
      
      Texture* texture_01 = foundation::Memory::Construct<Texture>(width, height, format);
      Texture* texture_02 = foundation::Memory::Construct<Texture>(width, height, format);

      buffers_[0] = AssetSystem::Instance().AddAsset(
        texture_01,
        foundation::String("__Render_Target[0]_") + foundation::to_string(idx));

      buffers_[1] = AssetSystem::Instance().AddAsset(
        texture_02,
        foundation::String("__Render_Target[1]_") + foundation::to_string(idx));

      idx++;
    }

    TextureHandle RenderTarget::GetTarget(bool use_copy)
    {
      if (use_copy == true)
      {
        using_copy_ = !using_copy_;
      }

      return using_copy_ ? buffers_[1] : buffers_[0];
    }

    TextureHandle RenderTarget::GetTarget() const
    {
      return using_copy_ ? buffers_[1] : buffers_[0];
    }
  }
}