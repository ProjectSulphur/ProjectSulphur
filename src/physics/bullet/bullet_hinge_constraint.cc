#include "physics/bullet/bullet_conversions.h"
#include "bullet_hinge_constraint.h"

#include <foundation/memory/memory.h>
#include "bullet_hinge_constraint.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletHingeConstraint::BulletHingeConstraint(
      btRigidBody* body_a,
      btRigidBody* body_b,
      btDynamicsWorld* world) :
      world_(world),
      body_a_(body_a),
      body_b_(body_b),
      pivot_a_(btVector3()),
      pivot_b_(btVector3()),
      axis_a_(btVector3(0.0f,0.0f,1.0f)),
      axis_b_(btVector3(0.0f,0.0f,1.0f)),
      limit_min_(0.0f),
      limit_max_(0.0f)
    {
      btTransform body_a_trans;
      btTransform body_b_trans;
      body_a->getMotionState()->getWorldTransform(body_a_trans);
      body_b->getMotionState()->getWorldTransform(body_b_trans);

      btTransform fixedPoint = btTransform::getIdentity();
      fixedPoint.setOrigin(
        (body_a_trans.getOrigin() + body_b_trans.getOrigin()) * 0.5f);

      btVector3 axis_a1;
      btVector3 axis_a2;
      btPlaneSpace1(axis_a_, axis_a1, axis_a2);

      btTransform transform_a = body_a_trans.inverse() * fixedPoint;
      btTransform transform_b = body_b_trans.inverse() * fixedPoint;
      pivot_a_ = fixedPoint.getOrigin();

      transform_a.getBasis().setValue(
        axis_a1.getX(), axis_a2.getX(), axis_a_.getX(),
        axis_a1.getY(), axis_a2.getY(), axis_a_.getY(),
        axis_a1.getZ(), axis_a2.getZ(), axis_a_.getZ());

      axis_b_ = body_a_->getCenterOfMassTransform().getBasis() * axis_a_;
      btQuaternion rotation_arc = shortestArcQuat(axis_a_, axis_b_);
      btVector3 axis_b1 = quatRotate(rotation_arc, axis_a1);
      btVector3 axis_b2 = axis_b_.cross(axis_b1);

      pivot_b_ = transform_b.getOrigin();
      transform_b.getBasis().setValue(
        axis_b1.getX(), axis_b2.getX(), axis_b_.getX(),
        axis_b1.getY(), axis_b2.getY(), axis_b_.getY(),
        axis_b1.getZ(), axis_b2.getZ(), axis_b_.getZ());

      bullet_constraint_ = 
        foundation::Memory::Construct<btHingeConstraint>(
          *body_a, 
          *body_b,
          transform_a,
          transform_b);

      world->addConstraint(bullet_constraint_, true);
    }

    //-------------------------------------------------------------------------
    BulletHingeConstraint::~BulletHingeConstraint()
    {
      world_->removeConstraint(bullet_constraint_);
      foundation::Memory::Destruct(bullet_constraint_);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetTorqueLimit(float limit)
    {
      // Sadly not implemented by bullet
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetForceLimit(float limit)
    {
      // So this does the same as a force, or well, an impulse
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetForceLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetTorqueLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameA(const glm::vec3& pivot, const glm::vec3& axis, bool calc_b)
    {
      pivot_a_ = BulletConversions::ToBt(pivot);
      axis_a_ = BulletConversions::ToBt(axis);

      btVector3 axis_a1;
      btVector3 axis_a2;
      btPlaneSpace1(axis_a_, axis_a1, axis_a2);

      btTransform transform_a; 
      btTransform transform_b = bullet_constraint_->getFrameOffsetB();

      transform_a.getOrigin() = pivot_a_;
      transform_a.getBasis().setValue(
        axis_a1.getX(), axis_a2.getX(), axis_a_.getX(),
        axis_a1.getY(), axis_a2.getY(), axis_a_.getY(),
        axis_a1.getZ(), axis_a2.getZ(), axis_a_.getZ());

      if (calc_b == true)
      {
        axis_b_ = body_a_->getCenterOfMassTransform().getBasis() * axis_a_;
        btQuaternion rotationArc = shortestArcQuat(axis_a_, axis_b_);
        btVector3 axis_b1 = quatRotate(rotationArc, axis_a1);
        btVector3 axis_b2 = axis_b_.cross(axis_b1);
        pivot_b_ = (body_a_->getCenterOfMassTransform().getOrigin());

        transform_b.getOrigin() = pivot_b_;
        transform_b.getBasis().setValue(
          axis_b1.getX(), axis_b2.getX(), axis_b_.getX(),
          axis_b1.getY(), axis_b2.getY(), axis_b_.getY(),
          axis_b1.getZ(), axis_b2.getZ(), axis_b_.getZ());
      }

      bullet_constraint_->setFrames(
        transform_a, transform_b);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameB(const glm::vec3& pivot, const glm::vec3& axis)
    {
      pivot_b_ = BulletConversions::ToBt(pivot);
      axis_b_ = BulletConversions::ToBt(axis);

      btVector3 axis_b2, axis_b3;
      btPlaneSpace1(axis_a_, axis_b2, axis_b3);

      btTransform transform;

      transform.getOrigin() = pivot_b_;
      transform.getBasis().setValue(
        axis_b2.getX(), axis_b3.getX(), axis_b_.getX(),
        axis_b2.getY(), axis_b3.getY(), axis_b_.getY(),
        axis_b2.getZ(), axis_b3.getZ(), axis_b_.getZ());

      bullet_constraint_->setFrames(
        bullet_constraint_->getFrameOffsetA(), transform);
    }

    //-------------------------------------------------------------------------
    const glm::vec3 BulletHingeConstraint::GetPivotA() const
    {
      return BulletConversions::ToGlm(pivot_a_);
    }

    //-------------------------------------------------------------------------
    const glm::vec3 BulletHingeConstraint::GetPivotB() const
    {
      return BulletConversions::ToGlm(pivot_b_);
    }

    //-------------------------------------------------------------------------
    const glm::vec3 BulletHingeConstraint::GetAxisA() const
    {
      return BulletConversions::ToGlm(axis_a_);
    }

    //-------------------------------------------------------------------------
    const glm::vec3 BulletHingeConstraint::GetAxisB() const
    {
      return BulletConversions::ToGlm(axis_b_);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetPivotB(const glm::vec3 & pivot)
    {
      pivot_b_ = BulletConversions::ToBt(pivot);
      SetFrameB(pivot, BulletConversions::ToGlm(axis_b_));
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetPivotA(const glm::vec3 & pivot)
    {
      pivot_a_ = BulletConversions::ToBt(pivot);
      SetFrameA(pivot, BulletConversions::ToGlm(axis_a_), false);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetAxisA(const glm::vec3 & axis)
    {
      axis_a_ = BulletConversions::ToBt(axis);
      SetFrameA(BulletConversions::ToGlm(pivot_a_), axis, false);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetAxisB(const glm::vec3 & axis)
    {
      axis_b_ = BulletConversions::ToBt(axis);
      SetFrameB(BulletConversions::ToGlm(pivot_a_), axis);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetMin(float min)
    {
      limit_min_ = min;
      bullet_constraint_->setLimit(limit_min_, limit_max_);
    }
    
    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetMax(float max)
    {
      limit_max_ = max;
      bullet_constraint_->setLimit(limit_min_, limit_max_);
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetMin() const
    {
      return limit_min_;
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetMax() const
    {
      return limit_max_;
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletHingeConstraint::GetBodyA()
    {
      return reinterpret_cast<PhysicsBody*>(body_a_->getUserPointer());
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletHingeConstraint::GetBodyB()
    {
      return reinterpret_cast<PhysicsBody*>(body_b_->getUserPointer());
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameA(const glm::mat4x4 & transform)
    {
      bullet_constraint_->setFrames(
        BulletConversions::ToBt(transform), bullet_constraint_->getFrameOffsetB());
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameB(const glm::mat4x4 & transform)
    {
      bullet_constraint_->setFrames(
        bullet_constraint_->getFrameOffsetA(), BulletConversions::ToBt(transform));
    }

    //-------------------------------------------------------------------------
    const glm::mat4x4 BulletHingeConstraint::GetFrameA() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetA());
    }

    //-------------------------------------------------------------------------
    const glm::mat4x4 BulletHingeConstraint::GetFrameB() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetB());
    }
  }
}
