#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    const float IPhysicsShape::kDefaultExtents = 0.5f;

    //-------------------------------------------------------------------------
    const foundation::Vector<PhysicsCollider*>& IPhysicsShape::instances() const
    {
      return instances_;
    }

    //-------------------------------------------------------------------------
    void IPhysicsShape::UpdateColliders()
    {
      for (PhysicsCollider* collider : instances_)
      {
        collider->OnShapeUpdate();
      }
    }

    //-------------------------------------------------------------------------
    void IPhysicsShape::AddCollider(PhysicsCollider* instance)
    {
      instances_.push_back(instance);
    }

    //-------------------------------------------------------------------------
    void IPhysicsShape::RemoveCollider(PhysicsCollider* instance)
    {
      for (unsigned int i = 0; i < instances_.size(); ++i)
      {
        if (instance == instances_[i])
        {
          instances_.erase(instances_.begin() + i);
          return;
        }
      }
    }
  }
}
