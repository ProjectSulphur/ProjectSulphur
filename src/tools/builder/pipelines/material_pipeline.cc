#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/shared/util.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/io/binary_writer.h>
#include <foundation/logging/logger.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/pipeline-assets/model.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>


namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    bool MaterialPipeline::Create(const aiScene* scene,
      const foundation::Path& scene_directory,
      ModelFileType model_file_type,
      ShaderPipeline& shader_pipeline, 
      foundation::ModelTextureCache& texture_cache,
      const foundation::AssetName& vertex_shader,
      const foundation::AssetName& pixel_shader,
      foundation::Vector<foundation::MaterialAsset>& materials) const
    {
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error, 
          "scene == nullptr. No materials created.");
        return false;
      }

      foundation::AssetID vertex_shader_id = foundation::GenerateId(vertex_shader);
      foundation::AssetID geometry_shader_id = 0;
      foundation::AssetID pixel_shader_id = foundation::GenerateId(pixel_shader);

      // Get shader reflection data
      foundation::Vector<foundation::ShaderResource> uniform_buffers = {};
      foundation::Vector<foundation::ShaderResource> textures = {};
      foundation::Vector<foundation::ShaderResource> samplers = {};
      if (GetCombinedReflectedShaderData(shader_pipeline, vertex_shader_id, geometry_shader_id,
        pixel_shader_id, uniform_buffers, textures, samplers) == false)
      {
        PS_LOG_BUILDER(Error,
          "Vertex and fragment shader are incompatible.");
        return false;
      }

      const size_t initial_size = materials.size();

      materials.reserve(initial_size + scene->mNumMaterials);
      for (unsigned int i = 0u; i < scene->mNumMaterials; ++i)
      {
        const aiMaterial* ai_mat = scene->mMaterials[i];
        foundation::MaterialAsset material = {};

        // Get the name
        aiString ai_string = {};
        if (ai_mat->Get(AI_MATKEY_NAME, ai_string) != AI_SUCCESS)
        {
          PS_LOG_BUILDER(Error,
            "Material has no name. All assets must have names. Results should be discarded.");
          return false;
        }
        material.name = ai_string.C_Str();

        // Set the shaders
        material.data.vertex_shader_id = vertex_shader_id;
        material.data.pixel_shader_id = pixel_shader_id;

        // Create shader resources
        material.data.uniform_buffers.resize(uniform_buffers.size());
        for(int j = 0; j < uniform_buffers.size(); ++j)
        {
          material.data.uniform_buffers[j].data.resize(uniform_buffers[j].size);
        }
        material.data.separate_images.resize(textures.size(), 0);
        material.data.separate_samplers.resize(samplers.size());

        // Model type specific data
        if(model_file_type == ModelFileType::kFBX)
        {
          
        }
        else if(model_file_type == ModelFileType::kOBJ)
        {
          
        }
        else if(model_file_type == ModelFileType::kglTF)
        {
          float ai_float = 0.0f;
          if (ai_mat->Get("$mat.gltf.pbrMetallicRoughness.metallicFactor", 0, 0, 
            ai_float) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, 
              "ps_float_metallic", ai_float);
          }

          if (ai_mat->Get("$mat.gltf.pbrMetallicRoughness.roughnessFactor", 0, 0,
            ai_float) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers,
              "ps_float_roughness", ai_float);
          }
        }

        {
          // Get ambient color
          aiColor4D ai_color4d = {};
          if (ai_mat->Get(AI_MATKEY_COLOR_AMBIENT, ai_color4d) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_color_ambient",
              *reinterpret_cast<glm::vec4*>(&ai_color4d));
          }

          // Get diffuse color
          if (ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color4d) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_color_diffuse",
              *reinterpret_cast<glm::vec4*>(&ai_color4d));
          }

          // Get specular color
          if (ai_mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_color4d) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_color_specular",
              *reinterpret_cast<glm::vec4*>(&ai_color4d));
          }

          // Get emissive color
          if (ai_mat->Get(AI_MATKEY_COLOR_EMISSIVE, ai_color4d) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_color_emissive",
              *reinterpret_cast<glm::vec4*>(&ai_color4d));
          }

          // Get wireframe enabled
          int ai_int = 0;
          if (ai_mat->Get(AI_MATKEY_ENABLE_WIREFRAME, ai_int) == AI_SUCCESS)
          {
            material.data.wireframe = ai_int > 0;
          }

          // Get backface_culling
          if (ai_mat->Get(AI_MATKEY_TWOSIDED, ai_int) == AI_SUCCESS)
          {
            material.data.backface_culling = ai_int <= 0;
          }

          // Get blend function
          if (ai_mat->Get(AI_MATKEY_BLEND_FUNC, ai_int) == AI_SUCCESS)
          {
            switch(ai_int)
            {
            case aiBlendMode_Default:
              material.data.blend_function = foundation::BlendMode::kDefault;
              break;
            case aiBlendMode_Additive:
              material.data.blend_function = foundation::BlendMode::kAdditive;
              break;
            default:
              material.data.blend_function = foundation::BlendMode::kNone;
              break;
            }
          }

          // Get opacity
          float ai_float = 0.0f;
          if (ai_mat->Get(AI_MATKEY_OPACITY, ai_float) == AI_SUCCESS)
          {
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_float_opacity",
              ai_float);

            if(ai_float >= 1.0f)
              material.data.blend_function = foundation::BlendMode::kNone;
            else if(material.data.blend_function == foundation::BlendMode::kNone)
              material.data.blend_function = foundation::BlendMode::kDefault;
          }

          // Get roughness
          if (ai_mat->Get(AI_MATKEY_SHININESS, ai_float) == AI_SUCCESS)
          {
            ai_float = 1.0f - ai_float * 0.001f;  // Shininess is oposite of roughness
            FindAndSetUniform(uniform_buffers, material.data.uniform_buffers, "ps_float_roughness",
              ai_float);
          }
        }

        {
          // Get albedo texture
          if (LoadTexture(ai_mat, aiTextureType_DIFFUSE, scene_directory, texture_cache,
            textures, material.data) == false)
          {
            return false;
          }

          // Get normal texture
          if (LoadTexture(ai_mat, aiTextureType_NORMALS, scene_directory, texture_cache,
            textures, material.data) == false)
          {
            return false;
          }

          // Get metallic texture
          if (LoadTexture(ai_mat, aiTextureType_SPECULAR, scene_directory, texture_cache,
            textures, material.data) == false)
          {
            return false;
          }

          // Get roughness texture
          if (LoadTexture(ai_mat, aiTextureType_SHININESS, scene_directory, texture_cache,
            textures, material.data) == false)
          {
            return false;
          }
        }

        materials.push_back(material);
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::Create(const foundation::String& name,
      ShaderPipeline& shader_pipeline, TexturePipeline& texture_pipeline,
      const foundation::AssetName& vertex_shader,
      const foundation::AssetName& pixel_shader,
      const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::Vector<foundation::AssetID>& textures,
      const foundation::Vector<foundation::SamplerData>& sampler_data,
      foundation::MaterialAsset& material)
    {
      return Create(name, shader_pipeline, texture_pipeline, 
        foundation::GenerateId(vertex_shader), 0, 
        foundation::GenerateId(pixel_shader), uniform_buffer_data, textures, 
        sampler_data, material);
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::Create(const foundation::String& name,
      ShaderPipeline& shader_pipeline, TexturePipeline& texture_pipeline,
      const foundation::AssetName& vertex_shader,
      const foundation::AssetName& geometry_shader,
      const foundation::AssetName& pixel_shader,
      const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::Vector<foundation::AssetID>& textures,
      const foundation::Vector<foundation::SamplerData>& sampler_data,
      foundation::MaterialAsset& material)
    {
      return Create(name, shader_pipeline, texture_pipeline, foundation::GenerateId(vertex_shader), 
        foundation::GenerateId(geometry_shader), foundation::GenerateId(pixel_shader),
        uniform_buffer_data, textures, sampler_data, material);
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::Create(const foundation::String& name, 
      ShaderPipeline& shader_pipeline,
      TexturePipeline& texture_pipeline,
      foundation::AssetID vertex_shader, 
      foundation::AssetID pixel_shader,
      const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::Vector<foundation::AssetID>& textures,
      const foundation::Vector<foundation::SamplerData>& sampler_data,
      foundation::MaterialAsset& material)
    {
      return Create(name, shader_pipeline, texture_pipeline, vertex_shader, 0, pixel_shader, 
        uniform_buffer_data, textures, sampler_data, material);
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::Create(const foundation::String& name, 
      ShaderPipeline& shader_pipeline,
      TexturePipeline& texture_pipeline,
      foundation::AssetID vertex_shader, 
      foundation::AssetID geometry_shader,
      foundation::AssetID pixel_shader,
      const foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::Vector<foundation::AssetID>& textures,
      const foundation::Vector<foundation::SamplerData>& sampler_data,
      foundation::MaterialAsset& material)
    {
      foundation::ShaderData vertex_shader_asset = {};
      if(shader_pipeline.LoadAssetFromPackage(vertex_shader, vertex_shader_asset) == false)
      {
        PS_LOG_BUILDER(Error,
          "Vertex shader with id(%llu) couldn't be loaded from package.", vertex_shader);
        return false;
      }

      foundation::ShaderData geometry_shader_asset = {};
      if (geometry_shader != 0)
      {
        if (shader_pipeline.LoadAssetFromPackage(geometry_shader, geometry_shader_asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Geometry shader with id(%llu) couldn't be loaded from package.", geometry_shader);
          return false;
        }
      }

      foundation::ShaderData pixel_shader_asset = {};
      if (shader_pipeline.LoadAssetFromPackage(pixel_shader, pixel_shader_asset) == false)
      {
        PS_LOG_BUILDER(Error,
          "Pixel shader with id(%llu) couldn't be loaded from package.", pixel_shader);
        return false;
      }

      foundation::Vector<foundation::TextureData> texture_assets(textures.size());
      for(int i = 0; i < textures.size(); ++i)
      {
        foundation::AssetID id = textures[i];
        if (texture_pipeline.LoadAssetFromPackage(id, texture_assets[i]) == false)
        {
          PS_LOG_BUILDER(Error,
            "Texture with id(%llu) couldn't be loaded from package.", id);
          return false;
        }
      }

      material.name = name;
      material.data.vertex_shader_id = vertex_shader;
      material.data.geometry_shader_id = geometry_shader;
      material.data.pixel_shader_id = pixel_shader;
      material.data.uniform_buffers = uniform_buffer_data;
      material.data.separate_images = textures;
      material.data.separate_samplers = sampler_data;

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::CreateTextureCache(const aiScene* scene, 
      const foundation::Path& scene_directory,
      TexturePipeline& texture_pipeline,
      foundation::ModelTextureCache& texture_cache) const
    {
      const auto create_texture_func = [](const foundation::Path& scene_directory, 
        foundation::ModelTextureCache& texture_cache, 
        TexturePipeline& texture_pipeline, 
        const aiMaterial* ai_mat, 
        aiTextureType texture_type)
      {
        if (ai_mat->GetTextureCount(texture_type) > 0)
        {
          aiString ai_string = {};
          ai_mat->GetTexture(texture_type, 0, &ai_string);
          const foundation::Path texture_filepath = scene_directory.GetString() +
            foundation::String(ai_string.C_Str());

          if (texture_cache.texture_lookup.find(texture_filepath.GetString()) ==
            texture_cache.texture_lookup.end())
          {
            foundation::TextureAsset texture = {};
            if (texture_pipeline.Create(texture_filepath, texture) == true)
            {
              texture_cache.textures.push_back(eastl::move(texture));
              texture_cache.texture_lookup[texture_filepath] = 
                static_cast<int>(texture_cache.textures.size()) - 1;
            }
          }
        }
      };

      for (unsigned int i = 0u; i < scene->mNumMaterials; ++i)
      {
        const aiMaterial* ai_mat = scene->mMaterials[i];

        // Get albedo texture
        create_texture_func(scene_directory, texture_cache, texture_pipeline, ai_mat, 
          aiTextureType_DIFFUSE);

        // Get normal texture
        create_texture_func(scene_directory, texture_cache, texture_pipeline, ai_mat,
          aiTextureType_NORMALS);

        // Get metallic texture
        create_texture_func(scene_directory, texture_cache, texture_pipeline, ai_mat,
          aiTextureType_SPECULAR);

        // Get roughness texture
        create_texture_func(scene_directory, texture_cache, texture_pipeline, ai_mat,
          aiTextureType_SHININESS);
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::PackageMaterial(const foundation::Path& asset_origin, 
      foundation::MaterialAsset& material)
    {
      if (material.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error, 
          "Material name not initialized. The material will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if(RegisterAsset(asset_origin, material.name, output_file, material.id) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to register the material. The material will not be packaged.");
        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(material.data);

      if (writer.Save() == false)
      {
        PS_LOG_BUILDER(Error, 
          "Failed to package material.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::PackageTextureCache(foundation::ModelTextureCache& texture_cache,
      TexturePipeline& texture_pipeline,
      foundation::Vector<foundation::MaterialAsset>& materials) const
    {
      for(const eastl::pair<foundation::Path, int> it : texture_cache.texture_lookup)
      {
        for(foundation::MaterialAsset& material: materials)
        {
          for(foundation::AssetID& texture_id : material.data.separate_images)
          {
            if(texture_id == it.second)
            {
              if(texture_id == 0)
              {
                if(texture_pipeline.AssetExists("ps_default_texture") == false)
                {
                  PS_LOG_BUILDER(Error,
                    "Default material is not in the cache.");
                  return false;
                }

                texture_id = foundation::GenerateId("ps_default_texture");
              }
              else
              {
                foundation::TextureAsset& texture = texture_cache.textures[it.second];
                if (texture.id == 0)
                {
      
                  if (texture_pipeline.PackageTexture(it.first, texture) == false)
                  {
                    PS_LOG_BUILDER(Error,
                      "Failed to package texture cache.");
                    return false;
                  }
                }

                texture_id = texture.id;
              }
            }
          }
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String MaterialPipeline::GetPackageExtension() const
    {
      return "sma";
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::PackageDefaultAssets()
    {
      if (AssetExists("ps_default_material") == false)
      {
        foundation::MaterialAsset asset = {};
        asset.name = "ps_default_material";
        asset.data.vertex_shader_id = foundation::GenerateId("ps_default_vertex_shader");
        asset.data.pixel_shader_id = foundation::GenerateId("ps_default_pixel_shader");

        if (PackageMaterial(ASSET_ORIGIN_USER, asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to package default asset.");
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String MaterialPipeline::GetCacheName() const
    {
      return "material_package";
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::LoadTexture(const aiMaterial* ai_mat, 
      aiTextureType texture_type,
      const foundation::Path& scene_directory,
      foundation::ModelTextureCache& texture_cache, 
      const foundation::Vector<foundation::ShaderResource>& textures, 
      foundation::MaterialData& material) const
    {
      if (ai_mat->GetTextureCount(texture_type) > 0)
      {
        foundation::String texture_name = "";
        switch (texture_type)
        {
        case aiTextureType_DIFFUSE: texture_name = "ps_texture_albedo"; break;
        case aiTextureType_SPECULAR: texture_name = "ps_texture_metallic"; break;
        case aiTextureType_AMBIENT: texture_name = "ps_texture_occlusion"; break;
        case aiTextureType_EMISSIVE: texture_name = "ps_texture_emissive"; break;
        case aiTextureType_SHININESS: texture_name = "ps_texture_roughness"; break;
        case aiTextureType_NORMALS: texture_name = "ps_texture_normals"; break;
        default:
        {
          PS_LOG_BUILDER(Error,
            "Trying to load a texture of an unknown type.");
          return false;
        }
        }

        aiString ai_string = {};
        ai_mat->GetTexture(texture_type, 0, &ai_string);
        const foundation::Path texture_filepath = scene_directory + 
          foundation::Path(ai_string.C_Str());

        const eastl::map<foundation::Path, int>::iterator it = 
          texture_cache.texture_lookup.find(texture_filepath);

        foundation::AssetID texture_id = 0;

        if(it != texture_cache.texture_lookup.end())
        {
          texture_id = it->second;
        }

        FindAndSetTexture(textures, material.separate_images,
          texture_name, texture_id);
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::GetCombinedReflectedShaderData(ShaderPipeline& shader_pipeline,
      foundation::AssetID& vertex_shader,
      foundation::AssetID& geometry_shader,
      foundation::AssetID& pixel_shader,
      foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::ShaderResource>& textures,
      foundation::Vector<foundation::ShaderResource>& samplers) const
    {
      // Load the shaders from the package
      foundation::ShaderData vertex_shader_asset = {};
      if (shader_pipeline.LoadAssetFromPackage(vertex_shader, vertex_shader_asset) == false)
      {
        PS_LOG_BUILDER(Warning,
          "Vertex shader with id(%llu) couldn't be loaded from package. Using default shaders instead.", 
          vertex_shader);

        vertex_shader = foundation::GenerateId("ps_default_vertex_shader");
        geometry_shader = 0;
        pixel_shader = foundation::GenerateId("ps_default_pixel_shader");

        vertex_shader_asset = {};
        if (shader_pipeline.LoadAssetFromPackage(vertex_shader, vertex_shader_asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Default vertex shader is not in the cache.");

          return false;
        }
      }

      foundation::ShaderData geometry_shader_asset = {};
      if (geometry_shader != 0)
      {
        if (shader_pipeline.LoadAssetFromPackage(geometry_shader, geometry_shader_asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Geometry shader with id(%llu) couldn't be loaded from package.", geometry_shader);
          return false;
        }
      }

      foundation::ShaderData pixel_shader_asset = {};
      if (shader_pipeline.LoadAssetFromPackage(pixel_shader, pixel_shader_asset) == false)
      {
        PS_LOG_BUILDER(Warning,
          "Pixel shader with id(%llu) couldn't be loaded from package. Using default shaders instead.", 
          pixel_shader);

        vertex_shader = foundation::GenerateId("ps_default_vertex_shader");
        geometry_shader = 0;
        pixel_shader = foundation::GenerateId("ps_default_pixel_shader");

        vertex_shader_asset = {};
        if (shader_pipeline.LoadAssetFromPackage(vertex_shader, vertex_shader_asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Default vertex shader is not in the cache.");

          return false;
        }

        pixel_shader_asset = {};
        if(shader_pipeline.LoadAssetFromPackage(pixel_shader, pixel_shader_asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Default pixel shader is not in the cache.");

          return false;
        }
      }

      // Check the shader stages
      if (vertex_shader_asset.stage != foundation::ShaderData::ShaderStage::kVertex)
      {
        PS_LOG_BUILDER(Error,
          "Vertex shader(%lli) is not a vertex shader.", vertex_shader);
        return false;
      }

      if (geometry_shader != 0)
      {
        if (geometry_shader_asset.stage != foundation::ShaderData::ShaderStage::kGeometry)
        {
          PS_LOG_BUILDER(Error,
            "Geometry shader(%lli) is not a geometry shader.", geometry_shader);
          return false;
        }
      }

      if (pixel_shader_asset.stage != foundation::ShaderData::ShaderStage::kPixel)
      {
        PS_LOG_BUILDER(Error,
          "Pixel shader(%lli) is not a pixel shader.", pixel_shader);
        return false;
      }

      // Create list of unique uniform buffers
      GetShaderResourceCount get_shader_resource_count = [](const foundation::ShaderData& shader)
      {
        return shader.uniform_buffers.size();
      };
      GetShaderResource get_shader_resource = [](const foundation::ShaderData& shader, int i)
      {
        return shader.uniform_buffers[i];
      };

      if(ShaderResourceLoop(vertex_shader_asset, 
        (geometry_shader != 0) ? &geometry_shader_asset : nullptr, 
        pixel_shader_asset, 
        get_shader_resource_count, 
        get_shader_resource, 
        uniform_buffers) == false)
      {
        return false;
      }
      
      // Create list of unique texture units
      get_shader_resource_count = [](const foundation::ShaderData& shader)
      {
        return shader.separate_images.size();
      };
      get_shader_resource = [](const foundation::ShaderData& shader, int i)
      {
        return shader.separate_images[i];
      };

      if(ShaderResourceLoop(vertex_shader_asset,
        (geometry_shader != 0) ? &geometry_shader_asset : nullptr,
        pixel_shader_asset,
        get_shader_resource_count,
        get_shader_resource,
        textures) == false)
      {
        return false;
      }

      // Create list of unique sampler units
      get_shader_resource_count = [](const foundation::ShaderData& shader)
      {
        return shader.separate_samplers.size();
      };
      get_shader_resource = [](const foundation::ShaderData& shader, int i)
      {
        return shader.separate_samplers[i];
      };

      if(ShaderResourceLoop(vertex_shader_asset,
        (geometry_shader != 0) ? &geometry_shader_asset : nullptr,
        pixel_shader_asset,
        get_shader_resource_count,
        get_shader_resource,
        samplers) == false)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MaterialPipeline::ShaderResourceLoop(const foundation::ShaderData& vertex_shader_asset, 
      const foundation::ShaderData* geometry_shader_asset, 
      const foundation::ShaderData& pixel_shader_asset, 
      GetShaderResourceCount get_shader_resource_count, 
      GetShaderResource get_shader_resource, 
      foundation::Vector<foundation::ShaderResource>& unique_resource_list) const
    {
      size_t vertex_shader_resource_count = get_shader_resource_count(vertex_shader_asset);
      unique_resource_list.resize(vertex_shader_resource_count);
      for(int i = 0; i < vertex_shader_resource_count; ++i)
      {
        unique_resource_list[i] = get_shader_resource(vertex_shader_asset, i);
      }

      if (geometry_shader_asset != nullptr)
      {
        for (int i = 0; i < get_shader_resource_count(*geometry_shader_asset); ++i)
        {
          const foundation::ShaderResource geom_resource = 
            get_shader_resource(*geometry_shader_asset, i);
          bool is_unique = true;

          for (const foundation::ShaderResource& unique_resource : unique_resource_list)
          {
            if (geom_resource.LinkerCheck(unique_resource) == false)
            {
              PS_LOG_BUILDER(Error,
                "Geometry shader has resource that is incompatible with the resources defined in the other shaders.");
              return false;
            }

            if (geom_resource.binding == unique_resource.binding &&
              geom_resource.desc_set == unique_resource.desc_set)
            {
              is_unique = false;
              break;
            }
          }

          if (is_unique == true)
          {
            unique_resource_list.push_back(geom_resource);
          }
        }
      }

      for (int i = 0; i < get_shader_resource_count(pixel_shader_asset); ++i)
      {
        const foundation::ShaderResource pixel_resource =
          get_shader_resource(pixel_shader_asset, i);
        bool is_unique = true;

        for (const foundation::ShaderResource& unique_resource : unique_resource_list)
        {
          if (pixel_resource.LinkerCheck(unique_resource) == false)
          {
            PS_LOG_BUILDER(Error,
              "Pixel shader has resource that is incompatible with the resources defined in the other shaders.");
            return false;
          }

          if (pixel_resource.binding == unique_resource.binding)
          {
            is_unique = false;
            break;
          }
        }

        if (is_unique == true)
        {
          unique_resource_list.push_back(pixel_resource);
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::Vector<foundation::ShaderResource> MaterialPipeline::FindShaderResources(
      const foundation::Vector<foundation::ShaderResource>& shader_resources,
      const foundation::String& name) const
    {
      foundation::Vector<foundation::ShaderResource> result = {};

      for(const foundation::ShaderResource& resource : shader_resources)
      {
        if(resource.name == name)
        {
          result.push_back(resource);
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------------
    void MaterialPipeline::FindAndSetUniform(
      const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::String& uniform_name, float v) const
    {
      FindAndSetUniformInternal(uniform_buffers, uniform_buffer_data, uniform_name, v,
        foundation::ShaderResource::ConcreteType::kFloat, "float");
    }

    //--------------------------------------------------------------------------------
    void MaterialPipeline::FindAndSetUniform(
      const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::String& uniform_name, const glm::vec2& v) const
    {
      FindAndSetUniformInternal(uniform_buffers, uniform_buffer_data, uniform_name, v,
        foundation::ShaderResource::ConcreteType::kVec2, "float2");
    }

    //--------------------------------------------------------------------------------
    void MaterialPipeline::FindAndSetUniform(
      const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::String& uniform_name, const glm::vec3& v) const
    {
      FindAndSetUniformInternal(uniform_buffers, uniform_buffer_data, uniform_name, v,
        foundation::ShaderResource::ConcreteType::kVec3, "float3");
    }

    //--------------------------------------------------------------------------------
    void MaterialPipeline::FindAndSetUniform(
      const foundation::Vector<foundation::ShaderResource>& uniform_buffers,
      foundation::Vector<foundation::UniformBufferData>& uniform_buffer_data,
      const foundation::String& uniform_name, const glm::vec4& v) const
    {
      FindAndSetUniformInternal(uniform_buffers, uniform_buffer_data, uniform_name, v, 
        foundation::ShaderResource::ConcreteType::kVec4, "float4");
    }

    //--------------------------------------------------------------------------------
    void MaterialPipeline::FindAndSetTexture(
      const foundation::Vector<foundation::ShaderResource>& textures,
      foundation::Vector<foundation::AssetID>& texture_data,
      const foundation::String& texture_name, foundation::AssetID texture_id) const
    {
      for(int i = 0; i < textures.size(); ++i)
      {
        if(textures[i].name == texture_name && 
          textures[i].image.dimension == foundation::ShaderResource::Image::Dimensions::k2D)
        {
          texture_data[i] = texture_id;
        }
      }
    }
  }
}
