#include "skeleton.h"

namespace sulphur 
{
	namespace foundation 
	{
    //--------------------------------------------------------------------------------
	  void Bone::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(parent);
      binary_writer.Write(transform);
      binary_writer.Write(children);
	  }

    //--------------------------------------------------------------------------------
	  void Bone::Read(BinaryReader& binary_reader)
	  {
      parent = binary_reader.ReadUnsigned32();
      transform = binary_reader.Read<glm::mat4>();
      children = binary_reader.ReadVector<unsigned>();
	  }

	  //--------------------------------------------------------------------------------
	  void SkeletonData::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(bone_names);
      binary_writer.Write(bones);
	  }

    //--------------------------------------------------------------------------------
	  void SkeletonData::Read(BinaryReader& binary_reader)
	  {
      bone_names = binary_reader.ReadMap<String, unsigned>();
      bones = binary_reader.ReadVector<Bone>();
	  }
	}
}
