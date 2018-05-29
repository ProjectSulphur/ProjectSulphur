#pragma once
#include "foundation/utils/asset_definitions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @struct sulphur::foundation::AnimationVectorKey
    * @brief A key used in keyframe animations for position and scale.
    * @author Timo van Hees
    */
    struct AnimationVectorKey
    {
      float time;       //!< Timestamp of this key.
      glm::vec3 vector; //!< The keyframe value.
    };

    /**
    * @struct sulphur::foundation::AnimationQuaternionKey
    * @brief A key used in keyframe animations for rotation.
    * @author Timo van Hees
    */
    struct AnimationQuaternionKey
    {
      float time;           //!< Timestamp of this key.
      glm::quat quaternion; //!< The keyframe value.
    };

    /**
    * @class sulphur::foundation::AnimationChannel : public sulphur::foundation::IBinarySerializable
    * @brief Keyframe animation channel.
    * @author Timo van Hees
    */
    class AnimationChannel : public IBinarySerializable
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

      String bone_name; //!< The name of the bone this channel controlls.
      Vector<AnimationVectorKey> position_keys; //!< Positions keys
      Vector<AnimationQuaternionKey> rotation_keys; //!< Rotation keys
      Vector<AnimationVectorKey> scale_keys;  //!< Scale keys
    };


    /**
     * @class sulphur::foundation::AnimationData : public sulphur::foundation::IBinarySerializable
     * @brief The keyframe data of an animation to store in the package.
     * @author Timo van Hees
     */
    class AnimationData : public IBinarySerializable
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

      float duration; //!< The duration of the animation.
      float ticks_per_second; //!< The amount of keyframes per channel per second.
      Vector<AnimationChannel> channels; //!< The animation channels.
    };

    /**
    * @struct sulphur::foundation::AnimationAsset
    * @brief Keyframe animation loaded from a file.
    * @author Timo van Hees
    */
    struct AnimationAsset
    {
      AssetName name;     //!< The name of the animation.
      AssetID id;         //!< The ID of the animation.
      AnimationData data; //!< The keyframe data of the animation.
    };
  }
}
