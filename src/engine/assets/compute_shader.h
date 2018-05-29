#pragma once
#include "engine/assets/asset_interfaces.h"
#include "engine/assets/shader_program.h"

#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::ComputeShader
    * @brief A specialization class for compute shaders
    * @see sulphur::foundation::Shader
    * @author Jelle de Haan
    */
    class ComputeShader
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a compute shader
      */
      ComputeShader() = default;

      /**
      * @brief Create a compute shader with a shader program
      * @param[in] compute_shader (sulphur::engine::ShaderProgramHandle) The compute shader program
      */
      ComputeShader(ShaderProgramHandle compute_shader);

      /**
      * @brief Returns the compute shader program
      */
      const ShaderProgramHandle& compute_shader() const { return compute_shader_; }
      
      /**
      * @brief Returns the amount of thread in each work group of this compute pass
      * @return (const glm::uvec3&) The work group size
      */
      const glm::uvec3& GetWorkGroupSize() const;

      /**
      * @brief Returns a reference to the uniform buffer attached to the compute shader
      * @return (const sulphur::engine::UniformBuffer&) A const reference to the uniform buffer
      */
      const UniformBuffer& GetUniformBuffer() const;

      /**
      * @brief Returns a list of all texture info used by the shader program
      * @return (const sulphur::foundation::Vector <TextureInfo>&) The list of texture types
      */
      const foundation::Vector<TextureInfo>& GetTextureInfo() const;
      
      /**
      * @brief Returns a list of all uav texture info used by the shader program
      * @return (const sulphur::foundation::Vector <TextureInfo>&) The list of texture types
      */
      const foundation::Vector<TextureInfo>& GetUAVInfo() const;

    private:
      ShaderProgramHandle compute_shader_;
    };

    using ComputeShaderHandle = AssetHandle<ComputeShader>; //!< Asset handle to a compute shader
  }
}
