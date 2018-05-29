#include "render_target.h"
#include <engine/assets/asset_system.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    RenderTarget::RenderTarget() :
      type_(RenderTargetType::kBackBuffer),
      swapped_buffers_(false)
    {}

    //-------------------------------------------------------------------------
    RenderTarget::RenderTarget(RenderTargetType type, const glm::u32vec2& size, TextureFormat format) :
      RenderTarget(type, size.x, size.y, format)
    {}

    //-------------------------------------------------------------------------
    RenderTarget::RenderTarget(RenderTargetType type, uint width, uint height,
        TextureFormat format) :
      type_(type),
      swapped_buffers_(false)
    {
      static int idx = 0;
      
      Texture* texture_01 = foundation::Memory::Construct<Texture>(width, height, format, 
        TextureCreateFlags::kAllowRenderTarget | TextureCreateFlags::kAllowUAV);

      Texture* texture_02 = foundation::Memory::Construct<Texture>(width, height, format, 
        TextureCreateFlags::kAllowRenderTarget | TextureCreateFlags::kAllowUAV);

      buffers_[0] = AssetSystem::Instance().AddAsset(
        texture_01,
        foundation::String("__Render_Target[0]_") + foundation::to_string(idx));

      buffers_[1] = AssetSystem::Instance().AddAsset(
        texture_02,
        foundation::String("__Render_Target[1]_") + foundation::to_string(idx));

      idx++;
    }
    
    //-------------------------------------------------------------------------
    void RenderTarget::SwapBuffers()
    {
      swapped_buffers_ = !swapped_buffers_;
    }

    //-------------------------------------------------------------------------
    TextureHandle RenderTarget::GetTextureResource() const
    {
      return swapped_buffers_ ? buffers_[1] : buffers_[0];
    }
  }
}