#pragma once

#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/pipeline-assets/mesh.h>

struct aiNode;
struct aiScene;

namespace sulphur 
{
  namespace foundation
  {
    struct SkeletonAsset;
  }

  namespace builder 
  {
    class SkeletonPipeline;

    /**
     * @class sulphur::builder::MeshPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of meshes.
     * @author Timo van Hees
     */
    class MeshPipeline : public PipelineBase
    {
    public:
      MeshPipeline() = default;
      ~MeshPipeline() = default;

      /**
       * @brief Creates meshes from the mesh information present in the scene.
       * @param[in] scene (const aiScene*) The scene containing the material information.
       * @param[in] single_mesh (bool) Forces the scene to be interpreted as a single mesh.
       * @param[in] skeleton_pipeline (const sulphur::builder::SkeletonPipeline&) 
       * Skeleton pipeline to use to import skeletons.
       * @param[out] meshes (sulphur::foundation::Vector <sulphur::foundation::MeshAsset>&)
       * List of meshes created from the meshes in the scene.
       * @param[out] skeletons (sulphur::foundation::Vector <sulphur::foundation::SkeletonAsset>&) 
       * Skeletons used by the meshes.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the meshes should be discarded.
       */
      bool Create(const aiScene* scene, bool single_mesh,
        const SkeletonPipeline& skeleton_pipeline,
        foundation::Vector<foundation::MeshAsset>& meshes, 
        foundation::Vector<foundation::SkeletonAsset>& skeletons) const;
      /**
       * @brief Adds a mesh to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] mesh (sulphur::foundation::MeshAsset&) The mesh to add to the package.
       * @return True if the mesh was added to the package succesfully.
       */
      bool PackageMesh(const foundation::Path& asset_origin, foundation::MeshAsset& mesh);

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
       * @brief Recursivly loads all sub-meshes and adds them to the mesh.
       * @param[in] scene (const aiScene*) The scene containing the meshes and the node.
       * @param[in] node (const aiNode*) The node in the scene hierarchy to load the meshes from.
       * @param[in] parent_transform (const glm::mat4&) The transform of the parent node.
       * @param[in] skeleton_pipeline (const sulphur::builder::SkeletonPipeline&) 
       * Skeleton pipeline to use to import skeletons.
       * @param[in|out] mesh (sulphur::foundation::MeshData&) The mesh to add the sub-meshes to.
       * @param[out] skeletons (sulphur::foundation::Vector <sulphur::foundation::SkeletonAsset>&) 
       * Skeletons used by the meshes.
       * @return (bool) False if an error occured loading the sub-meshes. 
       * @mesh should be discarded.
       */
      static bool LoadSubMeshes(const aiScene* scene, const aiNode* node, 
        const glm::mat4& parent_transform, foundation::MeshData& mesh, 
        const SkeletonPipeline& skeleton_pipeline,
        foundation::Vector<foundation::SkeletonAsset>& skeletons);
      /**
       * @brief Calculates the bounding box and sphere of a sub-mesh.
       * @param[in|out] sub_mesh (suplhur::foundation::SubMesh&) The sub-mesh to calculate
       *  the bounding shapes for. Bounding shapes are stored in the sub-mesh.
       */
      static void CalculateBoundingShapes(foundation::SubMesh& sub_mesh);
      /**
      * @brief Calculates the bounding box and sphere of a mesh.
      * @param[in|out] mesh (suplhur::foundation::MeshData&) The mesh to calculate
      *  the bounding shapes for. Bounding shapes are stored in the mesh.
      */
      static void CalculateBoundingShapes(foundation::MeshData& mesh);
    
    };
  }
}
