#pragma once

#include "engine/assets/asset_interfaces.h"
#include "engine/assets/uniform_buffer.h"

#include <foundation/utils/type_definitions.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    enum struct TextureType; // texture.h

    /**
    * @enum sulphur::engine::TextureInfo
    * @brief Describes the reflection info of a texture
    * @see sulphur::engine::Texture
    */
    struct TextureInfo
    {
      foundation::String name;
      TextureType type;
    };

    /**
    * @enum sulphur::engine::ShaderType
    * @brief Describes the type of shader program
    * @see sulphur::engine::ShaderProgram
    */
    enum struct ShaderType
    {
      kVertex,
      kHull,
      kDomain,
      kGemometry,
      kPixel,
      kCompute
    };

    /**
    * @class ShaderProgram
    * @brief A shader program that can be combined with other shader programs to form a shader
    * @see sulphur::engine::Shader
    * @author Jelle de Haan
    */
    class ShaderProgram
    {
    public:
      friend class Shader; // TEMP HACK FOR DEFAULT SHADER
      
      /**
      * @brief Default empty constructor, use the other constructor for making a shader program
      */
      ShaderProgram() {};

      /**
      * @brief Constructs a shader program from raw shader byte codes and reflection data
      * @param[in] shader_data (foundation::ShaderData&) The shader reflector data
      */
      ShaderProgram(foundation::ShaderData& shader_data);

      operator bool() const 
      {
        return IsValid(); 
      }

      /**
      * @brief Check if this ShaderProgram is valid
      * @return (bool) Returns true if the shader byte code data is not empty
      */
      inline bool IsValid() const
      { 
        return (!shader_byte_code_.empty());
      }

      /**
      * @brief Returns the raw shader byte code vector
      * @return (const foundation::Vector <byte>&) The byte code vector
      */
      const foundation::Vector<byte>& byte_code() const
      {
        return shader_byte_code_;
      }

      /**
      * @brief Returns a pointer to the beginning of the byte code data
      * @return (const byte*) The byte pointer
      */
      const byte* byte_code_data()
      {
        return shader_byte_code_.data();
      }

      /**
      * @brief Returns the size of the byte code data
      * @return (size_t) The byte code size
      */
      size_t byte_code_size() const
      {
        return shader_byte_code_.size();
      }

      /**
      * @brief Returns the uniform buffer created from the shader reflector data
      * @return (const UniformBuffer&) The constant UniformBuffer (Use the copy constructor to modify for a material)
      * @see sulphur::engine::UniformBuffer
      */
      const UniformBuffer& uniform_buffer()
      {
        return uniform_buffer_;
      }

      /**
      * @brief Returns a list of all texture types used by the shader program
      * @return (const foundation::Vector <TextureType>&) The list of texture types
      */
      const foundation::Vector<TextureInfo>& GetTextureInfo() const { return textures_; };

    private:
      foundation::Vector<byte> shader_byte_code_;
      UniformBuffer uniform_buffer_;

      foundation::Vector<TextureInfo> textures_;
    };

    using ShaderProgramHandle = AssetHandle<ShaderProgram>; //!< Asset handle to a shader program
  }
}
