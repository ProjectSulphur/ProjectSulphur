#include "skeleton.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    Bone::Bone() :
      offset(glm::mat4x4(1.0f))
    {

    }

    //------------------------------------------------------------------------------------------------------
    Bone::Bone(const glm::mat4x4& bone_transform) :
      offset(bone_transform)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Bone::~Bone()
    {

    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::Skeleton(const foundation::SkeletonData& skeleton)
    {
      root_node_ = skeleton.root_node;
      
      nodes_.resize(skeleton.nodes.size());

      for (size_t i = 0; i < skeleton.nodes.size(); i++)
      {
        nodes_[i].name = skeleton.nodes[i].name;
        nodes_[i].transform = skeleton.nodes[i].transform;
        nodes_[i].parent = skeleton.nodes[i].parent;
        nodes_[i].children = skeleton.nodes[i].children;
      }

      size_t size = skeleton.bones.size();
      bones_.resize(size);
      
      for (int i = 0; i < skeleton.bones.size(); ++i)
      {
        bones_[i] = Bone(skeleton.bones[i].transform);
      }

      using iterator = foundation::Map<foundation::String, unsigned int>::const_iterator;

      iterator bone_names_begin = skeleton.bone_names.cbegin();
      iterator bone_names_end = skeleton.bone_names.cend();

      for (iterator it = bone_names_begin; it != bone_names_end; ++it)
      {
        bone_names_.emplace(eastl::pair<foundation::String, unsigned int>(it->first, it->second ));
      }
    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::Skeleton(
      unsigned int root_node,
      const foundation::Vector<SkeletalNode>& nodes,
      const foundation::Vector<Bone>& bones, 
      const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names
    ) :
      root_node_(root_node),
      nodes_(nodes),
      bones_(bones),
      bone_names_(bone_names)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::~Skeleton()
    {

    }

    //------------------------------------------------------------------------------------------------------
    const SkeletalNode& Skeleton::root_node() const
    {
      return nodes_[root_node_];
    }

    //------------------------------------------------------------------------------------------------------
    unsigned int Skeleton::root_node_index() const
    {
      return root_node_;
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<SkeletalNode>& Skeleton::nodes() const
    {
      return nodes_;
    }

    //------------------------------------------------------------------------------------------------------
    const SkeletalNode& Skeleton::node_at(unsigned int index) const
    {
      return nodes_[index];
    }
    
    //------------------------------------------------------------------------------------------------------
    const foundation::UnorderedMap<foundation::String, unsigned int>& Skeleton::bone_names() const
    {
      return bone_names_;
    }
    
    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<Bone>& Skeleton::bones() const
    {
      return bones_;
    }
    
    //------------------------------------------------------------------------------------------------------
    const Bone& Skeleton::bone_at(unsigned int index) const
    {
      return bones_[index];
    }
  }
}