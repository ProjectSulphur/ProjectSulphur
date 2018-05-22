#pragma once
#include "tools/builder/base/flags_base.h"

namespace sulphur 
{
  namespace builder 
  {
    /**
    *@struct sulphur::builder::DirFlag : sulphur::builder::Flag
    *@brief flag specifying to search a certain directory
    *@author Stan Pepels
    */
    struct DirFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::FileFlag : sulphur::builder::Flag
    *@brief flag spicifying that only a single file should be cnverted
    *@author Stan Pepels
    */
    struct FileFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::RecursiveFlag : sulphur::builder::Flag
    *@brief flag specifying that some recursion will occur in the command
    *@author Stan Pepels
    */
    struct RecursiveFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::SingleFlag : sulphur::builder::Flag
    *@brief flag specifying that only one asset should be created
    *@author Timo van Hees
    */
    struct SingleFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::PSSLFlag : sulphur::builder::Flag
    *@brief flag specifying shader target
    *@author Stan Pepels
    */
    struct PSSLFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::HLSLFlag : sulphur::builder::Flag
    *@brief flag specifying shader target
    *@author Stan Pepels
    */
    struct HLSLFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::SpirvFlag : sulphur::builder::Flag
    *@brief flag specifying shader target
    *@author Stan Pepels
    */
    struct SpirvFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::IncludeDirFlag : sulphur::builder::Flag
    *@brief flag specifying additional folders that should be searched for include files
    *@author Stan Pepels
    */
    struct IncludeDirFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::MipMapFlag : sulphur::builder::Flag
    *@brief flag specifying the texture pipeline to generate mipmaps
    *@author Stan Pepels
    */
    struct MipMapFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::CompressionTypeFlag : sulphur::builder::Flag
    *@brief flag specifying a certain compressiontype for outputted files
    *@author Stan Pepels
    */
    struct CompressionTypeFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::VertexShaderFlag : sulphur::builder::Flag
    *@brief flag specifying a vertex shader
    *@author Timo van Hees
    */
    struct VertexShaderFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::PixelShaderFlag : sulphur::builder::Flag
    *@brief flag specifying a pixel shader
    *@author Timo van Hees
    */
    struct PixelShaderFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::GeometryShaderFlag : sulphur::builder::Flag
    *@brief flag specifying a geometry shader
    *@author Timo van Hees
    */
    struct GeometryShaderFlag : public Flag
    {
      /**
      *@see sulphur::builder::Flag::GetKey
      */
      const char* GetKey() const override;
    };

    /**
    *@struct sulphur::builder::OutputLocationFlag : sulphur::builder::Flag
    *@brief flag specifying the output directory
    *@author Timo van Hees
    */
    struct OutputLocationFlag : public Flag
    {
      const char* GetKey() const override;
    };
  }
}