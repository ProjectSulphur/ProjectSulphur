#include "physics_manifold.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    PhysicsManifold::PhysicsManifold()
    {
    }

    //-------------------------------------------------------------------------
    PhysicsManifold::~PhysicsManifold()
    {
    }

    //-------------------------------------------------------------------------
    PhysicsManifold::PhysicsManifold(
      PhysicsBody* body_a,
      PhysicsBody* body_b)
      : body_a_(body_a),
      body_b_(body_b),
      contact_points_(foundation::Vector<ContactPoint>())
    {
    }

    //-------------------------------------------------------------------------
    const foundation::Vector<PhysicsManifold::ContactPoint>& PhysicsManifold::get_contact_points() const
    {
      return contact_points_;
    }

    //-------------------------------------------------------------------------
    void PhysicsManifold::AddContactPoint(const ContactPoint& contact_point)
    {
      contact_points_.push_back(contact_point);
    }

    //-------------------------------------------------------------------------
    PhysicsBody * PhysicsManifold::body_a()
    {
      return body_a_;
    }

    //-------------------------------------------------------------------------
    PhysicsBody * PhysicsManifold::body_b()
    {
      return body_b_;
    }
  }
}

