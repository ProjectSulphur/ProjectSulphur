using System;

namespace sulphur
{
  namespace editor
  {
    // interface to convert classes to and from bytes
    // TODO: write a class that takes in data and can read the data with alignments
    class ByteToClassConverter
    {
    }

    class ClassToByteConverter
    { }


    interface IByteConvertable
    {
      void ToBytes(out ClassToByteConverter data);
      void FromBytes(ByteToClassConverter converter);
    }
  }
}