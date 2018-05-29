#pragma once

#include "engine/assets/asset_interfaces.h"
#include "engine/assets/shader.h"
#include "engine/assets/uniform_buffer.h"
#include "engine/assets/texture.h"

#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::MaterialPass
    * @brief Used in the material to render the same model using different shaders and pipeline parameters
    * @author Jelle de Haan
    */
    class MaterialPass
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a material pass
      */
      MaterialPass() {};

      /**
      * @brief Creates a material pass for this shader
      * @see sulphur::engine::Shader
      */
      MaterialPass(const ShaderHandle& shader);
      
      /**
      * @brief Replace the material pass with a new shader
      * @remarks Removes all textures and uniform buffers currently attached
      * @see sulphur::engine::Shader
      */
      void SetShader(const ShaderHandle& shader);

      /**
      * @brief Returns a shader handle to the currently attached shader
      * @return (const sulphur::engine::ShaderHandle&) The currently attached shader
      * @see sulphur::engine::Shader
      */
      const ShaderHandle& shader() const
      {
        return shader_;
      }

      /**
      * @brief Returns a list of all texture handles currently attached to the material pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of texture handles
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
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      void SetTexture(const foundation::String& name, const TextureHandle& texture);

      /**
      * @brief Returns the number of textures attached to this material pass
      * @return (size_t) The amount of textures attached
      */
      size_t NumTextures() const;

      /**
      * @brief Returns a list of all uav handles currently attached to the material pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of uav handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& uavs() const
      {
        return uavs_;
      };

      /**
      * @brief Returns the uav handle that is attached at index
      * @param[in] index (size_t) The index of the uav
      * @return (const sulphur::engine::TextureHandle&) The uav handle at index
      * @remarks Asserts on valid index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(size_t index) const;

      /**
      * @brief Returns the uav handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the uav
      * @return (const sulphur::engine::TextureHandle&) The uav handle
      * @remarks Asserts on valid name
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetUAV(const foundation::String& name) const;

      /**
      * @brief Sets the uav at index to a new uav handle
      * @param[in] index (size_t) The index of the uav
      * @param[in] uav (const sulphur::engine::TextureHandle&) The uav to set at this index
      * @remarks Asserts on valid index
      * @see sulphur::engine::UAV
      */
      void SetUAV(size_t index, const TextureHandle& uav);

      /**
      * @brief Sets the uav that matches the variable name in the shader to a new uav handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the uav
      * @param[in] uav (const sulphur::engine::TextureHandle&) The uav to set at this index
      * @remarks Asserts on valid name
      * @see sulphur::engine::UAV
      */
      void SetUAV(const foundation::String& name, const TextureHandle& uav);

      /**
      * @brief Returns the number of uavs attached to this material pass
      * @return (size_t) The amount of uavs attached
      */
      size_t NumUAVs() const;

      /**
      * @brief Returns a reference to the uniform buffer attached to this material pass
      * @param[in] shader_type (sulphur::engine::ShaderType) The shader to get the uniform buffer from
      * @return (sulphur::engine::UniformBuffer&) A reference to the uniform buffer
      * @see sulphur::engine::UniformBuffer
      */
      UniformBuffer& GetUniformBuffer(ShaderType shader_type = ShaderType::kPixel);

      /**
      * @brief Returns a reference to the uniform buffer attached to this material pass
      * @param[in] shader_type (sulphur::engine::ShaderType) The shader to get the uniform buffer from
      * @return (const sulphur::engine::UniformBuffer&) A reference to the uniform buffer
      * @see sulphur::engine::UniformBuffer
      */
      const UniformBuffer& GetUniformBuffer(ShaderType shader_type = ShaderType::kPixel) const;

    private:
      ShaderHandle shader_;

      UniformBuffer uniform_buffers_[5];

      foundation::Vector<TextureHandle> textures_;
      foundation::Vector<TextureHandle> uavs_;
    };

    /**
    * @class sulphur::engine::Material
    * @brief Used for rendering models
    * @author Jelle de Haan
    */
    class Material
    {
    public:
      /**
      * @brief Default empty constructor, use the other constructor for making a material
      */
      Material() {};
      
      /**
      * @brief Creates a material with a material pass for this shader
      * @param[in] shader (const sulphur::engine::ShaderHandle&) The shader to use
      * @see sulphur::engine::Shader
      * @see sulphur::engine::MaterialPass
      */
      Material(const ShaderHandle& shader);

      /**
      * @brief Creates a material with a material pass for every shader
      * @param[in] shaders (std::initializer_list <sulphur::engine::ShaderHandle>) The initializer list with all shaders
      * @see sulphur::engine::Shader
      * @see sulphur::engine::MaterialPass
      */
      Material(std::initializer_list<ShaderHandle> shaders);
      
      /**
      * @brief Add an extra material pass to the back of the list
      * @param[in] pass (const sulphur::engine::MaterialPass&) The material pass to add
      * @see sulphur::engine::MaterialPass
      */
      void AddMaterialPass(const MaterialPass& pass);

      /**
      * @brief Gets the material pass and a specific index
      * @param[in] index (size_t) The index of the material pass
      * @remarks Asserts on valid index
      * @return (sulphur::engine::MaterialPass&) A reference to the material pass. (Can be used for editing textures)
      * @see sulphur::engine::MaterialPass
      */
      MaterialPass& GetMaterialPass(size_t index);

      /**
      * @brief Gets the material pass and a specific index
      * @param[in] index (size_t) The index of the material pass
      * @remarks Asserts on valid index
      * @return (const sulphur::engine::MaterialPass&) A reference to the material pass. (Can be used for editing textures)
      * @see sulphur::engine::MaterialPass
      */
      const MaterialPass& GetMaterialPass(size_t index) const;

      /**
      * @brief Sets the material pass and a specific index
      * @param[in] index (size_t) The index of the material pass
      * @param[in] pass (const sulphur::engine::MaterialPass&) The material pass to set to this index
      * @remarks Asserts on valid index
      * @see sulphur::engine::MaterialPass
      */
      void SetMaterialPass(size_t index, const MaterialPass& pass);

      /**
      * @brief Removes the material pass and a specific index
      * @param[in] index (size_t) The index of the material pass
      * @remarks Asserts on valid index, invalidates other indices
      */
      void RemoveMaterialPass(size_t index);

      /**
      * @return (size_t) The amount of material passes currently attached to this material
      */
      size_t NumMaterialPasses() const;

      /**
      * @brief Returns a list of all texture handles currently attached to the material pass
      * @param[in] passIndex (size_t) The index of the material pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& GetTextures(size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handles that is attached at index
      * @param[in] texure_index (size_t) The index of the texture
      * @param[in] pass_index (size_t) The index of the material pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(size_t texture_index, size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] pass_index (size_t) The index of the material pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      const TextureHandle& GetTexture(const foundation::String& name, size_t pass_index = 0) const;

      /**
      * @brief Sets the texture at index to a new texture handle
      * @param[in] texture_index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the material pass
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::Texture
      */
      void SetTexture(size_t texture_index, const TextureHandle& texture, size_t pass_index = 0);

      /**
      * @brief Sets the texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const foundation::String&) The variable name of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the material pass
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::Texture
      */
      void SetTexture(const foundation::String& name, const TextureHandle& texture, size_t pass_index = 0);
      
      /**
      * @brief Returns a list of all uav texture handles currently attached to the material pass
      * @return (const sulphur::foundation::Vector <TextureHandle>&) The list of texture handles
      * @see sulphur::engine::Texture
      */
      const foundation::Vector<TextureHandle>& GetUAVs(size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handles that is attached at index
      * @param[in] texure_index (size_t) The index of the texture
      * @param[in] pass_index (size_t) The index of the material pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle at index
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::UAV
      */
      const TextureHandle& GetUAV(size_t texture_index, size_t pass_index = 0) const;

      /**
      * @brief Returns the texture handle that matches the variable name in the shader
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] pass_index (size_t) The index of the material pass
      * @return (const sulphur::engine::TextureHandle&) The texture handle
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::UAV
      */
      const TextureHandle& GetUAV(const foundation::String& name, size_t pass_index = 0) const;

      /**
      * @brief Sets the texture at index to a new texture handle
      * @param[in] texture_index (size_t) The index of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the material pass
      * @remarks Asserts on valid texture and pass index
      * @see sulphur::engine::UAV
      */
      void SetUAV(size_t texture_index, const TextureHandle& texture, size_t pass_index = 0);

      /**
      * @brief Sets the texture that matches the variable name in the shader to a new texture handle
      * @param[in] name (const sulphur::foundation::String&) The variable name of the texture
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to set at this index
      * @param[in] pass_index (size_t) The index of the material pass
      * @remarks Asserts on valid name and pass index
      * @see sulphur::engine::UAV
      */
      void SetUAV(const foundation::String& name, const TextureHandle& texture, size_t pass_index = 0);

    private:
      foundation::Vector<MaterialPass> material_passes_;
    };

    using MaterialHandle = AssetHandle<Material>; //!< Asset handle to a material
  }
}
