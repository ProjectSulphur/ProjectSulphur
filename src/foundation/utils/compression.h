#pragma once

#include "foundation/containers/vector.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @brief Types of compression.
     */
    enum struct CompressionType
    {
      kNone,            //!< No compression
      kFast,            //!< Fast compression speed with low compression ratio
      kDefault,         //!< Medium compression speed with average compression ratio
      kHighCompression  //!< Slow compression speed with high compression ratio
    };

    /**
     * @class sulphur::foundation::Compressor
     * @brief Compresses data.
     * @author Timo van Hees.
     */
    class Compressor
    {
    public:
      /**
       * @brief Compresses the data in src and puts it in dst.
       * @param[in] src (const char*) Pointer to the source data.
       * @param[in] src_size (int) The size of the source data.
       * @param[in|out] dst (char*) The output buffer. Most already be allocated.
       * @param[in] dst_size (int) The size of the output buffer.
       * @param[in] type (sulphur::foundation::CompressionType) 
       * How fast the data should be compressed.
       * @return (int) The size of the compressed data or 0 if compression failed.
       * @remark Use sulphur::foundation::Compressor::GetWorstCaseSize to get 
       * the recommended size of dst.
       */
      static int Compress(const char* src, int src_size, char* dst, int dst_size, 
        CompressionType type);
      /**
       * @brief Compresses the data in a vector.
       * @tparam T The type of the vector.
       * @param[in] data (const sulphur::foundation::Vector <char>&) Data to compress.
       * @param[in] type (sulphur::foundation::CompressionType) 
       * How fast the data should be compressed.
       * @param[out] compressed (sulphur::foundation::Vector <char>&) 
       * Output for the compressed data. Can be empty.
       * @return (bool) True if the data compressed succesfully.
       */
      template<typename T>
      static bool Compress(const Vector<T>& data, CompressionType type,
        Vector<T>& compressed);
      /**
       * @brief Returns the recommended size of an output buffer for compression.
       * @param src_size (int) The size of the data that will be compressed.
       * @return (int) The recommended size of the buffer.
       */
      static int GetWorstCaseSize(int src_size);
    };

    //-------------------------------------------------------------------------
    template <typename T>
    bool Compressor::Compress(const Vector<T>& data, CompressionType type,
      Vector<T>& compressed)
    {
      int size_of_type = sizeof(T);
      int size = static_cast<int>(data.size()) * size_of_type;
      int worst_case_size = GetWorstCaseSize(size);
      compressed.resize(static_cast<size_t>(worst_case_size));
      const int compressed_size = Compress(reinterpret_cast<const char*>(data.data()), size,
        reinterpret_cast<char*>(compressed.data()), worst_case_size, type);
      compressed.resize(compressed_size);

      return compressed_size > 0;
    }

    /**
    * @class sulphur::foundation::Decompressor
    * @brief Decompresses data.
    * @author Timo van Hees.
    */
    class Decompressor
    {
    public:
      /**
       * @brief Decompresses a buffer compressed using sulphur::foundation::Compressor.
       * @param src (const char*) The compressed data.
       * @param src_size (int) The size of the compressed data.
       * @param dst (char*) The output buffer for the decompressed data.
       * @param dst_size (int) The size of the output buffer.
       * @return (int) The size of the decompressed data or 0 if decompression failed.
       */
      static int Decompress(const char* src, int src_size, char* dst, int dst_size);
      /**
       * @brief Decompresses a buffer compressed using sulphur::foundation::Compressor.
       * @tparam T The type of the vector.
       * @param data (const sulphur::foundation::Vector <char>&) The compressed data.
       * @param decompressed (sulphur::foundation::Vector <char>&) 
       * Output for the decompressed data. Can be empty.
       * @return (bool) True if data was decompressed succesfully.
       */
      template<typename T>
      static bool Decompress(const Vector<T>& data, Vector<T>& decompressed);

      static int GetDecompressedSize(const char* src);
    };

    //-------------------------------------------------------------------------
    template <typename T>
    bool Decompressor::Decompress(const Vector<T>& data, Vector<T>& decompressed)
    {
      const int decompressed_size = GetDecompressedSize(reinterpret_cast<const char*>(data.data()));
      decompressed.resize(static_cast<size_t>(decompressed_size) / sizeof(T));
      return Decompress(reinterpret_cast<const char*>(data.data()), 
        static_cast<int>(data.size()) * sizeof(T), reinterpret_cast<char*>(decompressed.data()),
        decompressed_size) > 0;
    }
  }

}