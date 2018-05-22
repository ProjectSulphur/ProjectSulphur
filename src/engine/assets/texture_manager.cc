#include "texture_manager.h"
#include "texture.h"

#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/texture.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Texture* TextureManager::ImportAsset(const foundation::String& asset_file)
    {
      foundation::BinaryReader reader(asset_file);
      if (reader.is_ok() == true)
      {
        foundation::TextureData texture_data = reader.Read<foundation::TextureData>();

        Texture* texture = foundation::Memory::Construct<Texture>(texture_data.pixel_data, 
          texture_data.width, texture_data.height);
        return texture;
      }

      return nullptr;
    }
  }
}
