#include "physics/physics_collider.h"
#include "physics/iphysics_shape.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    const float PhysicsCollider::kStaticFrictionThreshold = 0.01f;

    //-------------------------------------------------------------------------
    PhysicsCollider::PhysicsCollider(IPhysicsBody* owner, IPhysicsShape* shape) :
      owner_(owner),
      shape_(shape),
      translation_(0,0,0),
      rotation_(1,0,0,0)
    {
      shape_->AddCollider(this);
    }

    //-------------------------------------------------------------------------
    PhysicsCollider::~PhysicsCollider()
    {
      shape_->RemoveCollider(this);
    }

    //-------------------------------------------------------------------------
    void PhysicsCollider::SetTranslation(const glm::vec3& translation)
    {
      translation_ = translation;
      owner_->UpdateCollider(this);
    }

    //-------------------------------------------------------------------------
    const glm::vec3& PhysicsCollider::translation() const
    {
      return translation_;
    }

    //-------------------------------------------------------------------------
    void PhysicsCollider::SetRotation(const glm::quat& rotation)
    {
      rotation_ = rotation;
      owner_->UpdateCollider(this);
    }

    //-------------------------------------------------------------------------
    const glm::quat& PhysicsCollider::rotation() const
    {
      return rotation_;
    }

    //-------------------------------------------------------------------------
    IPhysicsShape* PhysicsCollider::shape() const
    {
      return shape_;
    }

    //-------------------------------------------------------------------------
    IPhysicsBody* PhysicsCollider::owner() const
    {
      return owner_;
    }
  }
}