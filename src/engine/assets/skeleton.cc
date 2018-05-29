#include "skeleton.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    Bone::Bone() :
      bone_transform(glm::mat4x4(1.0f)),
      parent(INT32_MAX)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Bone::Bone(
      const glm::mat4x4& bone_transform, 
      unsigned int parent, 
      const foundation::Vector<unsigned int>& children
    ) :
      bone_transform(bone_transform),
      parent(parent),
      children(children)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Bone::~Bone()
    {

    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::Skeleton(const foundation::SkeletonData& skeleton)
    {
      size_t size = skeleton.bones.size();
      bones_.resize(size);
      
      for (int i = 0; i < skeleton.bones.size(); ++i)
      {
        bones_[i] = Bone(
          skeleton.bones[i].transform, 
          skeleton.bones[i].parent, 
          skeleton.bones[i].children
        );
      }

      using iterator = foundation::Map<foundation::String, unsigned int>::const_iterator;

      iterator bone_names_begin = skeleton.bone_names.cbegin();
      iterator bone_names_end = skeleton.bone_names.cend();
      iterator it;

      for (it = bone_names_begin; it != bone_names_end; it++)
      {
        bone_names_.emplace(eastl::pair<foundation::String, unsigned int>(it->first, it->second ));
      }
    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::Skeleton(
      const foundation::Vector<Bone>& bones, 
      const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names
    ) :
      bones_(bones),
      bone_names_(bone_names)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Skeleton::~Skeleton()
    {

    }
    
    //------------------------------------------------------------------------------------------------------
    const foundation::UnorderedMap<foundation::String, unsigned int>& Skeleton::bone_names()
    {
      return bone_names_;
    }
    
    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<Bone>& Skeleton::bones()
    {
      return bones_;
    }
  }
}