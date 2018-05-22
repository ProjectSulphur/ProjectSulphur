#pragma once
#include <foundation/pipeline-assets/texture.h>
#include <foundation/containers/vector.h>
#include <nvtt/nvtt.h>

namespace sulphur 
{
  namespace builder 
  {
    struct TextureProcessor : public nvtt::OutputHandler, public nvtt::ErrorHandler
    {
    public:
      TextureProcessor(foundation::TextureData* target, size_t total_size)
        : target_(target)
      {
        target->pixel_data.resize(total_size);
        curr_texel_ = target->pixel_data.begin();
      }

      void beginImage(int size, int width, int height, int depth, int face,
        int mip_level) override;
      bool writeData(const void * data, int size) override;
      void endImage() override;
      void error(nvtt::Error error);

    private:
      foundation::TextureData* target_;
      foundation::Vector<byte>::iterator curr_texel_;
    };
  }
}
