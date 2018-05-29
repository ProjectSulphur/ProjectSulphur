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
      kGeometry,
      kPixel,
      kCompute
    };

    /**
    * @class sulphur::engine::ShaderProgram
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
      * @param[in] shader_data (sulphur::foundation::ShaderData&) The shader reflector data
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
      * @return (const sulphur::foundation::Vector <byte>&) The byte code vector
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
      * @brief Returns the amount of thread in each work group of this compute pass
      * @return (const glm::uvec3&) The work group size
      */
      const glm::uvec3& work_group_size() const
      {
        return work_group_size_;
      }

      /**
      * @brief Returns the uniform buffer created from the shader reflector data
      * @return (const sulphur::engine::UniformBuffer&) The constant UniformBuffer (Use the copy constructor to modify for a material)
      * @see sulphur::engine::UniformBuffer
      */
      const UniformBuffer& uniform_buffer()
      {
        return uniform_buffer_;
      }

      /**
      * @brief Returns the stage at which this shader program should run
      * @return (sulphur::engine::ShaderType) The type of this shader program
      */
      ShaderType type()
      {
        return type_;
      }

      /**
      * @brief Returns a list with info about the textures used by the shader program
      * @return (const sulphur::foundation::Vector <TextureInfo>&) The list of texture types
      */
      const foundation::Vector<TextureInfo>& GetTextureInfo() const { return textures_; };

      /**
      * @brief Returns a list with info about the UAV textures used by the shader program
      * @return (const sulphur::foundation::Vector <TextureType>&) The list of UAV texture types
      */
      const foundation::Vector<TextureInfo>& GetUAVInfo() const { return uavs_; };

    private:
      foundation::Vector<byte> shader_byte_code_; //!< The raw byte code of the shader
      UniformBuffer uniform_buffer_; //!< The uniform buffer used to interact with shader variables

      foundation::Vector<TextureInfo> textures_; //!< A collection of texture types used by this shader
      foundation::Vector<TextureInfo> uavs_; //!< A collection of UAV texture types used by this shader
      ShaderType type_;
      glm::uvec3 work_group_size_;
    };

    using ShaderProgramHandle = AssetHandle<ShaderProgram>; //!< Asset handle to a shader program
  }
}
