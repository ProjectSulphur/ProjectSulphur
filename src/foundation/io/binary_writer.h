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
    * @class sulphur::foundation::BinaryWriter
    * @brief Helper class for writing binary data to files.
    * @author Timo van Hees
    */
    class BinaryWriter
    {
    public:
      /**
      * @brief Create a binary writer and sets the file to save the written data to.
      * @param[in] file (const sulphur::foundation::String&) The file to write to.
      */
      BinaryWriter(const String& file);
      BinaryWriter() = default;
      ~BinaryWriter() = default;

      /**
       * @brief Writes the data written to the buffer to a file.
       * @return (bool) If the data was written successfully.
       */
      bool Save();

      /**
      * @brief Writes the data written to the buffer to a file.
      * @param[in] file (const sulphur::foundation::String&) The file to output to.
      * @return (bool) If the data was written successfully.
      */
      bool Save(const String& file);

      /**
       * @brief Write a boolean to the buffer.
       * @param val (bool) The boolean value to write.
       */
      void Write(bool val);
      /**
      * @brief Write an 8 bit unsigned integer to the buffer.
      * @param val (uint8_t) The 8 bit unsigned integer value to write.
      */
      void Write(uint8_t val);
      /**
      * @brief Write an 16 bit unsigned integer to the buffer.
      * @param val (uint16_t) The 16 bit unsigned integer value to write.
      */
      void Write(uint16_t val);
      /**
      * @brief Write an 32 bit unsigned integer to the buffer.
      * @param val (uint32_t) The 32 bit unsigned integer value to write.
      */
      void Write(uint32_t val);
      /**
      * @brief Write an 64 bit unsigned integer to the buffer.
      * @param val (uint64_t) The 64 bit unsigned integer value to write.
      */
      void Write(uint64_t val);
      /**
      * @brief Write an integer to the buffer.
      * @param val (int) The integer value to write.
      */
      void Write(int val);
      /**
      * @brief Write a float to the buffer.
      * @param val (float) The float value to write.
      */
      void Write(float val);
      /**
      * @brief Write a double to the buffer.
      * @param val (double) The double value to write.
      */
      void Write(double val);
      /**
      * @brief Write a string to the buffer.
      * @param val (sulphur::foundation::String) The string value to write.
      */
      void Write(const String& val);
      /**
      * @brief Write an array to the buffer.
      * @param arrayptr (const char*) Pointer to an array of data.
      * @param len (unsigned int) The length of the array.
      */
      void Write(const char* arrayptr, unsigned int len);
      /**
      * @brief Write an array to the buffer.
      * @param arrayptr (const uint8_t*) Pointer to an array of data.
      * @param len (unsigned int) The length of the array.
      */
      void Write(const uint8_t* arrayptr, unsigned int len);
      /**
      * @brief Write an array to the buffer.
      * @param arrayptr (const unsigned int*) Pointer to an array of data.
      * @param len (unsigned int) The length of the array.
      */
      void Write(const unsigned int* arrayptr, unsigned int len);

      /**
       * @brief Write any type to the buffer.
       * @tparam T The type to write.
       * @param val (const T&) Variable of type T.
       */
      template<typename T>
      void Write(const T& val);

      /**
       * @brief Write a vector of any type to the buffer.
       * @tparam T The type in the vector.
       * @param val (const sulphur::foundation::Vector <T>&) The vector to write.
       */
      template<typename T>
      void Write(const Vector<T>& val);

      /**
       * @brief Write a map of any type of key and value to the buffer.
       * @tparam Key The type of the keys.
       * @tparam Value The type of the values.
       * @param val (const sulphur::foundation::Map <Key, Value>&) The map to write.
       */
      template<typename Key, typename Value>
      void Write(const Map<Key, Value>& val);

      /**
       * @brief Get the data in the buffer.
       * @return (const sulphur::foundation::Vector <unsigned char>&) The buffer.
       */
      const Vector<unsigned char>& get_data() const { return data_; }
      /**
       * @brief Get the size of the buffer.
       * @return (unsigned int) The size of the buffer in bytes.
       */
      unsigned int GetSize() const { return static_cast<unsigned int>(data_.size()); }

    private:
      /**
       * @brief Writes data using sulphur::foundation::IBinarySerializable::Write.
       * @param[in] serializable (const void*) Pointer to a type that derives 
       * from IBinarySerializable.
       */
      void WriteWriteable(const void* serializable);

      String file_;                 //!< The file to output the buffer to when calling save.
      Vector<unsigned char> data_;  //!< The buffer of written data.
    };

    template<typename T>
    void BinaryWriter::Write(const T& val)
    {
      if(std::is_base_of<IBinarySerializable, T>())
      {
        WriteWriteable(&val);
      }
      else
      {
        Write(reinterpret_cast<const unsigned char*>(&val), sizeof(T));
      }
    }

    template<typename T>
    void BinaryWriter::Write(const Vector<T>& val)
    {
      Write(val.size());
      if (std::is_base_of<IBinarySerializable, T>())
      {
        for (int i = 0; i < val.size(); ++i)
        {
          WriteWriteable(&val[i]);
        }
      }
      else
      {
        Write(reinterpret_cast<const unsigned char*>(val.data()), 
          static_cast<unsigned int>(sizeof(T) * val.size()));
      }
    }

    template <typename Key, typename Value>
    void BinaryWriter::Write(const Map<Key, Value>& val)
    {
      Write(val.size());
      for(const eastl::pair<Key, Value>& it : val)
      {
        Write(it.first);
        Write(it.second);
      }
    }
  }
}
