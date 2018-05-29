#pragma once

#include "engine/assets/asset_interfaces.h"
#include "engine/assets/compute_shader.h"
#include "engine/assets/uniform_buffer.h"
#include "engine/assets/texture.h"

#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::ComputePass
    * @brief Used in the post process PostProcessMaterial to render in image effect
    * @author Jelle de Haan
    */
    class ComputePass
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a compute pass
      */
      ComputePass() {};

      /**
      * @brief Creates a compute pass for this shader
      * @see sulphur::engine::ComputeShader
      */
      ComputePass(const ComputeShaderHandle& shader);
      
      /**
      * @brief Replace the compute pass with a new shader
      * @remarks Removes all textures and uniform buffers currently attached
      * @see sulphur::engine::ComputeShader
      */
      void SetShader(const ComputeShaderHandle& shader);

      /**
      * @brief Returns a handle to the currently attached shader
      * @return (const sulphur::engine::ComputeShaderHandle&) The currently attached shader
      * @see sulphur::engine::ComputeShader
      */
      const ComputeShaderHandle& compute_shader() const
      {
        return compute_shader_;
      }

      /**
      * @brief Returns a list of all texture handles currently attached to the compute pass
      * @return (const sulphur::foundation::Vector <sulphur::engine::TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& textures() const
      {
        return textures_;
      };

      /**
      * @brief Returns the texture handle that is attached at index
      * @param[in] index (size_t) The index of the texture
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(size_t index) const;

      /**
      * @brief Returns the texture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @return (const sulphur::engine::TextureHandle&) The texture handle
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(const foundation::String& name) const;

      /**
      * @brief Sets the texture at index to a new texture handle
      * @param[in] index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @remarks Asserts on valid index
      * @see sulphur::engine::Texture
      */
      void SetTexture(size_t index, const TextureHandle& texture);

      /**
      * @brief Sets the texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] texture (const TextureHandle&) The texture to set at this index
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      void SetTexture(const foundation::String& name, const TextureHandle& texture);

      /**
      * @brief Returns the number of textures attached to this compute pass
      * @return (size_t) The amount of textures attached
      */
      size_t NumTextures() const;

      /**
      * @brief Returns a list of all uav texture handles currently attached to the compute pass
      * @return (const sulphur::foundation::Vector <sulphur::engine::TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& uavs() const
      {
        return uavs_;
      };

      /**
      * @brief Returns the uav texture handle that is attached at index
      * @param[in] index (size_t) The index of the texture
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(size_t index) const;

      /**
      * @brief Returns the uav vtexture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @return (const TextureHandle&) The texture handle
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(const foundation::String& name) const;

      /**
      * @brief Sets the uav texture at index to a new texture handle
      * @param[in] index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @remarks Asserts on valid index
      * @see sulphur::engine::Texture
      */
      void SetUAV(size_t index, const TextureHandle& texture);

      /**
      * @brief Sets the uav texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      void SetUAV(const foundation::String& name, const TextureHandle& texture);

      /**
      * @brief Returns the number of uav textures attached to this compute pass
      * @return (size_t) The amount of textures attached
      */
      size_t NumUAVs() const;

      /**
      * @brief Returns the amount of thread in each work group of this compute pass
      * @return (const glm::uvec3&) The work group size
      */
      const glm::uvec3& GetWorkGroupSize() const;

      /**
      * @brief Returns a reference to the uniform buffer attached to this compute pass
      * @return (sulphur::engine::UniformBuffer&) A reference to the uniform buffer
      * @see sulphur::engine::UniformBuffer
      */
      UniformBuffer& GetUniformBuffer();

      /**
      * @brief Returns a reference to the uniform buffer attached to this compute pass
      * @return (const sulphur::engine::UniformBuffer&) A reference to the uniform buffer
      * @see sulphur::engine::UniformBuffer
      */
      const UniformBuffer& GetUniformBuffer() const;

    private:
      ComputeShaderHandle compute_shader_; //!< A handle to the compute shader

      UniformBuffer uniform_buffer_; //!< The uniform buffer used to interact with shader variables

      foundation::Vector<TextureHandle> textures_; //!< A collection of texture types used by this shader
      foundation::Vector<TextureHandle> uavs_; //!< A collection of UAV texture types used by this shader
    };

    /**
    * @class sulphur::engine::PostProcessMaterial
    * @brief Used for rendering image effects
    * @author Jelle de Haan
    */
    class PostProcessMaterial
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a material
      */
      PostProcessMaterial() {};
      
      /**
      * @brief Creates a post process material with a compute pass for this shader
      * @param[in] compute_shader (const sulphur::engine::ComputeShaderHandle&) The shader to use
      * @see sulphur::engine::ComputeShader
      * @see sulphur::engine::ComputePass
      */
      PostProcessMaterial(const ComputeShaderHandle& compute_shader);

      /**
      * @brief Creates a post process material with a compute pass for every shader
      * @param[in] shaders (std::initializer_list <sulphur::engine::ComputeShaderHandle>) The initializer list with all shaders
      * @see sulphur::engine::ComputeShader
      * @see sulphur::engine::ComputePass
      */
      PostProcessMaterial(std::initializer_list<ComputeShaderHandle> shaders);
      
      /**
      * @brief Add an extra compute pass to the back of the list
      * @param[in] pass (const sulphur::engine::ComputePass&) The compute pass to add
      * @see sulphur::engine::ComputePass
      */
      void AddComputePass(const ComputePass& pass);

      /**
      * @brief Gets the compute pass and a specific index
      * @param[in] index (size_t) The index of the compute pass
      * @remarks Asserts on valid index
      * @return (sulphur::engine::ComputePass&) A reference to the compute pass. (Can be used for editing textures)
      * @see sulphur::engine::ComputePass
      */
      ComputePass& GetComputePass(size_t index);

      /**
      * @brief Gets the compute pass and a specific index
      * @param[in] index (size_t) The index of the compute pass
      * @remarks Asserts on valid index
      * @return (const sulphur::engine::ComputePass&) A reference to the compute pass. (Can be used for editing textures)
      * @see sulphur::engine::ComputePass
      */
      const ComputePass& GetComputePass(size_t index) const;

      /**
      * @brief Sets the compute pass and a specific index
      * @param[in] index (size_t) The index of the compute pass
      * @param[in] pass (const sulphur::engine::ComputePass&) The compute pass to set to this index
      * @remarks Asserts on valid index
      * @see sulphur::engine::ComputePass
      */
      void SetComputePass(size_t index, const ComputePass& pass);

      /**
      * @brief Removes the compute pass and a specific index
      * @param[in] index (size_t) The index of the compute pass
      * @remarks Asserts on valid index, invalidates other indices
      */
      void RemoveComputePass(size_t index);

      /**
      * @return (size_t) The amount of compute passes currently attached to this PostProcessMaterial
      */
      size_t NumComputePasses() const;

      /**
      * @brief Returns a list of all texture handles currently attached to the compute pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& GetTextures(size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handles that is attached at index
      * @param[in] texure_index (size_t) The index of the texture
      * @param[in] pass_index (size_t) The index of the compute pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(size_t texture_index, size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] pass_index (size_t) The index of the compute pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(const foundation::String& name, size_t pass_index = 0) const;

      /**
      * @brief Sets the texture at index to a new texture handle
      * @param[in] texture_index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the compute pass
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      void SetTexture(size_t texture_index, const TextureHandle& texture, size_t pass_index = 0);

      /**
      * @brief Sets the texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] texture (const TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the compute pass
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      void SetTexture(const foundation::String& name, const TextureHandle& texture, size_t pass_index = 0);

      /**
      * @brief Returns a list of all texture handles currently attached to the compute pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& GetUAVs(size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handles that is attached at index
      * @param[in] texure_index (size_t) The index of the texture
      * @param[in] pass_index (size_t) The index of the compute pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(size_t uav_index, size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] pass_index (size_t) The index of the compute pass
      * @return (const TextureHandle&) The texture handle
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(const foundation::String& name, size_t pass_index = 0) const;

      /**
      * @brief Sets the texture at index to a new texture handle
      * @param[in] uav_index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the compute pass
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      void SetUAV(size_t uav_index, const TextureHandle& texture, size_t pass_index = 0);

      /**
      * @brief Sets the texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the compute pass
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      void SetUAV(const foundation::String& name, const TextureHandle& texture, size_t pass_index = 0);

    private:
      foundation::Vector<ComputePass> compute_passes_;
    };

    using PostProcessMaterialHandle = AssetHandle<PostProcessMaterial>; //!< Asset handle to a PostProcessMaterial
  }
}
