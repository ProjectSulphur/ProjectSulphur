#pragma once
#include "foundation/utils/asset_definitions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace sulphur
{
  namespace foundation
  {
    /**
    * @struct sulphur::foundation::SkeletalNode : sulphur::foundation::IBinarySerializable
    * @brief A SkeletalNode represents any node that lives in the original scene from which the skeleton was originally loaded.
    * @remarks The SkeletalNode is necessary because it makes sure that there is a proper transform-chain between bones & nodes.
    * @remarks The SkeletalNode is also necessary because it maintains the parent-child hierarchy from the original scene.
    * @author Riko Ophorst, Timo van Hees
    */
    class SkeletalNode : public IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable::Read
      */
      void Read(BinaryReader& binary_reader) override;

      String name;                    //!< The name of the node as it was in the original scene.
      glm::mat4 transform;            //!< The transform of the node as it was in the original scene.
      unsigned int parent;            //!< The index of the parent node that resides at SkeletonData::nodes.
      Vector<unsigned int> children;  //!< The indices of the child nodes that reside at SkeletonData::nodes. 
    };

    /**
    * @class sulphur::foundation::Bone : sulphur::foundation::IBinarySerializable
    * @brief Bone of a skeleton.
    * @author Riko Ophorst, Timo van Hees
    */
    class Bone : public IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable::Read
      */
      void Read(BinaryReader& binary_reader) override;

      glm::mat4 transform;  //!< The t-pose transform of the bone.
    };

    /**
    * @struct sulphur::foundation::SkeletonData : sulphur::foundation::IBinarySerializable
    * @brief Bone data of a skeleton to store in the package.
    * @author Riko Ophorst, Timo van Hees
    */
    class SkeletonData : public IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Read(BinaryReader& binary_reader) override;

      unsigned int root_node;               //!< The index of the root skeletal node of the skeleton. Slots into the nodes array.
      Vector<SkeletalNode> nodes;           //!< The array of skeletal nodes that this skeleton consists of.
      Vector<Bone> bones;                   //!< A vector of Bones stored in the Skeleton.
      Map<String, unsigned int> bone_names; //!< Stores a map of bone names that map to indices in the bones array.
    };

    /**
    * @struct sulphur::foundation::SkeletonAsset
    * @brief Skeleton loaded from a file.
    * @author Riko Ophorst, Timo van Hees
    */
    struct SkeletonAsset
    {
      AssetName name;     //!< The name of the skeleton.
      AssetID id;         //!< The ID of the skeleton.
      SkeletonData data;  //!< The bone data of the skeleton.
    };
  }
}
