#include "foundation/utils/compression.h"

#include <lz4.h>
#include <lz4hc.h>

namespace sulphur
{
  namespace foundation
  {
    //-------------------------------------------------------------------------
    int Compressor::Compress(const char* src, int src_size, char* dst, int dst_size, 
      CompressionType type)
    {
      const int size_of_int = sizeof(int);

      int compressed_size = 0;
      switch (type)
      {
      case CompressionType::kNone:
        memcpy_s(dst, dst_size, src, src_size);
        return (src_size < dst_size) ? src_size : dst_size;
      case CompressionType::kFast:
        compressed_size = LZ4_compress_fast(src, dst + size_of_int,
          src_size, dst_size - size_of_int, 16);
        break;
      case CompressionType::kDefault:
        compressed_size = LZ4_compress_default(src, dst + size_of_int,
          src_size, dst_size - size_of_int);
        break;
      case CompressionType::kHighCompression:
        compressed_size = LZ4_compress_HC(src, dst + size_of_int,
          src_size, dst_size - size_of_int, LZ4HC_CLEVEL_MAX);
        break;
      default:
        break;
      }

      if (compressed_size <= 0)
      {
        return 0;
      }

      *reinterpret_cast<int*>(dst) = src_size;

      return compressed_size + size_of_int;
    }

    //-------------------------------------------------------------------------
    int Compressor::GetWorstCaseSize(int src_size)
    {
      const int worst_case_size = LZ4_compressBound(src_size);
      const int size_of_int = sizeof(int);
      return worst_case_size + size_of_int;
    }

    //-------------------------------------------------------------------------
    int Decompressor::Decompress(const char* src, int src_size, char* dst, int dst_size)
    {      
      const int decompressed_size = GetDecompressedSize(src);
      if(decompressed_size > dst_size)
      {
        return 0;
      }

      const int size_of_int = sizeof(int);

      bool result = LZ4_decompress_safe(src + size_of_int, dst,
        src_size - size_of_int, decompressed_size) >= 0;

      if(result == false)
      {
        return 0;
      }

      return decompressed_size;
    }

    //-------------------------------------------------------------------------
    int Decompressor::GetDecompressedSize(const char* src)
    {
      return *reinterpret_cast<const int*>(src);
    }
  }
}