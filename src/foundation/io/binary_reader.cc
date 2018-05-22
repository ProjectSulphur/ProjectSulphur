#include "binary_reader.h"
#include "foundation/logging/logger.h"
#include <fstream>
#include "filesystem.h"

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------------
    BinaryReader::BinaryReader(const String& file, bool missing_file_error)
    {
      read_pos_ = 0;
      is_ok_ = false;

      foundation::SystemPath f(file);

      std::ifstream in_file(f, std::ios::binary | std::ios::ate);
      if(in_file.is_open())
      {
        const unsigned int size = static_cast<unsigned int>(in_file.tellg());
        data_.resize(size);
        in_file.seekg(0, std::ios::beg);
        in_file.read(reinterpret_cast<char*>(data_.data()), size);
        in_file.close();
        is_ok_ = true;
      }
      else if(missing_file_error == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, "Failed to read from file: %s", file.c_str());
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
