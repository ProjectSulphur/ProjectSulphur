#pragma once

#include "engine/assets/asset_interfaces.h"

#include <foundation/utils/type_definitions.h>
#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @enum sulphur::engine::TextureFormat
    * @brief Describes the byte layout of a texture
    * @see sulphur::engine::Texture
    */
    enum struct TextureFormat
    {
      kR8G8B8A8_UNORM,
      kR16G16B16A16_FLOAT,
      kR32G32B32A32_FLOAT,
      kR24G8_TYPELESS,
      kR32_TYPELESS,
    };

    /**
    * @enum sulphur::engine::TextureType
    * @brief Describes the type of a texture
    * @see sulphur::engine::Texture
    */
    enum struct TextureType
    {
      kTexture2D,
      kTexture3D,
      kCubed
    };

    /**
     * @author Jelle de Haan
     */
    class Texture
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a texture
      */
      Texture();

      /**
      * @brief Creates a texture from pixel data.
      * @param[in] pixel_data (const sulphur::foundation::Vector <byte>&) The pixel 
      * data of the texture. Data should be in RGBA8 format.
      * @param[in] width (uint) The width of the texture in pixels.
      * @param[in] height (uint) The height of the texture in pixels.
      * @param[in] format (TextureFormat) The format of the texture.
      */
      Texture(const foundation::Vector<byte>& pixel_data, uint width, uint height,
        TextureFormat format = TextureFormat::kR8G8B8A8_UNORM);
      
      /**
      * @brief Creates a texture from pixel data.
      * @param[in] pixel_data (const sulphur::foundation::Vector <byte>&) The pixel
      * data of the texture. Data should be in RGBA8 format.
      * @param[in] size (const glm::u32vec2&) The size of the texture in pixels.
      * @param[in] format (TextureFormat) The format of the texture.
      */
      Texture(const foundation::Vector<byte>& pixel_data, const glm::u32vec2& size, 
        TextureFormat format = TextureFormat::kR8G8B8A8_UNORM);

      /**
      * @brief Creates a texture and sets all bytes to 0u.
      * @param[in] width (uint) The width of the texture in pixels.
      * @param[in] height (uint) The height of the texture in pixels.
      * @param[in] format (TextureFormat) The format of the texture.
      */
      Texture(uint width, uint height, 
        TextureFormat format = TextureFormat::kR8G8B8A8_UNORM);

      /**
      * @brief Creates a texture and sets all bytes to 0u.
      * @param[in] size (const glm::u32vec2& size) The size of the texture in pixels.
      * @param[in] format (TextureFormat) The format of the texture.
      */
      Texture(const glm::u32vec2& size, 
        TextureFormat format = TextureFormat::kR8G8B8A8_UNORM);

      /**
      * @brief Returns the size of the texture in pixels
      * @return (glm::u32vec2) The size
      */
      glm::u32vec2 size() const { return size_; };

      /**
      * @brief Returns the width of the texture in pixels
      * @return (uint) The width
      */
      uint width() const { return size_.x; };

      /**
      * @brief Returns the height of the texture in pixels
      * @return (uint) The height
      */
      uint height() const { return size_.y; };

      /**
      * @brief Returns a constant vector containing the texture data in bytes
      * @return (const foundation::Vector <byte>&) The byte vector
      */
      const foundation::Vector<byte>& raw_data() const { return data_; };

      /**
      * @brief Returns the format of the texture 
      * @return (TextureFormat) The format
      */
      TextureFormat format() const { return format_; };

    private:
      const glm::u32vec2 size_;
      foundation::Vector<byte> data_;
      const TextureFormat format_;

      foundation::Vector<byte> GenerateTextureData();
    };

    using TextureHandle = AssetHandle<Texture>; //!< Asset handle to a texture
  }
}
