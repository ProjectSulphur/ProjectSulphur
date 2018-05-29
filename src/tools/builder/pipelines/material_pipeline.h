#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include <foundation/pipeline-assets/material.h>
#include <foundation/containers/vector.h>
#include <foundation/pipeline-assets/shader.h>
#include <glm/glm.hpp>

struct aiNode;
struct aiScene;
struct aiMaterial;
enum aiTextureType;

namespace sulphur 
{
  namespace foundation 
  {
    struct ModelTextureCache;
  }

  namespace builder
  {
    enum struct ModelFileType : unsigned char;
    class ShaderPipeline;
    class TexturePipeline;

    /**
     * @brief Function pointer typedef for get the amount of shader resources.
     */
    using GetShaderResourceCount = size_t(*)(const foundation::ShaderData&);
    /**
    * @brief Function pointer typedef for get the shader resources.
    */
    using GetShaderResource = foundation::ShaderResource(*)(const foundation::ShaderData&, int);

    /**
     * @class sulphur::builder::MaterialPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of materials.
     * @author Timo van Hees
     */
    class MaterialPipeline : public PipelineBase
    {
    public:
      MaterialPipeline() = default;
      ~MaterialPipeline() = default;

      /**
       * @brief Creates materials from the material information present in the scene.
       * @param[in] scene (const aiScene*) The scene containing the material information.
       * @param[in] scene_directory (const sulphur::foundation::Path&) 
       * Directory of the scene file. Ending with '/'.
       * @param[in] model_file_type (sulpher::builder::ModelFileType) The file type of the scene.
       * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) 
       * The shader pipeline used to package the shaders used by this material.
       * @param[in] texture_cache (sulphur::foundation::ModelTextureCache&) 
       * Texture cache with the textures used by the materials in the scene.
       * @param[in] vertex_shader (const sulphur::foundation::AssetName&) The name of the
       * packaged vertex shader.
       * @param[in] pixel_shader (const sulphur::foundation::AssetName&) The name of the
       * packaged pixel shader.
       * @param[out] materials (sulphur::foundation::Vector <sulphur::foundation::MaterialAsset>&)
       * List of materials created from the materials in the scene.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the materials should be discarded.
       */
      bool Create(const aiScene* scene, 
        const foundation::Path& scene_directory,
        ModelFileType model_file_type,
        ShaderPipeline& shader_pipeline, 
        foundation::ModelTextureCache& texture_cache, 
        const foundation::AssetName& vertex_shader,
        const foundation::AssetName& pixel_shader,
        foundation::Vector<foundation::MaterialAsset>& materials) const;

      /**
      * @brief Creates a shader based material.
      * @param[in] name (const sulphur::foundation::String&) The name of the material.
      * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) The shader
      * pipeline that packaged the shaders.
      * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) The texture
      * pipeline that packaged the textures.
      * @param[in] vertex_shader (const sulphur::foundation::AssetName&) The name of the
      * packaged vertex shader.
      * @param[in] pixel_shader (const sulphur::foundation::AssetName&) The name of the
      * packaged pixel shader.
      * @param[in] uniform_buffer_data (sulphur::foundation::Vector <sulphur::foundation::UniformBufferData>&)
      * Data for the uniform buffers.
      * @param[in] textures (sulphur::foundation::Vector <sulphur::foundation::AssetID>&)
      * The textures for each texture binding.
      * @param[in] sampler_data (sulphur::foundation::Vector <sulphur::foundation::SamplerData>&)
      * The settings for each sampler.
      * @param[out] material (sulphur::builder::Material) The material created from the input.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the material should be discarded.
      */
      bool Create(const foundation::String& name, ShaderPipeline& shader_pipeline,
        TexturePipeline& texture_pipeline, const foundation::AssetName& vertex_shader,
         const foundation::AssetName& pixel_shader,
        const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::Vector<foundation::AssetID>& textures,
        const foundation::Vector<foundation::SamplerData>& sampler_data, foundation::MaterialAsset& material);

      /**
      * @brief Creates a shader based material.
      * @param[in] name (const sulphur::foundation::String&) The name of the material.
      * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) The shader
      * pipeline that packaged the shaders.
      * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) The texture
      * pipeline that packaged the textures.
      * @param[in] vertex_shader (const sulphur::foundation::AssetName&) The name of the
      * packaged vertex shader.
      * @param[in] geometry_shader (const sulphur::foundation::AssetName&) The name of the
      * packaged geometry shader.
      * @param[in] pixel_shader (const sulphur::foundation::AssetName&) The name of the
      * packaged pixel shader.
      * @param[in] uniform_buffer_data (sulphur::foundation::Vector <sulphur::foundation::UniformBufferData>&)
      * Data for the uniform buffers.
      * @param[in] textures (sulphur::foundation::Vector <sulphur::foundation::AssetID>&)
      * The textures for each texture binding.
      * @param[in] sampler_data (sulphur::foundation::Vector <sulphur::foundation::SamplerData>&)
      * The settings for each sampler.
      * @param[out] material (sulphur::builder::Material) The material created from the input.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the material should be discarded.
      */
      bool Create(const foundation::String& name, ShaderPipeline& shader_pipeline,
        TexturePipeline& texture_pipeline,  const foundation::AssetName& vertex_shader,
         const foundation::AssetName& geometry_shader,  const foundation::AssetName& pixel_shader,
        const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::Vector<foundation::AssetID>& textures,
        const foundation::Vector<foundation::SamplerData>& sampler_data, foundation::MaterialAsset& material);

      /**
       * @brief Creates a shader based material.
       * @param[in] name (const sulphur::foundation::String&) The name of the material.
       * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) The shader 
       * pipeline that packaged the shaders.
       * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) The texture 
       * pipeline that packaged the textures.
       * @param[in] vertex_shader (sulphur::foundation::AssetID) The ID of the 
       * packaged vertex shader.
       * @param[in] pixel_shader (sulphur::foundation::AssetID) The ID of the 
       * packaged pixel shader.
       * @param[in] uniform_buffer_data (sulphur::foundation::Vector <sulphur::foundation::UniformBufferData>&) 
       * Data for the uniform buffers.
       * @param[in] textures (sulphur::foundation::Vector <sulphur::foundation::AssetID>&)
       * The textures for each texture binding.
       * @param[in] sampler_data (sulphur::foundation::Vector <sulphur::foundation::SamplerData>&)
       * The settings for each sampler.
       * @param[out] material (sulphur::builder::Material) The material created from the input.
       * @return (bool) False when there was an error that couldn't be recovered from.
       * @remark If the function returned false, the material should be discarded.
       */
      bool Create(const foundation::String& name, ShaderPipeline& shader_pipeline, 
        TexturePipeline& texture_pipeline, foundation::AssetID vertex_shader, 
        foundation::AssetID pixel_shader, 
        const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::Vector<foundation::AssetID>& textures,
        const foundation::Vector<foundation::SamplerData>& sampler_data, foundation::MaterialAsset& material);

      /**
      * @brief Creates a shader based material.
      * @param[in] name (const sulphur::foundation::String&) The name of the material.
      * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) The shader
      * pipeline that packaged the shaders.
      * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) The texture 
      * pipeline that packaged the textures.
      * @param[in] vertex_shader (sulphur::foundation::AssetID) The ID of the
      * packaged vertex shader.
      * @param[in] geometry_shader (sulphur::foundation::AssetID) The ID of the
      * packaged geometry shader.
      * @param[in] pixel_shader (sulphur::foundation::AssetID) The ID of the
      * packaged pixel shader.
      * @param[in] uniform_buffer_data (sulphur::foundation::Vector <sulphur::foundation::UniformBufferData>&)
      * Data for the uniform buffers.
      * @param[in] textures (sulphur::foundation::Vector <sulphur::foundation::AssetID>&)
      * The textures for each texture binding.
      * @param[in] sampler_data (sulphur::foundation::Vector <sulphur::foundation::SamplerData>&)
      * The settings for each sampler.
      * @param[out] material (sulphur::builder::Material) The material created from the input.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the material should be discarded.
      */
      bool Create(const foundation::String& name, ShaderPipeline& shader_pipeline, 
        TexturePipeline& texture_pipeline, foundation::AssetID vertex_shader, 
        foundation::AssetID geometry_shader, foundation::AssetID pixel_shader, 
        const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::Vector<foundation::AssetID>& textures, 
        const foundation::Vector<foundation::SamplerData>& sampler_data, foundation::MaterialAsset& material);

      /**
       * @brief Creates a texture cache for all materials in the scene.
       * @param[in] scene (const aiScene*) The scene containing the material information.
       * @param[in] scene_directory (const sulphur::foundation::Path&) 
       * Directory of the scene file. Ending with '/'.
       * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) 
       * The texture pipeline to use to create the textures.
       * @param[out] texture_cache (sulphur::builder::foundation::ModelTextureCache&)
       * The texture cache to be filled with textures.
       * @return (bool) False when there was an error that couldn't be recovered from.
      *  @remark If the function returned false, the texture cache should be discarded.
       */
      bool CreateTextureCache(const aiScene* scene,
        const foundation::Path& scene_directory,
        TexturePipeline& texture_pipeline,
        foundation::ModelTextureCache& texture_cache) const;

      /**
       * @brief Adds a material to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] material (sulphur::foundation::MaterialAsset&) The material to add to the package.
       * @return True if the material was added to the package succesfully.
       */
      bool PackageMaterial(const foundation::Path& asset_origin, foundation::MaterialAsset& material);

      /**
       * @brief Packages the textures in the texture cache and assigns the 
       * correct texture ids to the materials.
       * @param[in] texture_cache (sulphur::builder::foundation::ModelTextureCache&) 
       * Texture cache containing the textures used in the scene.
       * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) 
       * Texture pipeline to use to package the textures.
       * @param[in] materials (foundation::Vector <foundation::MaterialAsset>&)
       * Materials created from the scene.
       * @return (bool) False when there was an error that couldn't be recovered from.
       * @remark If the function returned false, the texture cache and the 
       * materials should be discarded.
       */
      bool PackageTextureCache(foundation::ModelTextureCache& texture_cache,
        TexturePipeline& texture_pipeline,
        foundation::Vector<foundation::MaterialAsset>& materials) const;

      /**
       * @see sulphur::builder::PipelineBase::GetPackageExtension
       */
      foundation::String GetPackageExtension() const override;

      /*
      * @see sulphur::builder::PipelineBase::PackageDefaultAssets
      */
      bool PackageDefaultAssets() override;

      /**
       * @see sulphur::builder::PipelineBase::GetCacheName 
       */
      foundation::String GetCacheName() const override;

    private:
      /**
       * @brief Loads a texture from an assimp material.
       * @param[in] ai_mat (const aiMaterial*) The assimp material.
       * @param[in] texture_type (aiTextureType) The texture type to load from the material.
       * @param[in] scene_directory (const sulphur::foundation::Path&) 
       * Directory of the scene file. Ending with '/'.
       * @param[in] texture_cache (sulphur::builder::foundation::ModelTextureCache&) 
       * Texture cache containing the texture used in the scene.
       * @param[in] textures (const sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&)
       * List of textures defined in the shaders.
       * @param[out] material (sulphur::foundation::MaterialData&) Material to add the textures to.
       * @return (bool) False when there was an error that couldn't be recovered from.
       */
      bool LoadTexture(const aiMaterial* ai_mat, aiTextureType texture_type,
        const foundation::Path& scene_directory, foundation::ModelTextureCache& texture_cache, 
        const foundation::Vector<foundation::ShaderResource>& textures,
        foundation::MaterialData& material) const;

      /**
       * @brief Get the combined reflection data from the shaders.
       * @param[in] shader_pipeline (sulphur::builder::ShaderPipeline&) 
       * Shader pipeline used to load shaders from the packages. 
       * @param[in] vertex_shader (sulphur::foundation::AssetID&) The ID of the vertex shader.
       * @param[in] geometry_shader (sulphur::foundation::AssetID&) The ID of the geometry shader.
       * @param[in] pixel_shader (sulphur::foundation::AssetID&) The ID of the pixel shader.
       * @param[out] uniform_buffers (sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&) 
       * Output for unique uniform buffers.
       * @param[out] textures (sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&) 
       * Output for unique texture units.
       * @param[out] samplers (sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&) 
       * Output for unique sampler units
       * @return (bool) False if the shaders are incompatible.
       */
      bool GetCombinedReflectedShaderData(ShaderPipeline& shader_pipeline,
        foundation::AssetID& vertex_shader,
        foundation::AssetID& geometry_shader,
        foundation::AssetID& pixel_shader,
        foundation::Vector<foundation::ShaderResource>& uniform_buffers,
        foundation::Vector<foundation::ShaderResource>& textures,
        foundation::Vector<foundation::ShaderResource>& samplers) const;

      /**
       * @brief Creates a list of unique resources from a set of shaders.
       * @param[in] vertex_shader_asset (const sulphur::foundation::ShaderData&) 
       * The vertex shader.
       * @param[opt|in] geometry_shader_asset (const sulphur::foundation::ShaderData&) 
       * The geometry shader.
       * @param[in] pixel_shader_asset (const sulphur::foundation::ShaderData&) 
       * The pixel shader.
       * @param[in] get_shader_resource_count (sulphur::builder::GetShaderResourceCount) 
       * Function that returns the number of resources in a shader.
       * @param[in] get_shader_resource (sulphur::builder::GetShaderResource) 
       * Function that returns a resource in a shader.
       * @param[out] unique_resource_list (sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&)
       * Output list of unique resources.
       * @return (bool) False if the shaders have conflicting resources.
       */
      bool ShaderResourceLoop(const foundation::ShaderData& vertex_shader_asset, 
        const foundation::ShaderData* geometry_shader_asset, 
        const foundation::ShaderData& pixel_shader_asset, 
        GetShaderResourceCount get_shader_resource_count, 
        GetShaderResource get_shader_resource, 
        foundation::Vector<foundation::ShaderResource>& unique_resource_list) const;

      /**
       * @brief Finds shader resources with a given name in a vector.
       * @param[in] shader_resources (const sulphur::foundation::Vector<foundation::ShaderResource>&)
       * List of shader resources.
       * @param[in] name (const sulphur::foundation::String&) The name of the resource to look for.
       * @return (sulphur::foundation::Vector <sulphur::foundation::ShaderResource>)
       * Shader resources matching the given name.
       */
      foundation::Vector<foundation::ShaderResource> FindShaderResources(
        const foundation::Vector<foundation::ShaderResource>& shader_resources,
        const foundation::String& name) const;

      /**
      * @see sulphur::builder::MaterialPipeline::FindAndSetUniformInternal
      */
      void FindAndSetUniform(const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
        foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::String& uniform_name, float v) const;

      /**
      * @see sulphur::builder::MaterialPipeline::FindAndSetUniformInternal
      */
      void FindAndSetUniform(const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
        foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::String& uniform_name, const glm::vec2& v) const;

      /**
      * @see sulphur::builder::MaterialPipeline::FindAndSetUniformInternal
      */
      void FindAndSetUniform(const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
        foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::String& uniform_name, const glm::vec3& v) const;

      /**
      * @see sulphur::builder::MaterialPipeline::FindAndSetUniformInternal
      */
      void FindAndSetUniform(const foundation::Vector<foundation::ShaderResource>& uniform_buffers, 
        foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data, 
        const foundation::String& uniform_name, const glm::vec4& v) const;

      /**
       * @brief Finds and then sets a uniform buffer member's data, if found.
       * @tparam T The type of the variable.
       * @param[in] uniform_buffers (const sulphur::foundation::Vector<foundation::ShaderResource>&)
       * The uniform buffers used by the shaders.
       * @param[in] uniform_buffer_data (sulphur::foundation::Vector<foundation::UniformBufferData>&)
       * The uniform buffer data to be stored with the material.
       * @param[in] uniform_name (const sulphur::foundation::String&) The uniform name.
       * @param[in] v (const T&) The new value.
       * @param[in] concrete_type (sulphur::foundation::ShaderResource::ConcreteType)
       * The type the variable is supposed to have.
       * @param[in] type_name (const sulphur::foundation::String&) 
       * The name of the type. Used to report error.
       */
      template <typename T>
      void FindAndSetUniformInternal(
        const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
        foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
        const foundation::String& uniform_name, const T& v, 
        foundation::ShaderResource::ConcreteType concrete_type, 
        const foundation::String& type_name) const;

      /**
       * @brief Looks for texture units with a cetain name and sets the given texture for all of them.
       * @param[in] textures (const sulphur::foundation::Vector <sulphur::foundation::ShaderResource>&)
       * Texture resources containing information about the texture units.
       * @param[out] texture_data (sulphur::foundation::Vector <sulphur::foundation::AssetID>&)
       * The texture data of the material.
       * @param[in] texture_name (const sulphur::foundation::String&) The name of the texture unit.
       * @param[in] texture_id (sulphur::foundation::AssetID) The texture.
       */
      void FindAndSetTexture(const foundation::Vector<foundation::ShaderResource>& textures,
        foundation::Vector<foundation::AssetID>& texture_data,
        const foundation::String& texture_name, foundation::AssetID texture_id) const;
    };

    /**
     * @see sulphur::builder::MaterialPipeline::FindAndSetUniformInternal 
     */
    template <typename T>
    void MaterialPipeline::FindAndSetUniformInternal(
      const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::String& uniform_name, const T& v,
      foundation::ShaderResource::ConcreteType concrete_type, 
      const foundation::String& type_name) const
    {
      for (int i = 0; i < uniform_buffers.size(); ++i)
      {
        foundation::Vector<foundation::ShaderResource> found_uniforms =
          FindShaderResources(uniform_buffers[i].members, uniform_name);

        for (const foundation::ShaderResource& uniform : found_uniforms)
        {
          if (uniform.concrete_type != concrete_type)
          {
            PS_LOG_BUILDER(Error,
              "Shader has a uniform called %s. This is a reserved name. The type must be a %s.",
              uniform_name.c_str(), type_name.c_str());
            return;
          }

          T* buffer_var = reinterpret_cast<T*>(uniform_buffer_data[i].data.data() + uniform.offset);
          if (*buffer_var == T{})
          {
            memcpy(buffer_var, &v, sizeof(v));
          }
        }
      }
    }
  }
}
