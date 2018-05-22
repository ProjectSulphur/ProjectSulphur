#pragma once


#include "engine/assets/asset_interfaces.h"
#include "engine/assets/shader_program.h"

#include <graphics/platform/pipeline_state.h>

#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class Shader
    * @brief A collection of shader programs, used for rendering materials
    * @author Jelle de Haan
    */
    class Shader
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a shader
      */
      Shader();

      /**
      * @brief Create a shader with some shader programs
      * @param[in] vertex_shader (ShaderProgramHandle) The vertex shader program
      * @param[in] hull_shader (ShaderProgramHandle) The hull shader program
      * @param[in] domain_shader (ShaderProgramHandle) The domain shader program
      * @param[in] geometry_shader (ShaderProgramHandle) The geometry shader program
      * @param[in] pixel_shader (ShaderProgramHandle) The pixel shader program
      */
      Shader(ShaderProgramHandle vertex_shader,
        ShaderProgramHandle hull_shader,
        ShaderProgramHandle domain_shader,
        ShaderProgramHandle geometry_shader,
        ShaderProgramHandle pixel_shader);

      const ShaderProgramHandle GetShaderByType(ShaderType type) const;
      const ShaderProgramHandle vertex_shader() const { return vertex_shader_; };
      const ShaderProgramHandle hull_shader() const { return hull_shader_; };
      const ShaderProgramHandle domain_shader() const { return domain_shader_; };
      const ShaderProgramHandle geometry_shader() const { return geometry_shader_; };
      const ShaderProgramHandle pixel_shader() const { return pixel_shader_; };
      
      /**
      * @brief Returns a reference to the uniform buffer attached to the shader of this type
      * @param[in] shader_type (ShaderType) The shader to get the uniform buffer from
      * @return A reference to the uniform buffer
      */
      const UniformBuffer& GetUniformBuffer(ShaderType shader_type = ShaderType::kPixel) const;

      /**
      * @brief Returns a list of all texture info used by the shader program
      * @param[in] shader_type (ShaderType) The shader to get the texture info from
      * @return (const foundation::Vector <TextureInfo>&) The list of texture types
      */
      const foundation::Vector<TextureInfo>& GetTextureInfo(ShaderType shader_type = ShaderType::kPixel) const;

      graphics::PipelineState pipeline_state;

    private:
      ShaderProgramHandle vertex_shader_;
      ShaderProgramHandle hull_shader_;
      ShaderProgramHandle domain_shader_;
      ShaderProgramHandle geometry_shader_;
      ShaderProgramHandle pixel_shader_;

      void DefaultVertexShader();
      void DefaultPixelShader();
    };

    using ShaderHandle = AssetHandle<Shader>; //!< Asset handle to a shader
  }
}
