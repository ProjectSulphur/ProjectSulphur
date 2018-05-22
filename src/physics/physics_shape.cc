#include "physics/physics_shape.h"
#include "physics/platform_physics_body.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    const float PhysicsShape::kDefaultExtents = 0.5f;

    //-------------------------------------------------------------------------
    PhysicsShape::PhysicsShape() :
      attached_body_(nullptr)
    {

    }

    //-------------------------------------------------------------------------
    PhysicsShape::~PhysicsShape()
    {

    }

    //-------------------------------------------------------------------------
    void PhysicsShape::AttachToBody(PhysicsBody* body)
    {
      if (attached_body_ == body)
      {
        return;
      }
      if (attached_body_ != nullptr)
      {
        attached_body_->SetShape(nullptr);
      }

      attached_body_ = body;
      if (body != nullptr)
      {
        body->SetShape(this);
      }
    }

  }
}