#pragma once


#include "engine/assets/texture.h"
#include "engine/assets/material.h"

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @enum sulphur::engine::RenderTargetType
    * @brief Describes the texture type of a render target
    * @see sulphur::engine::RenderTarget
    */
    enum struct RenderTargetType
    {
      kBackBuffer,
      kCubemap,
      kTexture2D,
      kTexture2DArray
    };

    /**
    * @class RenderTarget
    * @brief A render target.
    * @author Jelle de Haan
    */
    class RenderTarget
    {
    public:
      /**
      * @brief A default constructor for the render target.
      */
      RenderTarget();

      /**
      * @brief A constructor that creates a render target based upon the type, size and format.
      * @param[in] type (RenderTargetType) The type of render target that it is.
      * @param[in] size (const glm::u32vec2&) The size of the render target.
      * @param[in] format (TextureFormat) The format of the render target.
      */
      RenderTarget(RenderTargetType type, const glm::u32vec2& size, TextureFormat format);

      /**
      * @brief A constructor that creates a render target based upon the type, size and format.
      * @param[in] type (RenderTargetType) The type of render target that it is.
      * @param[in] width (uint) The width of the render target.
      * @param[in] height (uint) The height of the render target.
      * @param[in] format (TextureFormat) The format of the render target.
      */
      RenderTarget(RenderTargetType type, uint width, uint height, TextureFormat format);

      void set_post_process_material(const MaterialHandle& post_process_material)
      {
        post_process_material_ = post_process_material;
      };

      MaterialHandle post_process_material() const
      {
        return post_process_material_;
      };

      void set_scissor_rect(const glm::vec4& scissor_rect)
      {
        scissor_rect_ = scissor_rect;
      };

      glm::vec4 scissor_rect() const
      {
        return scissor_rect_;
      };

      TextureHandle GetTarget(bool use_copy = false);
      TextureHandle GetTarget() const;

      glm::u32vec2 GetTextureSize() const
      {
        return buffers_[0]->size();
      };
      
      RenderTargetType render_target_type() const
      {
        return type_;
      }

    private:
      RenderTargetType type_;
      bool using_copy_;

      TextureHandle buffers_[2];
      glm::vec4 scissor_rect_;

      MaterialHandle post_process_material_;
    };
  }
}