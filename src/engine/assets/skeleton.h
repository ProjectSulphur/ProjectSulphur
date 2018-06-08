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
    * @struct sulphur::engine::SkeletalNode
    * @brief A SkeletalNode represents any node that lives in the original scene from which the skeleton was originally loaded.
    * @remarks The SkeletalNode is necessary because it makes sure that there is a proper transform-chain between bones & nodes.
    * @remarks The SkeletalNode is also necessary because it maintains the parent-child hierarchy from the original scene.
    * @author Riko Ophorst
    */
    struct SkeletalNode
    {
      foundation::String name;                    //!< The name of the node as it was in the original scene.
      glm::mat4 transform;                        //!< The transform of the node as it was in the original scene.
      unsigned int parent;                        //!< The index of the parent node that resides at Skeleton::nodes_.
      foundation::Vector<unsigned int> children;  //!< The indices of the child nodes that reside at Skeleton::nodes_. 
    };

    /**
    * @struct sulphur::engine::Bone
    * @brief Stores information about a Bone in a Skeleton.
    * @author Riko Ophorst
    */
    struct Bone
    {
      /** @brief Default constructor. */
      Bone();

      /** 
      * @brief Constructs a bone with some data.
      * @param[in] bone_transform (const glm::mat4x4&) The transformation on the Bone in bind-pose stored in the owning Skeleton.
      */
      Bone(const glm::mat4x4& bone_transform);

      /** @brief Default destructor. */
      ~Bone();

      /** The transform of the Bone in local Bone space in bind-pose. */
      glm::mat4x4 offset;
    };

    /**
    * @class sulphur::engine::Skeleton
    * @brief Stores the data for a Skeleton.
    * @author Riko Ophorst
    */
    class Skeleton
    {
    public:
      /** 
      * @brief Constructs a Skeleton based on a pre-processed SkeletonData object.
      * @param[in] skeleton (const sulphur::foundation::SkeletonData&) The pre-processed SkeletonData this should be constructed with.
      */
      Skeleton(const foundation::SkeletonData& skeleton);

      /**
      * @brief Constructs a Skeleton based on a set of given bones and bone names.
      * @param[in] root_node (unsigned int) The index into the nodes array of the root node.
      * @param[in] nodes (const sulphur::foundation::Vector<sulphur::engine::SkeletalNode::SkeletalNode>&) An array of all SkeletalNode objects that the skeleton consists of.
      * @param[in] bones (const sulphur::foundation::Vector<sulphur::engine::SkeletalNode::Bone>&) A set of bones for the Skeleton.
      * @param[in] bone_names (const sulphur::foundation::UnorderedMap<sulphur::foundation::String, unsigned int>&) A map of bone names that map to indices in the given set of bones.
      */
      Skeleton(
        unsigned int root_node,
        const foundation::Vector<SkeletalNode>& nodes,
        const foundation::Vector<Bone>& bones, 
        const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names
      );

      /** @brief Default destructor. */
      ~Skeleton();

      /** @returns (const sulphur::engine::SkeletalNode&) The root node of the skeleton. */
      const SkeletalNode& root_node() const;

      /** @returns (unsigned int) The index of the root node of the skeleton. */
      unsigned int root_node_index() const;

      /** @returns (const sulphur::foundation::Vector<sulphur::engine::SkeletalNode>&) The array of nodes that make up the skeleton's node hierarchy. */
      const foundation::Vector<SkeletalNode>& nodes() const;

      /**
      * @param[in] index (unsigned int) The index of the skeletal node you wish to retrieve.
      * @returns (const sulphur::engine::SkeletalNode&) The skeletal node at a given index.
      * @remarks This can fail if the index is larger than the number of skeletal nodes in the skeleton.
      */
      const SkeletalNode& node_at(unsigned int index) const;

      /** @returns (const sulphur::foundation::UnorderedMap<sulphur::foundation::String, unsigned int>& A map that links bone names to indices in the Skeleton's set of Bones. */
      const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names() const;

      /** @returns (const sulphur::foundation::Vector<sulphur::engine::Bone>& A set of bones that is owned by the Skeleton. */
      const foundation::Vector<Bone>& bones() const;

      /**
      * @param[in] index (unsigned int) The index of the bone you wish to retrieve.
      * @returns (const sulphur::engine::Bone&) The bone at a given index.
      * @remarks This can fail if the index is larger than the number of bones in the skeleton.
      */
      const Bone& bone_at(unsigned int index) const;

    private:
      /** The index of the root skeletal node of the skeleton. Slots into the nodes_ array. */
      unsigned int root_node_;

      /** The array of skeletal nodes that this skeleton consists of. */
      foundation::Vector<SkeletalNode> nodes_;

      /** A vector of Bones stored in the Skeleton. */
      foundation::Vector<Bone> bones_;

      /** Stores a map of bone names that map to indices in the bones_ array. */
      foundation::UnorderedMap<foundation::String, unsigned int> bone_names_;
    };

    using SkeletonHandle = AssetHandle<Skeleton>; //!< Asset handle to a Skeleton
  }
}
