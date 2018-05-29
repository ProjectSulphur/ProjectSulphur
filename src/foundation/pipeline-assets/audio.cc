#include "foundation/pipeline-assets/audio.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void AudioBankData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(data);
    }

    //--------------------------------------------------------------------------------
    void AudioBankData::Read(BinaryReader& binary_reader)
    {
      data = binary_reader.ReadVector<char>();
    }
  }
}
