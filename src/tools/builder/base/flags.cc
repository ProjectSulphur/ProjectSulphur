#include "tools/builder/base/flags.h"

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    const char* DirFlag::GetKey() const
    {
      return "dir";
    }

    //-----------------------------------------------------------------------------------------------
    const char* FileFlag::GetKey() const
    {
      return "file";
    }
    
    //-----------------------------------------------------------------------------------------------
    const char* RecursiveFlag::GetKey() const
    {
      return "r";
    }

    //-----------------------------------------------------------------------------------------------
    const char* SingleFlag::GetKey() const
    {
      return "single";
    }

    //-----------------------------------------------------------------------------------------------
    const char* PSSLFlag::GetKey() const
    {
      return "pssl";
    }

    //-----------------------------------------------------------------------------------------------
    const char* HLSLFlag::GetKey() const
    {
      return "hlsl";
    }

    //-----------------------------------------------------------------------------------------------
    const char* SpirvFlag::GetKey() const
    {
      return "spirv";
    }

    //-----------------------------------------------------------------------------------------------
    const char * IncludeDirFlag::GetKey() const
    {
      return "incl_dir";
    }

    //-----------------------------------------------------------------------------------------------
    const char * MipMapFlag::GetKey() const
    {
      return "mipmap";
    }

    //-----------------------------------------------------------------------------------------------
    const char * CompressionTypeFlag::GetKey() const
    {
      return "compr";
    }

    //-----------------------------------------------------------------------------------------------
    const char* VertexShaderFlag::GetKey() const
    {
      return "vertex";
    }

    //-----------------------------------------------------------------------------------------------
    const char* PixelShaderFlag::GetKey() const
    {
      return "pixel";
    }

    //-----------------------------------------------------------------------------------------------
    const char* GeometryShaderFlag::GetKey() const
    {
      return "geom";
    }
    const char* OutputLocationFlag::GetKey() const
    {
      return "output";
    }
  }
}
