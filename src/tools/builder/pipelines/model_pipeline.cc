#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/shared/util.h"
#include <foundation/pipeline-assets/model_info.h>
#include <foundation/io/binary_writer.h>
#include <foundation/logging/logger.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    foundation::ModelInfo ModelPipeline::GetModelInfo(const foundation::String& file, 
      bool single_model)
    {
      if (LoadScene(file) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "Couldn't load the scene. Model info should be discarded. file: %s", file.c_str());
        return foundation::ModelInfo();
      }

      const aiScene* scene = GetScene();

      foundation::Vector<foundation::AssetName> model_names;
      if(scene->mRootNode->mNumMeshes > 0 || single_model == true)
      {
        foundation::String name = file;
        eastl::replace(name.begin(), name.end(), '\\', '/');
        const size_t directory_end = name.find_last_of('/') + 1;
        const size_t extension_start = name.find_last_of('.');
        name = name.substr(directory_end, extension_start - directory_end);

        model_names.push_back(foundation::AssetName(name));
      }
      else
      {
        for (unsigned int i = 0u; i < scene->mRootNode->mNumChildren; ++i)
        {
          const aiNode* node = scene->mRootNode->mChildren[i];
          if (NodeHasMeshes(node) == true)
          {
            foundation::String name = node->mName.C_Str();
            const size_t postfix_pos = name.find("_$Assimp");
            name = name.substr(0, postfix_pos);
            model_names.push_back(foundation::AssetName(name));
          }
        }
      }

      foundation::ModelInfo model_info(file, static_cast<unsigned int>(model_names.size()));
      for (unsigned int i = 0u; i < model_info.number_of_models(); ++i)
      {
        model_info.SetAssetName(i, model_names[i].GetCString());
        model_info.SetModelsToLoad(i, true);
      }

      return model_info;
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::Create(const foundation::String& file,
      bool single_model,
      const foundation::ModelInfo& model_info,
      const MeshPipeline& mesh_pipeline,
      const MaterialPipeline& material_pipeline,
      TexturePipeline& texture_pipeline,
      ShaderPipeline& shader_pipeline,
      const foundation::AssetName& vertex_shader,
      const foundation::AssetName& pixel_shader,
      foundation::Vector<foundation::ModelAsset>& models)
    {
      if (file != model_info.GetFile())
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "model_info is not related to this scene.");
        return false;
      }

      if (LoadScene(file) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Couldn't load the scene. No models created. file: %s", file.c_str());
        return false;
      }

      if (single_model == true && model_info.number_of_models() > 1)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "model_info not created with single_model flag enabled.");
        return false;
      }

      int models_enabled = 0;
      for(unsigned int i = 0; i < model_info.number_of_models(); ++i)
      {
        if(model_info.GetModelsToLoad(i) == true)
        {
          ++models_enabled;
        }
      }

      if(models_enabled == 0)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "All models in the scene have been disabled.");
        return false;
      }

      foundation::Vector<foundation::MeshAsset> meshes;
      if (mesh_pipeline.Create(GetScene(), single_model, meshes) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to create meshes from the scene. No models created.");
        return false;
      }

      foundation::String directory = file;
      eastl::replace(directory.begin(), directory.end(), '\\', '/');
      size_t directory_end = directory.find_last_of('/');
      directory = directory.substr(0, directory_end + 1);

      foundation::ModelTextureCache texture_cache;
      if(material_pipeline.CreateTextureCache(GetScene(), directory, texture_pipeline, 
        texture_cache) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to create texture cache from the scene. No models created.");
        return false;
      }

      foundation::Vector<foundation::MaterialAsset> materials;
      if (material_pipeline.Create(GetScene(), directory, model_file_type_, shader_pipeline, 
        texture_cache, vertex_shader, pixel_shader, materials) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to create materials from the scene. No models created.");
        return false;
      }

      const aiScene* scene = GetScene();

      if (scene->mRootNode->mNumMeshes > 0 || single_model == true)
      {
        const aiNode* node = scene->mRootNode;
        if (NodeHasMeshes(node) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error,
            "File doesn't contain any models.");

          return false;
        }

        if (model_info.GetModelsToLoad(0) == true)
        {
          foundation::ModelAsset model = {};
          model.name = model_info.GetAssetName(0);
          model.data.mesh = meshes[0];
          model.texture_cache = eastl::move(texture_cache);

          if (AddMaterialsToModel(node, scene, materials, model.data) == true)
          {
            models.push_back(eastl::move(model));
          }
        }
      }
      else
      {
        int empty_child_count = 0;
        for (unsigned int i = 0u; i < scene->mRootNode->mNumChildren; ++i)
        {
          const aiNode* node = scene->mRootNode->mChildren[i];
          if (NodeHasMeshes(node) == false)
          {
            ++empty_child_count;
            continue;
          }

          const int index = i - empty_child_count;

          if (model_info.GetModelsToLoad(index) == true)
          {
            foundation::ModelAsset model = {};
            model.name = model_info.GetAssetName(index);
            model.data.mesh = meshes[index];
            model.texture_cache = eastl::move(texture_cache);

            if (AddMaterialsToModel(node, scene, materials, model.data) == true)
            {
              models.push_back(eastl::move(model));
            }
          }
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::PackageModel(const foundation::String& asset_origin, 
      foundation::ModelAsset& model, MeshPipeline& mesh_pipeline,
      MaterialPipeline& material_pipeline, TexturePipeline& texture_pipeline)
    {
      if (model.name.get_length() == 0)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "Model name not initialized. The model will not be packaged.");
        return false;
      }

      foundation::String output_file = "";
      if(RegisterAsset(asset_origin, model.name, output_file, model.id) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "Failed to register model. It will not be packaged.");
        return false;
      }

      foundation::BinaryWriter writer(output_file);

      if(mesh_pipeline.PackageMesh(asset_origin, model.data.mesh) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "Failed to package the mesh used with this model. The model will not be packaged.");
        return false;
      }

      writer.Write(model.data.mesh.id);

      writer.Write(model.data.materials.size());

      if(material_pipeline.PackageTextureCache(model.texture_cache, texture_pipeline, 
        model.data.materials) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to package the texture cache used by this model. The model will not be packaged.");
        return false;
      }

      for(int i = 0; i < model.data.materials.size(); ++i)
      {
        if(material_pipeline.PackageMaterial(asset_origin, model.data.materials[i]) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
            "Failed to package a material used by this model. The model will not be packaged.");
          return false;
        }

        writer.Write(model.data.materials[i].id);
      }

      if (writer.Save() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to package model.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    const aiScene* ModelPipeline::GetScene() const
    {
      return importer_.GetScene();
    }

    //--------------------------------------------------------------------------------
    foundation::String ModelPipeline::GetPackageExtension() const
    {
      return "smo";
    }

    //--------------------------------------------------------------------------------
    foundation::String ModelPipeline::GetCacheName() const
    {
      return "model_package";
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::NodeHasMeshes(const aiNode* node)
    {
      if (node->mNumMeshes > 0)
      {
        return true;
      }

      for (unsigned int i = 0u; i < node->mNumChildren; ++i)
      {
        if (NodeHasMeshes(node->mChildren[i]) == true)
        {
          return true;
        }
      }

      return false;
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::LoadScene(const foundation::String& file)
    {
      if (file == last_file_loaded_)
      {
        return importer_.GetScene() != nullptr;
      }

      const aiScene* ai_scene = importer_.ReadFile(file.c_str(),
                                                  aiProcess_GenNormals |
                                                  aiProcess_CalcTangentSpace |
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_LimitBoneWeights |
                                                  aiProcess_RemoveRedundantMaterials |
                                                  aiProcess_Triangulate |
                                                  aiProcess_SortByPType |
                                                  aiProcess_ImproveCacheLocality);

      if (ai_scene == nullptr)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Assimp: %s", importer_.GetErrorString());
        return false;
      }

      last_file_loaded_ = file;

      size_t extension_start = file.find_last_of('.');
      foundation::String extension = file.substr(extension_start);
      char8_t(*to_lower_case)(char in) = eastl::CharToLower;
      eastl::transform(extension.begin(), extension.end(), extension.begin(), to_lower_case);
      if(extension == ".fbx")
      {
        model_file_type_ = ModelFileType::kFBX;
      }
      else if(extension == ".obj")
      {
        model_file_type_ = ModelFileType::kOBJ;
      }
      else if(extension == ".gltf")
      {
        model_file_type_ = ModelFileType::kglTF;
      }
      else
      {
        model_file_type_ = ModelFileType::kUnknown;
      }
      
      return true;
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::AddMaterialsToModel(const aiNode* node,
                                            const aiScene* scene,
                                            const foundation::Vector<foundation::MaterialAsset>&
                                            materials, foundation::ModelData& model)
    {
      for (unsigned int i = 0u; i < node->mNumMeshes; ++i)
      {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        const unsigned int material_index = ai_mesh->mMaterialIndex;
        if (material_index >= materials.size())
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Assert,
            "Material index is out of range.");
          return false;
        }

        model.materials.push_back(materials[material_index]);
      }

      for (unsigned int i = 0u; i < node->mNumChildren; ++i)
      {
        if (AddMaterialsToModel(node->mChildren[i], scene, materials, model) == false)
        {
          return false;
        }
      }

      return true;
    }
  }
}
