#include "tools/builder/pipelines/audio_pipeline.h"
#include <foundation/pipeline-assets/audio.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    bool AudioPipeline::Create(const foundation::Path& file, foundation::AudioBankAsset& bank) const
    {
      if(file.GetFileExtension() != "bank")
      {
        PS_LOG_BUILDER(Error,
          "File is not an audio bank. file: %s", file.GetString().c_str());
        return false;
      }

      foundation::BinaryReader reader(file);
      if(reader.is_ok() == false)
      {
        PS_LOG_BUILDER(Error, 
          "Failed to load audio bank. The audio bank asset should be discarded. file: %s", 
          file.GetString().c_str());
        return false;
      }

      const foundation::Vector<unsigned char>& file_data = reader.data();
      if (file_data.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "The loaded audio bank is empty. The audio bank asset should be discarded. file: %s",
          file.GetString().c_str());
        return false;
      }

      bank.name = file.GetFileName();
      eastl::move(file_data.begin(), file_data.end(), eastl::back_inserter(bank.data.data));

      return true;
    }

    //--------------------------------------------------------------------------------
    bool AudioPipeline::PackageAudioBank(const foundation::Path& asset_origin,
      foundation::AudioBankAsset& bank)
    {
      if (bank.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error,
          "Audio bank name not initialized. The audio bank will not be packaged.");
        return false;
      }

      if (bank.data.data.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "Audio bank holds no data. The audio bank will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if (RegisterAsset(asset_origin, bank.name, output_file, bank.id) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to register audio bank %s. It will not be packaged.", bank.name.GetCString());

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(bank.data);

      if (writer.SaveCompressed(foundation::CompressionType::kHighCompression) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to package audio bank %s.", bank.name.GetCString());
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String AudioPipeline::GetPackageExtension() const
    {
      return "sau";
    }

    //--------------------------------------------------------------------------------
    foundation::String AudioPipeline::GetCacheName() const
    {
      return "audio_package";
    }
  }
}
