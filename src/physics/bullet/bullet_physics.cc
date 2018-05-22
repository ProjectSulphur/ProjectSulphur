#include "physics/bullet/bullet_physics.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"
#include "physics/bullet/bullet_allocator.h"

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

      SetGlobalGravity(IPhysics::kDefaultGravity);

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

      // As of now the physics callbacks happen at the end of a simulation 
      // step. Assuming that only one step is done every Fixed frame call.
      // If this is not the case then the internal tick callback needs to 
      // be used. Because within those multiple steps contacts might occur
      // and be deleted, all within one fixed time step. The reason this 
      // this is currently not implemented is due to the fact that the 
      // callback has to be static. And by design there can be multiple 
      // worlds. Meaning multiple physics worlds (and thus multiple instances
      // of this class where the callback needs to be static)
      // See http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Simulation_Tick_Callbacks

      int numManifolds = dynamics_world_->getDispatcher()->getNumManifolds();
      for (int i = 0; i < numManifolds; i++)
      {
        btPersistentManifold* contactManifold = dynamics_world_->getDispatcher()->getManifoldByIndexInternal(i);
        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; j++)
        {
          btManifoldPoint& pt = contactManifold->getContactPoint(j);
          if (pt.getDistance() < 0.f)
          {
          }
        }
      }
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
        }

        return true;
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
    void BulletPhysics::UpdateBodies()
    {
      dynamics_world_->updateAabbs();
      dynamics_world_->computeOverlappingPairs();
    }

    //-------------------------------------------------------------------------
    BulletPhysics::~BulletPhysics()
    {
    }

    void TickCallback(btDynamicsWorld * world, btScalar timeStep)
    {
    }
  }
}
