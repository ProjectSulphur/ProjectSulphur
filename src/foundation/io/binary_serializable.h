#pragma once

namespace sulphur 
{
  namespace foundation 
  {
    class BinaryReader;
    class BinaryWriter;

    /**
     * @class sulphur::foundation::IBinarySerializable
     * @brief Interface for custom serialization behaviours when using 
     * sulphur::foundation::BinaryReader or sulphur::foundation::BinaryWriter.
     * @author Timo van Hees
     */
    class IBinarySerializable
    {
    public:
      virtual ~IBinarySerializable() = default;
      /**
       * @brief Writes data to a sulphur::foundation::BinaryWriter.
       * @param[in] binary_writer (sulphur::foundation::BinaryWriter&) 
       * The binary writer to write to.
       */
      virtual void Write(BinaryWriter& binary_writer) const = 0;
      /**
      * @brief Reads data from a sulphur::foundation::BinaryReader.
      * @param[in] binary_reader (sulphur::foundation::BinaryReader&)
      * The binary reader to read from.
      */
      virtual void Read(BinaryReader& binary_reader) = 0;
    };
  }
}
