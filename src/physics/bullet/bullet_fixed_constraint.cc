#include "physics/bullet/bullet_conversions.h"
#include "bullet_fixed_constraint.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletFixedConstraint::BulletFixedConstraint(
      btRigidBody* body_a,
      btRigidBody* body_b,
      btDynamicsWorld* world) :
      body_a_(body_a),
      body_b_(body_b),
      world_(world),
      frame_a_(btTransform()),
      frame_b_(btTransform())
    {
      btTransform body_a_trans;
      btTransform body_b_trans;
      body_a_->getMotionState()->getWorldTransform(body_a_trans);
      body_b_->getMotionState()->getWorldTransform(body_b_trans);

      btTransform fixedPoint = btTransform::getIdentity();
      fixedPoint.setOrigin(
        (body_a_trans.getOrigin() + body_b_trans.getOrigin()) * 0.5f);

      frame_a_ = body_a_trans.inverse() * fixedPoint;
      frame_b_ = body_b_trans.inverse() * fixedPoint;

      bullet_constraint_ = 
        foundation::Memory::Construct<btFixedConstraint>(
          *body_a, 
          *body_b,
          frame_a_,
          frame_b_);

      world->addConstraint(bullet_constraint_, true);
    }

    //-------------------------------------------------------------------------
    BulletFixedConstraint::~BulletFixedConstraint()
    {
      world_->removeConstraint(bullet_constraint_);
      foundation::Memory::Destruct(bullet_constraint_);
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetTorqueLimit(float limit)
    {
      // Sadly not implemented by bullet
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetForceLimit(float limit)
    {
      // So this does the same as a force, or well, an impulse
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    float BulletFixedConstraint::GetForceLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    float BulletFixedConstraint::GetTorqueLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletFixedConstraint::GetBodyA()
    {
      return reinterpret_cast<PhysicsBody*>(body_a_->getUserPointer());
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletFixedConstraint::GetBodyB()
    {
      return reinterpret_cast<PhysicsBody*>(body_b_->getUserPointer());
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetFrameA(const glm::mat4x4 & transform)
    {
      bullet_constraint_->setFrames(
        BulletConversions::ToBt(transform), bullet_constraint_->getFrameOffsetB());
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetFrameB(const glm::mat4x4 & transform)
    {
      bullet_constraint_->setFrames(
        bullet_constraint_->getFrameOffsetA(), BulletConversions::ToBt(transform));
    }

    //-------------------------------------------------------------------------
    const glm::mat4x4 BulletFixedConstraint::GetFrameA() const
    {
      return BulletConversions::ToGlm(frame_a_);
    }

    //-------------------------------------------------------------------------
    const glm::mat4x4 BulletFixedConstraint::GetFrameB() const
    {
      return BulletConversions::ToGlm(frame_b_);
    }
  }
}
