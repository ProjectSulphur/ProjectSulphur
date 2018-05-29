#include "foundation/io/binary_reader.h"
#include "foundation/io/filesystem.h"
#include "foundation/logging/logger.h"
#include "foundation/io/binary_writer.h"
#include <fstream>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------------
    BinaryReader::BinaryReader(const Path& file, bool missing_file_error)
    {
      read_pos_ = 0;
      is_ok_ = false;

      foundation::Path f(file);

      std::ifstream in_file(f.GetString().c_str(), std::ios::binary | std::ios::ate);
      if(in_file.is_open())
      {
        unsigned int size = static_cast<unsigned int>(in_file.tellg());
        in_file.seekg(0, std::ios::beg);
        
        // Read the compression header
        char compression_prefix[sizeof(PS_COMPRESSION_PREFIX)];
        in_file.read(compression_prefix, sizeof(PS_COMPRESSION_PREFIX));
        if(strcmp(compression_prefix, PS_COMPRESSION_PREFIX) != 0)
        {
          in_file.seekg(0, std::ios::beg);
          data_.resize(size);
          in_file.read(reinterpret_cast<char*>(data_.data()), size);
        }
        else
        {
          size -= sizeof(PS_COMPRESSION_PREFIX);
          Vector<unsigned char> compressed_data(size);
          in_file.read(reinterpret_cast<char*>(compressed_data.data()), size);
          if(Decompressor::Decompress(compressed_data, data_) == false)
          {
            PS_LOG_WITH(foundation::LineAndFileLogger, Error, "Failed to decompress file: %s",
              file.GetString().c_str());
          }
        }

        in_file.close();

        is_ok_ = true;
      }
      else if(missing_file_error == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, "Failed to read from file: %s", 
          file.GetString().c_str());
      }
    }

    //--------------------------------------------------------------------------------
    BinaryReader::BinaryReader(unsigned char* data, int length)
    {
      read_pos_ = 0;
      if (length <= 0)
      {
        is_ok_ = false;
        return;
      }
      else
      {
        is_ok_ = true;
      }

      data_.resize(length);
      memcpy_s(data_.data(), length, data, length);
    }

    //--------------------------------------------------------------------------------
    String BinaryReader::GetDataAsString() const
    {
      if (data_.empty() == true)
      {
        return String();
      }

      String result;
      result = reinterpret_cast<const char*>(data_.data());
      result.resize(GetSize());
      return result;
    }

    //--------------------------------------------------------------------------------
    bool BinaryReader::ReadBoolean()
    {
      return (ReadUnsigned8() > 0);
    }

    //--------------------------------------------------------------------------------
    uint8_t BinaryReader::ReadUnsigned8()
    {
      if (read_pos_ >= GetSize())
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Reading outside the buffer. Read position: %i Bytes to read: %i Buffer size: %i",
        read_pos_, sizeof(uint8_t), data_.size());
        return 0;
      }

      return data_[read_pos_++];
    }

    //--------------------------------------------------------------------------------
    uint16_t BinaryReader::ReadUnsigned16()
    {
      uint16_t result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    uint32_t BinaryReader::ReadUnsigned32()
    {
      uint32_t result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    uint64_t BinaryReader::ReadUnsigned64()
    {
      uint64_t result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    int BinaryReader::ReadInt32()
    {
      int result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    float BinaryReader::ReadFloat()
    {
      float result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    double BinaryReader::ReadDouble()
    {
      double result = 0;
      ReadBytes(reinterpret_cast<unsigned char*>(&result), sizeof(result));
      return result;
    }

    //--------------------------------------------------------------------------------
    String BinaryReader::ReadString()
    {
      String result = "";
      const unsigned length = ReadUnsigned32();
      result.resize(length);
      ReadBytes(reinterpret_cast<unsigned char*>(&result[0]), length);
      return result;
    }

    //--------------------------------------------------------------------------------
    void BinaryReader::ReadBytes(unsigned char* outbuffer, unsigned int length)
    {
      if (length == 0)
      {
        return;
      }

      if(length > GetSize() - read_pos_)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Reading outside the buffer. Read position: %i Bytes to read: %i Buffer size: %i",
          read_pos_, length, data_.size());
        return;
      }

      memcpy_s(outbuffer, length, &data_[read_pos_], length);
      read_pos_ += length;
    }

    //--------------------------------------------------------------------------------
    void BinaryReader::Seek(unsigned int read_pos)
    {
      if(read_pos >= GetSize())
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "read_pos exceeds the size of the buffer.");
        return;
      }

      read_pos_ = read_pos;
    }

    //--------------------------------------------------------------------------------
    void BinaryReader::ReadSerializable(void* serializable)
    {
      static_cast<IBinarySerializable*>(serializable)->Read(*this);
    }
  }
}
