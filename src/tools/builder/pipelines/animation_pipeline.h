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
    struct AnimationAsset;
    class AnimationData;
  }

  namespace builder 
  {
    class SceneLoader;

    /**
     * @class sulphur::builder::AnimationPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of animations.
     * @author Timo van Hees
     */
    class AnimationPipeline : public PipelineBase
    {
    public:
      AnimationPipeline() = default;
      ~AnimationPipeline() = default;

      /**
      * @brief Creates a animations from all animations found in the scene.
      * @param[in] file (const sulphur::foundation::Path&) The file containing the animations.
      * @param[in] scene_loader (sulpher::builder::SceneLoader&)
      * The scene loader to use to load the scenes.
      * @param[out] animations (sulphur::foundation::Vector <sulphur::foundation::AnimationAsset>&)
      * The animations created from the file.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the animations should be discarded.
      */
      bool Create(const foundation::Path& file, SceneLoader& scene_loader, 
        foundation::Vector<foundation::AnimationAsset>& animations);

      /**
       * @brief Adds an animation to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] animation (sulphur::foundation::AnimationAsset&) The animation to add to the package.
       * @return (bool) True if the animation was packaged succesfully.
       */
      bool PackageAnimation(const foundation::Path& asset_origin,
        foundation::AnimationAsset& animation);

      /**
       * @see sulphur::builder::PipelineBase::GetPackageExtension
       */
      foundation::String GetPackageExtension() const override;

      /**
       * @see sulphur::builder::PipelineBase::GetCacheName
       */
      foundation::String GetCacheName() const override; 
    };
  }
}
