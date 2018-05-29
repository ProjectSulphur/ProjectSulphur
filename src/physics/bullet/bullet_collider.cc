#include "physics/bullet/bullet_collider.h"
#include "physics/bullet/bullet_body.h"
#include "physics/bullet/bullet_include.h"

namespace sulphur
{
  namespace physics
  {

    //-------------------------------------------------------------------------
    BulletCollider::BulletCollider(IPhysicsBody* owner, PhysicsShape* shape) :
      PhysicsCollider(owner, shape)
    {
    }

    //-------------------------------------------------------------------------
    void BulletCollider::OnShapeUpdate()
    {
      reinterpret_cast<BulletBody*>(owner_)->UpdateCollider(this);
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetStaticFriction(float friction)
    {
      reinterpret_cast<BulletBody*>(owner_)->set_static_friction(friction);
    }

    //-------------------------------------------------------------------------
    float BulletCollider::GetStaticFriction() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->static_friction();
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetDynamicFriction(float friction)
    {
      reinterpret_cast<BulletBody*>(owner_)->rigid_body()->setFriction(friction);
    }

    //-------------------------------------------------------------------------
    float BulletCollider::GetDynamicFriction() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->rigid_body()->getFriction();
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetRollingFriction(float friction)
    {
      reinterpret_cast<BulletBody*>(owner_)->rigid_body()->setRollingFriction(friction);
    }

    //-------------------------------------------------------------------------
    float BulletCollider::GetRollingFriction() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->rigid_body()->getRollingFriction();
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetFrictionCombineMode(MaterialCombineMode mode)
    {
      reinterpret_cast<BulletBody*>(owner_)->set_friction_combine_mode(mode);
    }

    //-------------------------------------------------------------------------
    PhysicsCollider::MaterialCombineMode BulletCollider::GetFrictionCombineMode() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->friction_combine_mode();
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetRestitution(float restitution)
    {
      reinterpret_cast<BulletBody*>(owner_)->rigid_body()->setRestitution(restitution);
    }

    //-------------------------------------------------------------------------
    float BulletCollider::GetRestitution() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->rigid_body()->getRestitution();
    }

    //-------------------------------------------------------------------------
    void BulletCollider::SetRestitutionCombineMode(MaterialCombineMode mode)
    {
      reinterpret_cast<BulletBody*>(owner_)->set_restitution_combine_mode(mode);
    }

    //-------------------------------------------------------------------------
    PhysicsCollider::MaterialCombineMode BulletCollider::GetRestitutionCombineMode() const
    {
      return reinterpret_cast<BulletBody*>(owner_)->restitution_combine_mode();
    }
  }
}