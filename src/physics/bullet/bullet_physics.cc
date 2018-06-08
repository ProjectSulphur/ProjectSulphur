#include "physics/bullet/bullet_physics.h"
#include "physics/bullet/bullet_conversions.h"
#include "physics/bullet/bullet_allocator.h"
#include "physics/platform_physics_constraint.h"

#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletPhysics::BulletPhysics() :
      dynamics_world_(nullptr),
      broad_phase_(nullptr),
      constraint_solver_(nullptr),
      collision_dispatcher_(nullptr),
      collision_config_(nullptr)
    {

    }

    //-------------------------------------------------------------------------
    bool BulletPhysics::Initialize()
    {
      if (dynamics_world_ != nullptr)
      {
        return true;
      }

      btAlignedAllocSetCustom(BulletAllocator::Allocate, BulletAllocator::Deallocate);

      collision_config_ =
        foundation::Memory::Construct<btDefaultCollisionConfiguration>();

      collision_dispatcher_ =
        foundation::Memory::Construct<btCollisionDispatcher>(collision_config_);

      broad_phase_ = foundation::Memory::Construct<btDbvtBroadphase>();

      constraint_solver_ = foundation::Memory::Construct<btSequentialImpulseConstraintSolver>();

      dynamics_world_ = foundation::Memory::Construct<btDiscreteDynamicsWorld>(
        collision_dispatcher_,
        broad_phase_,
        constraint_solver_,
        collision_config_);

      dynamics_world_->setInternalTickCallback(InternalTickCallback);
      dynamics_world_->setWorldUserInfo(reinterpret_cast<void*>(this));
      SetGlobalGravity(IPhysics::kDefaultGravity);

      manifolds_ = static_cast<PhysicsManifold*>(
        foundation::Memory::Allocate(sizeof(PhysicsManifold) * kManifoldBufferLimit));
      callback_subs_ = static_cast<PhysicsBody**>(
        foundation::Memory::Allocate(sizeof(PhysicsBody) * kManifoldBufferLimit));

      callback_subs_size_ = 0;
      manifolds_size_ = 0;

      return true;
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::Destroy()
    {
      if (dynamics_world_ != nullptr)
      {
        foundation::Memory::Destruct(dynamics_world_);
        dynamics_world_ = nullptr;

        foundation::Memory::Destruct(broad_phase_);
        broad_phase_ = nullptr;

        foundation::Memory::Destruct(constraint_solver_);
        constraint_solver_ = nullptr;

        foundation::Memory::Destruct(collision_dispatcher_);
        collision_dispatcher_ = nullptr;

        foundation::Memory::Destruct(collision_config_);
        collision_config_ = nullptr;

        foundation::Memory::Destruct(manifolds_);
        manifolds_ = nullptr;

        foundation::Memory::Destruct(callback_subs_);
        callback_subs_ = nullptr;
      }
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletPhysics::AddPhysicsBody(
      const glm::vec3& translation,
      const glm::quat& rotation)
    {
      return foundation::Memory::Construct<PhysicsBody>(
        dynamics_world_,
        translation,
        rotation);
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::RemovePhysicsBody(PhysicsBody* body)
    {
      foundation::Memory::Destruct<PhysicsBody>(body);
    }

    //-------------------------------------------------------------------------
    IPhysicsConstraint* BulletPhysics::AddConstraint(PhysicsBody* owner,
      IPhysicsConstraint::ConstraintTypes type)
    {
      switch (type)
      {
      case IPhysicsConstraint::ConstraintTypes::kFixed:
        return foundation::Memory::Construct<FixedConstraint>(owner, dynamics_world_);
        break;
      case IPhysicsConstraint::ConstraintTypes::kHinge:
        return foundation::Memory::Construct<HingeConstraint>(owner, dynamics_world_);
        break;
      default:
        PS_LOG(Error, "Attempted to instantiate constraint of unknown type in BulletPhysics::AddConstraint.");
        return nullptr;
      }
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::RemoveConstraint(IPhysicsConstraint* constraint)
    {
      foundation::Memory::Destruct(constraint);
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::SetGlobalGravity(const glm::vec3& gravity)
    {
      if (dynamics_world_ == nullptr)
      {
        return;
      }

      dynamics_world_->setGravity(BulletConversions::ToBt(gravity));
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletPhysics::GetGlobalGravity() const
    {
      if (dynamics_world_ == nullptr)
      {
        return glm::vec3{ 0.0f, 0.0f, 0.0f };
      }

      btVector3 v = dynamics_world_->getGravity();
      return BulletConversions::ToGlm(v);
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::SimulateStep(float fixed_time_step)
    {
      if (dynamics_world_ == nullptr)
      {
        return;
      }
      
      // Reset the size of manifolds, this gets populated by the InternalTickCallback()
      manifolds_size_ = 0;

      dynamics_world_->stepSimulation(fixed_time_step, 1, fixed_time_step);
    }

    //-------------------------------------------------------------------------
    bool BulletPhysics::Raycast(
      const foundation::Ray& ray,
      RaycastHitInfo* out,
      float max_distance)
    {
      UpdateBodies();

      btVector3 start = BulletConversions::ToBt(ray.origin);
      btVector3 end = start + BulletConversions::ToBt(ray.direction) * max_distance;

      btCollisionWorld::ClosestRayResultCallback ray_callback(start, end);

      dynamics_world_->rayTest(start, end, ray_callback);

      if (ray_callback.hasHit() == true)
      {
        if (out != nullptr)
        {
          out->ray = ray;
          out->point = BulletConversions::ToGlm(ray_callback.m_hitPointWorld);
          out->normal = BulletConversions::ToGlm(ray_callback.m_hitNormalWorld);
          out->distance = ray_callback.m_closestHitFraction;
          out->hit = true;
        }

        return true;
      }

      if (out != nullptr)
      {
        out->ray = ray;
        out->point = glm::vec3();
        out->normal = glm::vec3();
        out->distance = 0.0f;
        out->hit = false;
      }

      return false;
    }

    //-------------------------------------------------------------------------
    RaycastHits BulletPhysics::RaycastAll(
      const foundation::Ray& ray,
      bool* hit,
      float max_distance)
    {
      UpdateBodies();

      btVector3 start = BulletConversions::ToBt(ray.origin);
      btVector3 end = start + BulletConversions::ToBt(ray.direction) * max_distance;

      btCollisionWorld::AllHitsRayResultCallback ray_callback(start, end);

      dynamics_world_->rayTest(start, end, ray_callback);

      foundation::Vector<RaycastHitInfo> hits;

      if (ray_callback.hasHit() == true)
      {
        if (hit != nullptr)
        {
          *hit = true;
        }

        const btAlignedObjectArray<btVector3>& points = ray_callback.m_hitPointWorld;
        const btAlignedObjectArray<btVector3>& normals = ray_callback.m_hitNormalWorld;
        const btAlignedObjectArray<btScalar>& distances = ray_callback.m_hitFractions;

        RaycastHitInfo info;
        size_t size = points.size();
        hits.resize(size);

        for (unsigned int i = 0; i < static_cast<unsigned int>(size); ++i)
        {
          info.ray = ray;
          info.point = BulletConversions::ToGlm(points.at(i));
          info.normal = BulletConversions::ToGlm(normals.at(i));
          info.distance = distances.at(i);

          hits.at(i) = info;
        }
      }

      return hits;
    }

    //-------------------------------------------------------------------------
    PhysicsManifold* BulletPhysics::GetManifolds()
    {
      return manifolds_;
    }

    //-------------------------------------------------------------------------
    size_t BulletPhysics::GetManifoldsSize()
    {
      return manifolds_size_;
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::SubscribeCallback(PhysicsBody * physics_body)
    {
      for (int i = 0; i < callback_subs_size_; ++i)
      {
        if (callback_subs_[i] == physics_body)
        {
          // Already subbed.
          return;
        }
      }
      
      if (callback_subs_size_ == kManifoldBufferLimit)
      {
        PS_LOG(Warning, "We have reached the sub limit!");
        return;
      }
      callback_subs_[callback_subs_size_++] = physics_body;
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::UnSubscribeCallback(PhysicsBody * physics_body)
    {
      for (int i = 0; i < callback_subs_size_; ++i)
      {
        if (callback_subs_[i] == physics_body)
        {
          for (int j = i; j < callback_subs_size_ - 1; ++j)
          {
            callback_subs_[j] = callback_subs_[j + 1];
          }

          callback_subs_size_--;
          break;
        }
      }
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::UpdateBodies()
    {
      dynamics_world_->updateAabbs();
      dynamics_world_->computeOverlappingPairs();
    }

    //-------------------------------------------------------------------------
    BulletPhysics::~BulletPhysics()
    {
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::InternalTickCallback(btDynamicsWorld* world, btScalar time_step)
    {
      // That is here so that we know to which physics world the callback belongs to.
      // In case we actually do as we designed and have multiple (physics) worlds
      BulletPhysics* that = reinterpret_cast<BulletPhysics*>(world->getWorldUserInfo());

      if (that != nullptr)
      {
        that->SaveManifolds(world);
      }
    }

    //-------------------------------------------------------------------------
    void BulletPhysics::SaveManifolds(btDynamicsWorld* world)
    {
      btDispatcher* dispatch = world->getDispatcher();

      int num_manifolds = dispatch->getNumManifolds();
      btPersistentManifold** manifolds = dispatch->getInternalManifoldPointer();

      for (int i = 0; i < num_manifolds; ++i)
      {
        bool contact = false;

        for (int j = 0; j < manifolds[i]->getNumContacts(); ++j)
        {
          btManifoldPoint& point = manifolds[i]->getContactPoint(j);

          if (point.getDistance() < 0.f)
          {
            contact = true;
            break;
          }
        }

        if (contact == true)
        {
          for (int j = 0; j < callback_subs_size_; ++j)
          {
            // Check if A or B are listed
            if (callback_subs_[j] ==
              static_cast<PhysicsBody*>(manifolds[i]->getBody0()->getUserPointer()))
            {
              PhysicsManifold* phys_manifold = new (
                manifolds_ + manifolds_size_++)  PhysicsManifold(
                reinterpret_cast<PhysicsBody*>(manifolds[i]->getBody0()->getUserPointer()),
                reinterpret_cast<PhysicsBody*>(manifolds[i]->getBody1()->getUserPointer()));

              for (int j = 0; j < manifolds[i]->getNumContacts(); ++j)
              {
                btManifoldPoint& point = manifolds[i]->getContactPoint(j);

                if (point.getDistance() < 0.f)
                {
                  phys_manifold->AddContactPoint(PhysicsManifold::ContactPoint(
                    BulletConversions::ToGlm(point.getPositionWorldOnA()),
                    BulletConversions::ToGlm(point.m_normalWorldOnB),
                    point.getDistance()));
                }
              }
            }
            else if (callback_subs_[j] ==
              static_cast<PhysicsBody*>(manifolds[i]->getBody1()->getUserPointer()))
            {
              PhysicsManifold* phys_manifold = new (
                manifolds_ + manifolds_size_++)  PhysicsManifold(
                reinterpret_cast<PhysicsBody*>(manifolds[i]->getBody1()->getUserPointer()),
                reinterpret_cast<PhysicsBody*>(manifolds[i]->getBody0()->getUserPointer()));

              for (int j = 0; j < manifolds[i]->getNumContacts(); ++j)
              {
                btManifoldPoint& point = manifolds[i]->getContactPoint(j);

                if (point.getDistance() < 0.f)
                {
                  phys_manifold->AddContactPoint(PhysicsManifold::ContactPoint(
                    BulletConversions::ToGlm(point.getPositionWorldOnB()),
                    BulletConversions::ToGlm(point.m_normalWorldOnB * -1.0f),
                    point.getDistance()));
                }
              }
            }
          }
        }
      }
    }
  }
}
