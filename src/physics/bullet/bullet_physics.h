#pragma once

#include "physics/iphysics.h"
#include "physics/bullet/bullet_body.h"
#include "physics/physics_manifold.h"
#include "physics/bullet/bullet_fixed_constraint.h"

#include <foundation/containers/map.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>
#include "physics/bullet/bullet_include.h"

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
      * @see sulphur::physics::IPhysics::GetManifolds()
      */
      PhysicsManifold* GetManifolds() override;

      /**
      * @see sulpur::physics::IPhysics::GetManifoldsSize()
      */
      size_t GetManifoldsSize() override;

      /**
      * @see sulphur::physics::IPhysics::SubscribeCallback
      */
      void SubscribeCallback(PhysicsBody* physics_body) override;

      /**
      * @see sulphur::physics::IPhysics::UnSubscribeCallback
      */
      void UnSubscribeCallback(PhysicsBody* physics_body) override;

      /**
      * @brief Updates all physics bodies internally without simulating a frame
      */
      void UpdateBodies();

      /**
      * @brief Default destructor
      */
      virtual ~BulletPhysics();

      /**
      * @brief Callback for bullet to call when an internal update happens
      */
      static void InternalTickCallback(btDynamicsWorld* world, btScalar time_step);

      /**
      * @see sulphur::physics::IPhysics::AddConstraint
      */
      IPhysicsConstraint* AddConstraint(PhysicsBody* owner,
                          IPhysicsConstraint::ConstraintTypes type) override;

      /**
      * @see sulphur::physics::IPhysics::RemoveConstraint
      */
      void RemoveConstraint(IPhysicsConstraint* constraint) override;

    private:
      PhysicsBody** callback_subs_; //!< Buffer of physics bodies that are subscribed for callbacks
      size_t callback_subs_size_; //!< Size of the callback subs buffer

      PhysicsManifold* manifolds_; //!< Buffer of converted manifolds 
      size_t manifolds_size_; //!< Size of the manifolds collection

      btDiscreteDynamicsWorld* dynamics_world_; //!< The Bullet physics world
      btDbvtBroadphase* broad_phase_; //!< The Bullet broad phase search
      btSequentialImpulseConstraintSolver* constraint_solver_; //!< The Bullet constraint solver
      btCollisionDispatcher* collision_dispatcher_; //!< The Bullet collision dispatcher
      btDefaultCollisionConfiguration* collision_config_; //!< The Bullet collision configuration

      /**
      * @brief Saves the manifolds that were made during the last internal update
      * @param[in] world (btDynamicsWorld*) the world where the callback originates \
      */
      void SaveManifolds(btDynamicsWorld* world); 
    };
  }
}
