#pragma once
#include "../containers/string.h"
#include "../containers/vector.h"
#include "../containers/map.h"
#include "binary_serializable.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @class sulphur::foundation::BinaryReader
     * @brief Helper class for reading binary data from files.
     * @author Timo van Hees
     */
    class BinaryReader
    {
    public:
      /**
       * @brief Create a binary reader and fill its buffer with the contents of a file.
       * @param[in] file (const sulphur::foundation::String&) The file to read using 
       * the binary reader.
       * @param[opt in] missing_file_error (bool) If an error should be thrown 
       * if the file doesn't exist.
       */
      BinaryReader(const String& file, bool missing_file_error = true);
      /**
       * @brief Create a binary reader that reads from a buffer.
       * @param[in] data (unsigned char*) Pointer to the buffer to read.
       * @param[in] length (int) The length of the buffer in bytes. 
       */
      BinaryReader(unsigned char* data, int length);
      BinaryReader() = default;
      ~BinaryReader() = default;

      /**
       * @return (sulphur::foundation::String) The data in the buffer as a string.
       */
      String GetDataAsString() const;
      /**
       * @brief Reads a boolean from the buffer.
       * @return (bool) The boolean value read from the buffer.
       */
      bool ReadBoolean();
      /**
       * @brief Read a 8 bit unsigned integer from the buffer.
       * @return (uint8_t) The 8 bit unsigned integer read from the buffer.
       */
      uint8_t ReadUnsigned8();
      /**
      * @brief Read a 16 bit unsigned integer from the buffer.
      * @return (uint16_t) The 16 bit unsigned integer read from the buffer.
      */
      uint16_t ReadUnsigned16();
      /**
      * @brief Read a 32 bit unsigned integer from the buffer.
      * @return (uint32_t) The 32 bit unsigned integer read from the buffer.
      */
      uint32_t ReadUnsigned32();
      /**
      * @brief Read a 64 bit unsigned integer from the buffer.
      * @return (uint64_t) The 64 bit unsigned integer read from the buffer.
      */
      uint64_t ReadUnsigned64();
      /**
      * @brief Read a integer from the buffer.
      * @return (int) The integer read from the buffer.
      */
      int ReadInt32();
      /**
      * @brief Read a float from the buffer.
      * @return (float) The float read from the buffer.
      */
      float ReadFloat();
      /**
      * @brief Read a double from the buffer.
      * @return (double) The double read from the buffer.
      */
      double ReadDouble();
      /**
      * @brief Read a string from the buffer.
      * @return (sulphur::foundation::String) The string read from the buffer.
      */
      String ReadString();
      /**
       * @brief Read a number of bytes from the buffer.
       * @param[out] out_buffer (unsigned char*) A buffer of @length amount of bytes.
       * @param[in] length (unsigned int) The amount of bytes to read.
       */
      void ReadBytes(unsigned char* out_buffer, unsigned int length);

      /**
       * @brief Read any type.
       * @tparam T The type to read.
       * @return (T) The type initialized with the data from the buffer.
       */
      template<typename T>
      T Read();

      /**
       * @brief Read a vector of any type from the buffer.
       * @tparam T The type in the vector.
       * @return (sulphur::foundation::Vector <T>) The vector initialized with 
       * the data in the buffer.
       */
      template<typename T>
      Vector<T> ReadVector();

      /**
       * @brief Read a map of any type of key and value from the buffer.
       * @tparam Key The type of the keys.
       * @tparam Value The type of the values.
       * @return (sulphur::foundation::Map <Key, Value>) The map initialized the 
       * data from the buffer.
       */
      template<typename Key, typename Value>
      Map<Key, Value> ReadMap();

      /**
       * @brief Jump to a position in the buffer.
       * @param read_pos (unsigned int) The position to jump to. 0 = Begin of the file.
       */
      void Seek(unsigned int read_pos);

      /**
       * @brief Get the buffer.
       * @return (const sulphur::foundation::Vector <unsigned char>&) The data in the buffer.
       */
      const Vector<unsigned char>& data() const { return data_; }
      /**
       * @brief Get the size of the buffer.
       * @return (unsigned int) The size of the buffer.
       */
      unsigned int GetSize() const { return static_cast<unsigned int>(data_.size()); }
      /**
       * @brief Get if the buffer was initialized successfully.
       * @return (bool) If the buffer was initialized successfully.
       */
      bool is_ok() const { return is_ok_; }
      /**
       * @brief Get the current reading position in the buffer.
       * @return (unsigned int) The current reading position.
       */
      unsigned int read_pos() const { return read_pos_; }

    private:
      /**
       * @brief Reads data using sulphur::foundation::IBinarySerializable::Read.
       * @param[in] serializable (void*) Pointer to a type that derives from IBinarySerializable. 
       */
      void ReadSerializable(void* serializable);

      Vector<unsigned char> data_;     //!< The buffer the reader reads from.
      unsigned int read_pos_; //!< The reading position in the buffer.
      bool is_ok_;            //!< If the reader was initialized successfully.
    };

    template<typename T>
    T BinaryReader::Read()
    {
      T outval;

      if(std::is_base_of<IBinarySerializable, T>())
      {
        ReadSerializable(&outval);
      }
      else
      {
        ReadBytes(reinterpret_cast<unsigned char*>(&outval), sizeof(T));
      }

      return outval;
    }

    /**
    * @see sulphur::foundation::BinaryReader::Read
    */
    template<>
    inline String BinaryReader::Read<String>()
    {
      return ReadString();
    }

    template <typename T>
    Vector<T> BinaryReader::ReadVector()
    {
      const size_t size = ReadUnsigned64();
      Vector<T> result(size);

      if (std::is_base_of<IBinarySerializable, T>())
      {
        for (int i = 0; i < result.size(); ++i)
        {
          ReadSerializable(&result[i]);
        }
      }
      else
      {
        ReadBytes(reinterpret_cast<unsigned char*>(result.data()),
          static_cast<unsigned int>(sizeof(T) * size));
      }
      
      return result;
    }

    template <typename Key, typename Value>
    Map<Key, Value> BinaryReader::ReadMap()
    {
      Map<Key, Value> result;
      size_t size = ReadUnsigned64();
      for(int i = 0; i < size; ++i)
      {
        Key k = Read<Key>();
        Value v = Read<Value>();
        result[k] = v;
      }
      return result;
    }
  }
}
