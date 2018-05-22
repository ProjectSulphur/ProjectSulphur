#pragma once

#include "physics/iphysics.h"
#include "physics/bullet/bullet_body.h"

#include <foundation/containers/map.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

class btDiscreteDynamicsWorld;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;

namespace sulphur 
{
  namespace physics //!< Physics namespace
  {
    class BulletBody;

    /**
    * @class sulphur::physics::BulletPhysics : public sulphur::physics::IPhysics
    * @brief The low level implementation for the Bullet library
    * @author Daniel Konings, Benjamin Waanders
    */
    class BulletPhysics : public IPhysics
    {

    public:

      /**
      * @brief Delete copy constructor
      */
      BulletPhysics(const BulletPhysics& other) = delete;


      /**
      * @brief Protected constructor
      */
      BulletPhysics();

      /**
      * @see sulphur::physics::IPhysics::Initialize()
      */
      bool Initialize() override;

      /**
      * @see sulphur::physics::IPhysics::Destroy()
      */
      void Destroy() override;

      /**
      * @see sulphur::physics::IPhysics::AddPhysicsBody()
      */
      BulletBody* AddPhysicsBody(
        const glm::vec3& translation, 
        const glm::quat& rotation) override;

      /**
      * @see sulphur::physics::IPhysics::RemovePhysicsBody()
      */
      void RemovePhysicsBody(PhysicsBody* body) override;

      /**
      * @see sulphur::physics::IPhysics::SetGlobalGravity()
      */
      void SetGlobalGravity(const glm::vec3& gravity) override;

      /**
      * @see sulphur::physics::IPhysics::GetGlobalGravity()
      */
      glm::vec3 GetGlobalGravity() const override;

      /**
      * @see sulphur::physics::IPhysics::SimulateStep()
      */
      void SimulateStep(float fixed_time_step) override;

      /**
      * @see sulphur::physics::IPhysics::Raycast()
      */
      bool Raycast(
        const foundation::Ray& ray, 
        RaycastHitInfo* out, 
        float max_distance) override;

      /**
      * @see sulphur::physics::IPhysics::RaycastAll()
      */
      RaycastHits RaycastAll(
        const foundation::Ray& ray, 
        bool* hit, 
        float max_distance) override;

      /**
      * @brief Updates all physics bodies internally without simulating a frame
      */
      void UpdateBodies();
      
      /**
      * @brief Default destructor
      */
      virtual ~BulletPhysics();

    private:

      btDiscreteDynamicsWorld* dynamics_world_; //!< The Bullet physics world
      btDbvtBroadphase* broad_phase_; //!< The Bullet broad phase search
      btSequentialImpulseConstraintSolver* constraint_solver_; //!< The Bullet constraint solver
      btCollisionDispatcher* collision_dispatcher_; //!< The Bullet collision dispatcher
      btDefaultCollisionConfiguration* collision_config_; //!< The Bullet collision configuration
    };
  }
}