#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/io/filesystem.h>

struct aiMesh;
struct aiNode;
struct aiScene;

namespace sulphur 
{
  namespace foundation 
  {
    struct SkeletonAsset;
    class SkeletonData;
  }

  namespace builder 
  {
    class SceneLoader;

    /**
     * @class sulphur::builder::SkeletonPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of skeletons.
     * @author Riko Ophorst, Timo van Hees
     */
    class SkeletonPipeline : public PipelineBase
    {
    public:
      SkeletonPipeline() = default;
      ~SkeletonPipeline() = default;

      /**
       * @brief Creates a skeleton from the bones attached to a mesh for all meshes in the scene.
       * @param[in] file (const sulphur::foundation::Path&) The file containing the skeletons.
       * @param[in] scene_loader (sulpher::builder::SceneLoader&) 
       * The scene loader to use to load the scenes.
       * @param[out] skeletons (sulphur::foundation::Vector <sulphur::foundation::SkeletonAsset>&) 
       * The skeletons created from the file.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the skeletons should be discarded.
       */
      bool Create(const foundation::Path& file, SceneLoader& scene_loader, 
        foundation::Vector<foundation::SkeletonAsset>& skeletons) const;

      /**
      * @brief Creates a skeleton from the bones attached to a mesh for all meshes in a node.
      * @param[in] scene (const aiScene*) The scene.
      * @param[in] node (const aiNode*) The node.
      * @param[out] skeleton (sulphur::foundation::SkeletonAsset>&) 
      * The skeleton created from the node.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the skeleton should be discarded.
      */
      bool Create(const aiScene* scene, const aiNode* node, foundation::SkeletonAsset& skeleton) const;

      /**
       * @brief Adds a skeleton to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] skeleton (sulphur::foundation::SkeletonAsset&) The skeleton to add to the package.
       * @return (bool) True if the skeleton was added to the package succesfully.
       */
      bool PackageSkeleton(const foundation::Path& asset_origin, 
        foundation::SkeletonAsset& skeleton);

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
       * @brief Finds all skeletons in the scene by walking the node tree.
       * @param[in] scene (const aiScene*) The scene containing the skeletons.
       * @param[in] node (const aiNode*) The current node to evaluate.
       * @param[out] skeletons (sulphur::foundation::Vector <sulphur::foundation::SkeletonAsset>&)
       * The skeletons found in the scene.
       * @return (bool) False when there was an error that couldn't be recovered from.
       * @remark If the function returned false, the skeletons should be discarded.
       */
      bool FindSkeletonsRecursivly(const aiScene* scene, const aiNode* node,
        foundation::Vector<foundation::SkeletonAsset>& skeletons) const;

      /**
       * @brief Adds the bones used by the mesh to the skeleton.
       * @param ai_mesh (const aiMesh*) The mesh to get the bones from.
       * @param skeleton (sulphur::foundation::SkeletonData&) The skeleton to add the bones to.
       * @return (bool) False when there was an error that couldn't be recovered from.
       * @remark If the function returned false, the skeleton should be discarded.
       */
      bool GetBonesFromMesh(const aiMesh* ai_mesh,
        foundation::SkeletonData& skeleton) const;
    };
  }
}
