#pragma once

#include "engine/assets/asset_interfaces.h"

#include <foundation/pipeline-assets/skeleton.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>

#include <glm/mat4x4.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::Bone
    * @brief Stores information about a Bone in a Skeleton.
    * @author Riko Ophorst
    */
    struct Bone
    {
    public:
      /** @brief Default constructor. */
      Bone();

      /** 
      * @brief Constructs a bone with some data.
      * @param[in] bone_transform (const glm::mat4x4&) The transformation on the Bone in bind-pose.
      * @param[in] parent (unsigned int) The index of the parent Bone stored in the owning Skeleton.
      * @param[in] children (const sulphur::foundation::Vector<unsigned int>&) The indices of the child Bones
      * stored in the owning Skeleton.
      */
      Bone(
        const glm::mat4x4& bone_transform, 
        unsigned int parent, 
        const foundation::Vector<unsigned int>& children
      );

      /** @brief Default destructor. */
      ~Bone();

      /** The transform of the Bone in local Bone space in bind-pose. */
      glm::mat4x4 bone_transform;

      /** The index of the parent Bone stored in the owning Skeleton. */
      unsigned int parent;

      /** The indices of child Bones stored in the owning Skeleton. */
      foundation::Vector<unsigned int> children;
    };

    /**
    * @class sulphur::engine::Skeleton
    * @brief Stores the data for a Skeleton
    * @author Riko Ophorst
    */
    class Skeleton
    {
    public:
      /** 
      * @brief Constructs a Skeleton based on a pre-processed SkeletonData object.
      * @param[in] skeleton (const sulphur::foundation::SkeletonData&) The pre-processed SkeletonData this
      * should be using.
      */
      Skeleton(const foundation::SkeletonData& skeleton);

      /**
      * @brief Constructs a Skeleton based on a set of given bones and bone names.
      * @param[in] bones (const sulphur::foundation::Vector<Bone>&) A set of bones for the Skeleton.
      * @param[in] bone_names (const sulphur::foundation::UnorderedMap<foundation::String, unsigned int>&) A map of bone names that map to indices in the given set of bones.
      */
      Skeleton(
        const foundation::Vector<Bone>& bones, 
        const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names
      );

      /** @brief Default destructor. */
      ~Skeleton();

      /** @returns A map that links bone names to indices in the Skeleton's set of Bones. */
      const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names();

      /** @returns A set of bones that is owned by the Skeleton. */
      const foundation::Vector<Bone>& bones();

    private:
      /** A vector of Bones stored in the Skeleton. */
      foundation::Vector<Bone> bones_;
      /** Stores a map of bone names (from the bones_ vector) that map to indices in the bones_ array. */
      foundation::UnorderedMap<foundation::String, unsigned int> bone_names_;
    };

    using SkeletonHandle = AssetHandle<Skeleton>; //!< Asset handle to a Skeleton
  }
}
