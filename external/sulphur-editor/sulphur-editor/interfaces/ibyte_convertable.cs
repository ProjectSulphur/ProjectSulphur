using System;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.ByteToClassConverter
     *@brief class that converts a byte array to a certain class or struct that implements the sulphur.editor.IByteConvertable interface
     *@remark this class reads byte arrays and converts them to classe. This class behaves muck like the sulphur.editor.BinaryReader class except that it does not do file operations.
     *@todo: actually implement the class when we need it 
     *@see sulphur::foundation::BinaryReader. 
     *@author Stan Pepels    
     */
    class ByteToClassConverter
    { }

    /**
     *@class sulphur.editor.ClassToByteConverter
     *@brief class that converts a certain class or struct that implements the sulphur.editor.IByteConvertable interface to a byte array
     *@remark this class reads a byte array into a class. This class behaves muck like the sulphur::foundation::BinaryWriter class except that it does not do file operations
     *@todo: actually implement the class when we need it 
     *@author Stan Pepels    
     */
    class ClassToByteConverter
    { }

    /**
     *@interface sulphur.editor.IByteConvertable
     *@brief interface used for converteng classes from and to byte arrays
     *@author Stan Pepels  
     */
    interface IByteConvertable
    {
      /**
       *@brief convert a class to a byte array
       *@param[out] data (suphur.editor.ClassToByteConverter) class filled with the byte array constructed from this class.
       */
      void ToBytes(out ClassToByteConverter data);

      /**
       *@brief convert a byte array to a class
       *@param[in] converter (suphur.editor.ByteToClassConverter) instance containing the byte array containing the data used to construct this class
       */
      void FromBytes(ByteToClassConverter converter);
    }
  }
}