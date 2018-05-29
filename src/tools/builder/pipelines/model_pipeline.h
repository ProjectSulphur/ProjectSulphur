#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/pipeline-assets/model.h>
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>
#include <foundation/io/filesystem.h>
#include <assimp/Importer.hpp>

struct aiNode;

namespace sulphur 
{
  namespace foundation 
  {
    class ModelInfo;    
  }

  namespace builder 
  {
    class SceneLoader;
    class TexturePipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ShaderPipeline;
    class SkeletonPipeline;

    /**
     * @class sulphur::builder::ModelPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of models.
     * @author Timo van Hees
     */
    class ModelPipeline : public PipelineBase
    {
    public:
      /**
       * @brief Get information about all models contained in the scene.
       * @param[in] scene_loader (sulphur::builder::SceneLoader&) 
       * The scene loader to use to load the scene.
       * @param[in] file (const sulphur::foundation::Path&) The file containing the scene.
       * @param[in] single_model (bool) Forces the scene to be interpreted as a single model.
       * @return (sulphur::foundation::ModelInfo) Struct containing information about 
       * all assets in the scene.
       */
      foundation::ModelInfo GetModelInfo(SceneLoader& scene_loader, 
        const foundation::Path& file, bool single_model);
      /**
       * @brief Creates meshes from the material information present in the scene.
       * @param[in] scene_loader (sulphur::builder::SceneLoader&) 
       * The scene loader to use to load the scene.
       * @param[in] file (const sulphur::foundation::Path&) The file containing the scene.
       * @param[in] single_model (bool) Forces the scene to be interpreted as a single model.
       * @param[in] model_info (const sulphur::foundation::ModelInfo&) Struct containing information 
       * about all assets in the scene and information about which assets need to be loaded.
       * @param[in] mesh_pipeline (const sulphur::builder::MeshPipeline&) 
       * The mesh pipeline to use when importing meshes used by the models.
       * @param[in] skeleton_pipeline (const sulphur::builder::SkeletonPipeline&) 
       * The skeleton pipeline to use when importing skeletons used by the meshes.
       * @param[in] material_pipeline (const sulphur::builder::MaterialPipeline&) 
       * The material pipeline to use when importing materials used by the models.
       * @param[in] texture_pipeline (const sulphur::builder::TexturePipeline&) 
       * The texture pipeline to use when importing textures used by the materials 
       * used by the models.
       * @param[in] shader_pipeline (const sulphur::builder::ShaderPipeline&)
       * The shader pipeline used to package the shaders.
       * @param[in] vertex_shader (const sulphur::foundation::AssetName&) 
       * The vertex shader to use when creating materials.
       * @param[in] pixel_shader (const sulphur::foundation::AssetName&) 
       * The pixel shader to use when creating materials.
       * @param[out] models (sulphur::foundation::Vector<sulphur::foundation::ModelAsset>&)
       * List of models created from the objects in the scene.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the models should be discarded.
       */
      bool Create(SceneLoader& scene_loader, const foundation::Path& file, bool single_model,
                  const foundation::ModelInfo& model_info,
                  const MeshPipeline& mesh_pipeline, const SkeletonPipeline& skeleton_pipeline,
                  const MaterialPipeline& material_pipeline,
                  TexturePipeline& texture_pipeline, ShaderPipeline& shader_pipeline,
                  const foundation::AssetName& vertex_shader,
                  const foundation::AssetName& pixel_shader,
                  foundation::Vector<foundation::ModelAsset>& models);
      /**
       * @brief Adds a mesh to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] model (sulphur::foundation::ModelAsset&) The model to add to the package.
       * @param[in] mesh_pipeline (sulphur::builder::MeshPipeline&) 
       * The mesh pipeline to use when packaging meshes used by the models.
       * @param[in] skeleton_pipeline (sulphur::builder::SkeletonPipeline&) 
       * The skeleton pipeline to use when packaging skeletons used by the models.
       * @param[in] material_pipeline (sulphur::builder::MaterialPipeline&) 
       * The material pipeline to use when packaging materials used by the models.
       * @param[in] texture_pipeline (sulphur::builder::TexturePipeline&) 
       * The texture pipeline to use when packaging textures used by the models.
       * @return True if the model was added to the package succesfully.
       */
      bool PackageModel(const foundation::Path& asset_origin, foundation::ModelAsset& model,
        MeshPipeline& mesh_pipeline, SkeletonPipeline& skeleton_pipeline, 
        MaterialPipeline& material_pipeline, TexturePipeline& texture_pipeline);

      /**
       * @see sulphur::builder::PipelineBase::GetPackageExtension
       */
      foundation::String GetPackageExtension() const override;

      /**
       * @see sulphur::builder::PipelineBase::GetCacheName
       */
      foundation::String GetCacheName() const override;

    private:
      /**
       * @brief Checks of the node or any of its child nodes contain meshes.
       * @param[in] node (const aiNode*) The node to check.
       * @return (bool) True if the node or any of its child nodes contain meshes.
       */
      static bool NodeHasMeshes(const aiNode* node);

      /**
       * @brief Adds materials to the model.
       * @param[in] node (const aiNode*) The base node of the model.
       * @param[in] scene (const aiScene*) The scene containing the node.
       * @param[in] materials (const sulphur::foundation::Vector<sulphur::foundation::MaterialAsset>&)
       * The materials imported from the scene.
       * @param[in|out] model (sulphur::foundation::ModelData&) The model to add the materials to.
       * @return (bool) True materials were added successfully.
       */
      static bool AddMaterialsToModel(const aiNode* node, const aiScene* scene, 
        const foundation::Vector<foundation::MaterialAsset>& materials, foundation::ModelData& model);
    };
  }
}
