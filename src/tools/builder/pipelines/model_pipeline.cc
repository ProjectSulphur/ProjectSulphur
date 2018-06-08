#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/shared/util.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/pipeline-assets/model_info.h>
#include <foundation/pipeline-assets/skeleton.h>
#include <foundation/io/binary_writer.h>
#include <foundation/logging/logger.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    foundation::ModelInfo ModelPipeline::GetModelInfo(SceneLoader& scene_loader, 
      const foundation::Path& file,
      bool single_model)
    {
      if (ValidatePath(file) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", file.path().c_str(),
          project_dir().path().c_str());
        return foundation::ModelInfo();
      }

      foundation::Path file_path = file.is_relative_path() ? project_dir() + file : file;

      const aiScene* scene = scene_loader.LoadScene(file_path);
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error, 
          "Couldn't load the scene. Model info should be discarded. file: %s", 
          file_path.GetString().c_str());
        return foundation::ModelInfo();
      }

      foundation::Vector<foundation::AssetName> model_names;
      if(scene->mRootNode->mNumMeshes > 0 || single_model == true)
      {
        model_names.push_back(foundation::AssetName(file_path.GetFileName()));
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

      foundation::ModelInfo model_info(file_path, static_cast<unsigned int>(model_names.size()));
      for (unsigned int i = 0u; i < model_info.number_of_models(); ++i)
      {
        model_info.SetAssetName(i, model_names[i].GetCString());
        model_info.SetModelsToLoad(i, true);
      }

      return model_info;
    }

    //--------------------------------------------------------------------------------
    bool ModelPipeline::Create(SceneLoader& scene_loader, 
      const foundation::Path& file,
      bool single_model,
      const foundation::ModelInfo& model_info,
      const MeshPipeline& mesh_pipeline,
      const SkeletonPipeline& skeleton_pipeline,
      const MaterialPipeline& material_pipeline,
      TexturePipeline& texture_pipeline,
      ShaderPipeline& shader_pipeline,
      const foundation::AssetName& vertex_shader,
      const foundation::AssetName& pixel_shader,
      foundation::Vector<foundation::ModelAsset>& models)
    {
      if (ValidatePath(file) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", file.path().c_str(), 
          project_dir().path().c_str());
        return false;
      }

      foundation::Path file_path = file.is_relative_path() ? project_dir() + file : file;

      if (file_path != model_info.GetFile())
      {
        PS_LOG_BUILDER(Error,
          "model_info is not related to this scene.");
        return false;
      }

      const aiScene* scene = scene_loader.LoadScene(file_path);
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "Couldn't load the scene. No models created. file: %s", file_path.GetString().c_str());
        return false;
      }

      if (single_model == true && model_info.number_of_models() > 1)
      {
        PS_LOG_BUILDER(Error,
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
        PS_LOG_BUILDER(Warning,
          "All models in the scene have been disabled.");
        return false;
      }

      foundation::Vector<foundation::MeshAsset> meshes;
      foundation::Vector<foundation::SkeletonAsset> skeletons;
      if (mesh_pipeline.Create(scene, single_model, skeleton_pipeline, meshes, skeletons) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to create meshes from the scene. No models created.");
        return false;
      }

      const foundation::Path directory = file_path.GetFolderPath();

      foundation::ModelTextureCache texture_cache;
      if(material_pipeline.CreateTextureCache(scene, directory, texture_pipeline, 
        texture_cache) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to create texture cache from the scene. No models created.");
        return false;
      }

      foundation::Vector<foundation::MaterialAsset> materials;
      if (material_pipeline.Create(scene, directory, scene_loader.GetModelFileType(), 
        shader_pipeline, texture_cache, vertex_shader, pixel_shader, materials) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to create materials from the scene. No models created.");
        return false;
      }

      if (scene->mRootNode->mNumMeshes > 0 || single_model == true)
      {
        const aiNode* node = scene->mRootNode;
        if (NodeHasMeshes(node) == false)
        {
          PS_LOG_BUILDER(Error,
            "File doesn't contain any models.");

          return false;
        }

        if (model_info.GetModelsToLoad(0) == true)
        {
          foundation::ModelAsset model = {};
          model.name = model_info.GetAssetName(0);
          model.data.mesh = meshes[0];
          model.data.skeletons = skeletons;
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
            model.data.skeletons = skeletons;
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
    bool ModelPipeline::PackageModel(const foundation::Path& asset_origin, 
      foundation::ModelAsset& model, MeshPipeline& mesh_pipeline, 
      SkeletonPipeline& skeleton_pipeline, MaterialPipeline& material_pipeline, 
      TexturePipeline& texture_pipeline)
    {
      if (ValidatePath(asset_origin) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed.");
        return false;
      }

      foundation::Path origin = CreateProjectRelativePath(asset_origin);

      if (model.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error, 
          "Model name not initialized. The model will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if(RegisterAsset(origin, model.name, output_file, model.id) == false)
      {
        PS_LOG_BUILDER(Error, 
          "Failed to register model. It will not be packaged.");
        return false;
      }

      foundation::BinaryWriter writer(output_file);

      if(mesh_pipeline.PackageMesh(origin, model.data.mesh) == false)
      {
        PS_LOG_BUILDER(Error, 
          "Failed to package the mesh used with this model. The model will not be packaged.");
        return false;
      }

      for (int i = 0; i < model.data.skeletons.size(); ++i)
      {
        if (skeleton_pipeline.PackageSkeleton(origin, model.data.skeletons[i]) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to package a skeleton used by this model. The model will not be packaged.");
          return false;
        }
      }

      if (material_pipeline.PackageTextureCache(model.texture_cache, texture_pipeline,
        model.data.materials) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to package the texture cache used by this model. The model will not be packaged.");
        return false;
      }

      for (int i = 0; i < model.data.materials.size(); ++i)
      {
        if (material_pipeline.PackageMaterial(origin, model.data.materials[i]) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to package a material used by this model. The model will not be packaged.");
          return false;
        }
      }

      writer.Write(model.data);

      if (writer.Save() == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to package model.");
        return false;
      }

      return true;
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
          PS_LOG_BUILDER(Assert,
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
