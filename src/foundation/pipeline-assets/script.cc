#include "foundation/pipeline-assets/script.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void ScriptData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(binary);
    }

    //--------------------------------------------------------------------------------
    void ScriptData::Read(BinaryReader& binary_reader)
    {
      binary = binary_reader.ReadVector<char>();
    }
  }
}
