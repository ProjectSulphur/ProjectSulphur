#pragma once
#include "engine/assets/render_target.h"


namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::GBuffer
    * @brief Helper class for rendering with a deferred context
    * @author Jelle de Haan
    */
    class GBuffer : public RenderTarget
    {
    public:
      /**
      * @brief Creates an invalid GBuffer, use the other constructor instead
      */
      GBuffer() {}

      /**
      * @brief A constructor that creates a collection of render targets
      * @param[in] size (const glm::u32vec2&) The size of the render target
      */
      GBuffer(const glm::u32vec2& size);

      /**
      * @brief A constructor that creates a collection of render targets
      * @param[in] width (uint) The width of the render target
      * @param[in] height (uint) The height of the render target
      */
      GBuffer(uint width, uint height);

      RenderTarget& diffuse_buffer() { return static_cast<RenderTarget&>(*this); };
      RenderTarget& normal_buffer() { return normal_buffer_; };


      RenderTargetType render_target_type() const override
      {
        return RenderTargetType::kGBuffer;
      }

    private:
      RenderTarget normal_buffer_;
    };
  }
}