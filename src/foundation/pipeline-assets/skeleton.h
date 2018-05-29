#pragma once
#include "foundation/utils/asset_definitions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace sulphur 
{
	namespace foundation 
	{
    /**
    * @struct sulphur::foundation::Bone : sulphur::foundation::IBinarySerializable
    * @brief Bone of a skeleton.
    * @author Timo van Hees
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

      unsigned int parent;            //!< The index of the parent bone.
      glm::mat4 transform;            //!< The t-pose transform of the bone.
      Vector<unsigned int> children;  //!< The bone indices of the child bones.
    };

    /**
    * @struct sulphur::foundation::SkeletonData : sulphur::foundation::IBinarySerializable
    * @brief Bone data of a skeleton to store in the package.
    * @author Timo van Hees
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

      Map<String, unsigned> bone_names; //!< The names of the bones in this skeleton and their bone index.
      Vector<Bone> bones;               //!< List of bones.
    };

    /**
    * @struct sulphur::foundation::SkeletonAsset
    * @brief Skeleton loaded from a file.
		* @author Timo van Hees
    */
		struct SkeletonAsset
		{
		  AssetName name;     //!< The name of the skeleton.
      AssetID id;         //!< The ID of the skeleton.
      SkeletonData data;  //!< The bone data of the skeleton.
		};
	}
}
