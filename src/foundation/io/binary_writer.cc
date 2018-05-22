#include "binary_writer.h"
#include "foundation/logging/logger.h"
#include <fstream>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------------
    BinaryWriter::BinaryWriter(const String& file) : file_(file)
    {}

    //--------------------------------------------------------------------------------
    bool BinaryWriter::Save()
    {
      if (file_.empty() == true)
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

      std::ofstream out_file(file_.c_str(), std::ios::binary | std::ios::trunc);
      if (out_file.is_open() == true)
      {
        out_file.write(reinterpret_cast<char*>(data_.data()), GetSize());
        out_file.close();
      }
      else
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to save to file. file: %s", file_.c_str());
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool BinaryWriter::Save(const String& file)
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

      std::ofstream out_file(file.c_str(), std::ios::binary | std::ios::trunc);
      if (out_file.is_open() == true)
      {
        out_file.write(reinterpret_cast<char*>(data_.data()), GetSize());
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
