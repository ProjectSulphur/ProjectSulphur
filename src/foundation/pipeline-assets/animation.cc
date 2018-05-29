#include "animation.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void AnimationChannel::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(bone_name);
      binary_writer.Write(position_keys);
      binary_writer.Write(rotation_keys);
      binary_writer.Write(scale_keys);
    }

    //--------------------------------------------------------------------------------
    void AnimationChannel::Read(BinaryReader& binary_reader)
    {
      bone_name = binary_reader.ReadString();
      position_keys = binary_reader.ReadVector<AnimationVectorKey>();
      rotation_keys = binary_reader.ReadVector<AnimationQuaternionKey>();
      scale_keys = binary_reader.ReadVector<AnimationVectorKey>();
    }

    //--------------------------------------------------------------------------------
    void AnimationData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(duration);
      binary_writer.Write(ticks_per_second);
      binary_writer.Write(channels);
    }

    //--------------------------------------------------------------------------------
    void AnimationData::Read(BinaryReader& binary_reader)
    {
      duration = binary_reader.ReadFloat();
      ticks_per_second = binary_reader.ReadFloat();
      channels = binary_reader.ReadVector<AnimationChannel>();
    }
  }
}
