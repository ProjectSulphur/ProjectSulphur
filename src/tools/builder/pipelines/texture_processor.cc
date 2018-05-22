#include "tools/builder/pipelines/texture_processor.h"

#include <foundation/logging/logger.h>

#include <nvtt/nvtt.h>

namespace sulphur 
{
  namespace builder 
  {
    // ------------------------------------------------------------------------
    void TextureProcessor::beginImage(
      int /*size*/,
      int width,
      int height,
      int depth,
      int /*mip_level*/,
      int mip_level)
    {
      if (mip_level == 0)
      {
        target_->width = width;
        target_->height = height;
        target_->depth = depth;
      }
    }

    // ------------------------------------------------------------------------
    bool TextureProcessor::writeData(const void* data, int size)
    {
      const byte* source = static_cast<const byte*>(data);
      eastl::copy(source, source + size, curr_texel_);
      
      curr_texel_ += size;

      return true;
    }

    // ------------------------------------------------------------------------
    void TextureProcessor::endImage()
    {
    }

    // ------------------------------------------------------------------------
    void TextureProcessor::error(nvtt::Error error)
    {
      PS_LOG_WITH(foundation::LineAndFileLogger, Error,
        nvtt::errorString(error));
    }
  }
}