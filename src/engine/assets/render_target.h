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
      kGBuffer,
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

      /**
      * @brief This is needed for some reason, else it won't compile
      */
      virtual ~RenderTarget() {};

      glm::vec4 scissor_rect() const { return scissor_rect_; };
      void set_scissor_rect(const glm::vec4& scissor_rect)
      {
        scissor_rect_ = scissor_rect;
      };
      
      /**
      * @brief Swaps the two texture buffers so you can use it as a shader resource
      */
      virtual void SwapBuffers();

      /**
      * @brief Returns a handle to a texture resource (Should only be used by the renderers)
      * @return (TextureHandle) The texture resource
      */
      virtual TextureHandle GetTextureResource() const;


      virtual glm::u32vec2 GetTextureSize() const
      {
        return buffers_[0]->size();
      };
      
      virtual RenderTargetType render_target_type() const
      {
        return type_;
      }

    protected:
      RenderTargetType type_; //<! The type of the render target
      bool swapped_buffers_; //<! Indicates what index to use for `buffers_`

      TextureHandle buffers_[2]; //<! The texture buffers
      glm::vec4 scissor_rect_; //<! The scissor rect
    };
  }
}