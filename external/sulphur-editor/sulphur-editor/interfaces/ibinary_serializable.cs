using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sulphur
{
  namespace editor
  {
    /**
     *@interface sulphur.editor.IBinarySerializable
     *@brief interface for reading more complex serialized classes written to disk with sulphur::foundation::BinaryReader
     *@author Stan Pepels   
     */
    public interface IBinarySerializable
    {
      /**
       *@brief function that reads from a file
       *@param[in] reader (sulphur.editor.BinaryReader) reader to read the object from  
       */
      void Read(BinaryReader reader);
    }
  }
}
