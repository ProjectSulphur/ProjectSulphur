#include "foundation/io/binary_writer.h"
#include "foundation/logging/logger.h"
#include <fstream>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------------
    BinaryWriter::BinaryWriter(const Path& file) : file_(file)
    {}

    //--------------------------------------------------------------------------------
    bool BinaryWriter::Save()
    {
      return SaveCompressed(file_.GetString(), CompressionType::kNone);
    }

    //--------------------------------------------------------------------------------
    bool BinaryWriter::Save(const String& file)
    {
      return SaveCompressed(file, CompressionType::kNone);
    }

    //--------------------------------------------------------------------------------
    bool BinaryWriter::SaveCompressed(CompressionType type)
    {
      return SaveCompressed(file_.GetString(), type);
    }

    //--------------------------------------------------------------------------------
    bool BinaryWriter::SaveCompressed(const String& file, CompressionType type)
    {
      if (file.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "No file name given. Nothing will be saved.");
        return false;
      }

      if (data_.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Nothing was written before saving. Nothing will be saved.");
        return false;
      }

      Vector<unsigned char>* data = &data_;
      Vector<unsigned char> compressed_data = {};
      if (type != CompressionType::kNone)
      {
        if(Compressor::Compress(data_, type, compressed_data) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
            "Data could not be compressed. Nothing will be saved.");
          return false;
        }
        data = &compressed_data;
      }

      std::ofstream out_file(file.c_str(), std::ios::binary | std::ios::trunc);
      if (out_file.is_open() == true)
      {
        if(type != CompressionType::kNone)
        {
          // Write compression header
          out_file.write(PS_COMPRESSION_PREFIX, sizeof(PS_COMPRESSION_PREFIX));
        }

        // Write the data
        out_file.write(reinterpret_cast<char*>(data->data()), data->size());
        out_file.close();
      }
      else
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to save to file. file: %s", file.c_str());
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(bool val)
    {
      Write(val ? uint8_t(1) : uint8_t(0));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(uint8_t val)
    {
      data_.push_back(val);
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(uint16_t val)
    {
      Write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(uint32_t val)
    {
      Write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(uint64_t val)
    {
      Write(reinterpret_cast<const char*>(&val), sizeof(val));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(int val)
    {
      Write(static_cast<uint32_t>(val));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(float val)
    {
      Write<float>(val);
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(double val)
    {
      Write<double>(val);
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(const String& val)
    {
      Write(static_cast<uint32_t>(val.length()));
      Write(val.data(), static_cast<int>(val.length()));
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(const Path& val)
    {
      Write(val.GetString());
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(const char* arrayptr, unsigned int len)
    {
      const size_t offset = data_.size();
      data_.resize(offset + len);
      memcpy_s(data_.data() + offset, len, arrayptr, len);
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::WriteWriteable(const void* serializable)
    {
      static_cast<const IBinarySerializable*>(serializable)->Write(*this);
    }

  //--------------------------------------------------------------------------------
    void BinaryWriter::Write(const uint8_t* arrayptr, unsigned int len)
    {
      const size_t offset = data_.size();
      data_.resize(offset + len);
      memcpy_s(data_.data() + offset, len, arrayptr, len);
    }

    //--------------------------------------------------------------------------------
    void BinaryWriter::Write(const unsigned int* arrayptr, unsigned int len)
    {
      const size_t offset = data_.size();
      data_.resize(offset + len * sizeof(unsigned int));
      memcpy_s(data_.data() + offset, len * sizeof(unsigned int), arrayptr, len * sizeof(unsigned int));
    }
  }
}
