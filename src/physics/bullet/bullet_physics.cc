#include "physics/bullet/bullet_physics.h"
#include "physics/bullet/bullet_conversions.h"
#include "physics/bullet/bullet_allocator.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

#include <LinearMath/btVector3.h>

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

      bullet_manifolds = foundation::Vector<btPersistentManifold>();
      dynamics_world_->setInternalTickCallback(InternalTickCallback);
      dynamics_world_->setWorldUserInfo(reinterpret_cast<void*>(this));
      SetGlobalGravity(IPhysics::kDefaultGravity);

      manifolds_ = foundation::Map<PhysicsBody*, foundation::Vector<PhysicsManifold>>();

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

      dynamics_world_->stepSimulation(fixed_time_step, 1, fixed_time_step);
      
      manifolds_.clear();
      //btDispatcher* dispatch = dynamics_world_->getDispatcher();

      for (int i = 0; i < bullet_manifolds.size(); i++)
      {
        btPersistentManifold& manifold = bullet_manifolds[i];

        int numberOfContacts = manifold.getNumContacts();

        PhysicsManifold phys_manifold_a = PhysicsManifold(
          reinterpret_cast<PhysicsBody*>(manifold.getBody0()->getUserPointer()),
          reinterpret_cast<PhysicsBody*>(manifold.getBody1()->getUserPointer()));

        PhysicsManifold phys_manifold_b = PhysicsManifold(
          reinterpret_cast<PhysicsBody*>(manifold.getBody1()->getUserPointer()),
          reinterpret_cast<PhysicsBody*>(manifold.getBody0()->getUserPointer()));

        bool hasContact = false;

        for (int j = 0; j < numberOfContacts; ++j)
        {
          btManifoldPoint& point = manifold.getContactPoint(j);

          if (point.getDistance() < 0.f)
          {
            hasContact = true;

            phys_manifold_a.AddContactPoint(PhysicsManifold::ContactPoint(
              BulletConversions::ToGlm(point.getPositionWorldOnA()),
              BulletConversions::ToGlm(point.m_normalWorldOnB),
              point.getDistance()));

            phys_manifold_b.AddContactPoint(PhysicsManifold::ContactPoint(
              BulletConversions::ToGlm(point.getPositionWorldOnB()),
              BulletConversions::ToGlm(point.m_normalWorldOnB * -1.0f),
              point.getDistance()));
          }
        }

        if (hasContact == true)
        {
          if (manifolds_.find(phys_manifold_a.body_a()) == manifolds_.end())
          {
            foundation::Vector<PhysicsManifold> manifolds = foundation::Vector<PhysicsManifold>();
            manifolds.push_back(phys_manifold_a);
            manifolds_.emplace(phys_manifold_a.body_a(), manifolds);
          }
          else
          {
            manifolds_.at(phys_manifold_a.body_a()).push_back(phys_manifold_a);
          }

          if (manifolds_.find(phys_manifold_b.body_a()) == manifolds_.end())
          {
            foundation::Vector<PhysicsManifold> manifolds = foundation::Vector<PhysicsManifold>();
            manifolds.push_back(phys_manifold_b);
            manifolds_.emplace(phys_manifold_b.body_a(), manifolds);
          }
          else
          {
            manifolds_.at(phys_manifold_b.body_a()).push_back(phys_manifold_b);
          }
        }
      }

      bullet_manifolds.clear();
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
    Manifolds& BulletPhysics::GetManifolds()
    {
      return manifolds_;
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
    void BulletPhysics::InternalTickCallback(btDynamicsWorld * world, btScalar time_step)
    {
      BulletPhysics* that = reinterpret_cast<BulletPhysics*>(reinterpret_cast<btDiscreteDynamicsWorld*>(world)->getWorldUserInfo());
      if (that != nullptr)
      {
        that->SaveManifolds(world, time_step);
      }
    }

    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    void BulletPhysics::SaveManifolds(btDynamicsWorld * world, btScalar time_step)
    {
      btDispatcher* dispatch = world->getDispatcher();
    
      int num_manifolds = dispatch->getNumManifolds();
      btPersistentManifold** manifolds = dispatch->getInternalManifoldPointer();

      for (int i = 0; i < num_manifolds; ++i)
      {
        bullet_manifolds.push_back(*manifolds[i]);
      }
    }
  }
}
