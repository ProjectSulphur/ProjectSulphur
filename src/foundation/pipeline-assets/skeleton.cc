#include "skeleton.h"

namespace sulphur
{
  namespace foundation
  {
    //------------------------------------------------------------------------------------------------------
    void SkeletalNode::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(name);
      binary_writer.Write(transform);
      binary_writer.Write(parent);
      binary_writer.Write(children);
    }

    //------------------------------------------------------------------------------------------------------
    void SkeletalNode::Read(BinaryReader& binary_reader)
    {

      name = binary_reader.ReadString();
      transform = binary_reader.Read<glm::mat4>();
      parent = binary_reader.Read<unsigned int>();
      children = binary_reader.ReadVector<unsigned int>();
    }

    //--------------------------------------------------------------------------------
    void Bone::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(transform);
    }

    //--------------------------------------------------------------------------------
    void Bone::Read(BinaryReader& binary_reader)
    {
      transform = binary_reader.Read<glm::mat4>();
    }

    //--------------------------------------------------------------------------------
    void SkeletonData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(root_node);
      binary_writer.Write(nodes);
      binary_writer.Write(bones);
      binary_writer.Write(bone_names);
    }

    //--------------------------------------------------------------------------------
    void SkeletonData::Read(BinaryReader& binary_reader)
    {
      root_node = binary_reader.Read<unsigned int>();
      nodes = binary_reader.ReadVector<SkeletalNode>();
      bones = binary_reader.ReadVector<Bone>();
      bone_names = binary_reader.ReadMap<String, unsigned>();
    }
  }
}
