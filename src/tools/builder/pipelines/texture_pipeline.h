#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/pipeline-assets/texture.h>

namespace nv // NVIDIA namespace
{
  class DirectDrawSurface;
}

namespace sulphur 
{
  namespace builder 
  {
    /**
     * @class sulphur::builder::TexturePipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging, processing and management 
     * of textures.
     * @author Timo van Hees 
     */
    class TexturePipeline : public PipelineBase
    {
    public:
      /**
       * @brief Creates a texture from the pixel data in an image.
       * @param[in] image_file (const sulphur::foundation::Path&) The image file 
       * containing the pixel data. 
       * @param[out] texture (sulphur::builder::TextureAsset&) The texture created 
       * from the image.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the texture should be discarded.
       */
      bool Create(const foundation::Path& image_file, foundation::TextureAsset& texture) const;
      /**
       * @brief Adds a texture to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was 
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] texture (sulphur::builder::TextureAsset&) The texture to add to the package.
       * @return (bool) True if the texture was added to the package succesfully.
       */
      bool PackageTexture(const foundation::Path& asset_origin, foundation::TextureAsset& texture);
      /**
       * @see sulphur::builder::PipelineBase::GetCacheName
       */
      foundation::String GetCacheName() const override;
      /**
      * @see sulphur::builder::PipelineBase::GetPackageExtension
      */
      foundation::String GetPackageExtension() const override;

      /**
      * @see sulphur::builder::PipelineBase::PackageDefaultAssets
      */
      bool PackageDefaultAssets() override;

    private:
      /**
       * @brief Loads an image and creates a texture with the pixel data.
       * @param[in] image_file (const sulphur::foundation::Path&) The image file 
       * containing the pixel data. 
       * @param[out] texture (sulphur::builder::TextureAsset&) The texture created 
       * from the image.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the texture should be discarded.
       */
      bool LoadImage(const foundation::Path& image_file, foundation::TextureAsset& texture) const;
      /**
       * @brief Loads an image using STB_Image and creates a texture with the pixel data.
       * This is used to load png, bmp, jpg and tga image files.
       * @param[in] image_file (const sulphur::foundation::Path&) The image file 
       * containing the pixel data. 
       * @param[out] texture (sulphur::builder::TextureAsset&) The texture created 
       * from the image.
       * @param image_data (const unsigned char*) The data in the image file.
       * @param size (const int size) The size of the image data.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the texture should be discarded.
       */
      bool LoadImageSTBI(const foundation::Path& image_file, foundation::TextureAsset& texture,
        const unsigned char* image_data, const int size) const;
      /**
      * @brief Loads an image using Nvidia Texture Tools and creates a texture with 
      * the pixel data. This is used to load dds image files.
      * @param[in] image_file (const sulphur::foundation::Path&) The image file
      * containing the pixel data.
      * @param[out] texture (sulphur::builder::TextureAsset&) The texture created
      * from the image.
      * @param image_data (const unsigned char*) The data in the image file.
      * @param size (const int size) The size of the image data.
      * @return (bool) False when there was an error that couldn't be recovered from.
      * @remark If the function returned false, the texture should be discarded.
      */
      bool LoadImageNVTT(const foundation::Path& /*image_file*/, foundation::TextureAsset& /*texture*/,
        const unsigned char* /*image_data*/, const int /*size*/) const;

      /**
       * @brief Loads a single mip of a single face or slice from a DDS image.
       * @param[in] dds (nv::DirectDrawSurface&) DDS image.
       * @param[in] face (int) The face of a cubemap (0, 6] or the slice of a 3D texture (0, depth]
       * @param[in] mip (int) The mip of the face to load.
       * @param[out] pixel_data (sulphur::foundation::Vector <byte>&) RGBA pixel data output. 
       * @return (bool) False when there was an error that couldn't be recovered from.
       * @remark If the function returned false, the texture should be discarded.
       */
      bool LoadSurface(nv::DirectDrawSurface& dds, int face, int mip, 
        foundation::Vector<byte>& pixel_data) const;
    };
  }
}
