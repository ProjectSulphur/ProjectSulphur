#pragma once
#include "tools/builder/pipelines/pipeline_base.h"

namespace sulphur 
{
  namespace foundation 
  {
    struct AudioBankAsset;
  }

  namespace builder 
  {
    /**
     * @class sulphur::builder::AudioPipeline : public sulphur::builder::PipelineBase
     * @brief Pipeline that loads and packages FMOD audio banks.
     * @author Timo van Hees
     */
    class AudioPipeline : public PipelineBase
    {
    public:
      /**
       * @brief Default constructor.
       */
      AudioPipeline() = default;
      /**
       * @brief Destructor.
       */
      ~AudioPipeline() = default;

      /**
      * @brief Creates audio bank asset.
      * @param[in] file (const sulphur::foundation::Path&) The FMOD audio bank.
      * @param[out] bank (sulphur::foundation::AudioBankAsset&) The audio bank.
      * @return (bool) True if the audio bank loaded succesfully.
      * @remark When false, the audio bank should be discarded.
      */
      bool Create(const foundation::Path& file, foundation::AudioBankAsset& bank) const;

      /**
      * @brief Adds an audio bank to the package.
      * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was
      * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
      * @param[in] bank (sulphur::foundation::AudioBankAsset&) The audio bank to add to the package.
      * @return (bool) True if the audio bank was added to the package succesfully.
      */
      bool PackageAudioBank(const foundation::Path& asset_origin,
        foundation::AudioBankAsset& bank);

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
