#include "physics/physics_shape.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    const float PhysicsShape::kDefaultExtents = 0.5f;

    //-------------------------------------------------------------------------
    const foundation::Vector<PhysicsCollider*>& PhysicsShape::instances() const
    {
      return instances_;
    }

    //-------------------------------------------------------------------------
    void PhysicsShape::UpdateColliders()
    {
      for (PhysicsCollider* collider : instances_)
      {
        collider->OnShapeUpdate();
      }
    }

    //-------------------------------------------------------------------------
    void PhysicsShape::AddCollider(PhysicsCollider* instance)
    {
      instances_.push_back(instance);
    }

    //-------------------------------------------------------------------------
    void PhysicsShape::RemoveCollider(PhysicsCollider* instance)
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
