#pragma once

#include "engine/assets/base_asset_manager.h"
#include <foundation/pipeline-assets/audio.h>

namespace sulphur 
{
  namespace engine 
  {
    using AudioBankData = foundation::AudioBankData;

    /**
    * @class sulphur::engine::AudioManager : sulphur::engine::BaseAssetManager <sulphur::engine::AudioBankData>
    * @brief Loads and manages audio banks loaded from packages.
    * @author Timo van Hees
    */
    class AudioManager : public BaseAssetManager<AudioBankData>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      AudioBankData* ImportAsset(const foundation::Path& asset_file) override;

      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String AudioManager::GetCacheName() const
    {
      return "audio_package";
    }
  }
}
