#include "physics_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"

#include <foundation/utils/frame.h>
#include <foundation/memory/memory.h>

#include <map>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    PhysicsSystem::PhysicsSystem() :
      IServiceSystem("PhysicsSystem")
    {
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      physics_ = &app.platform_physics();
      bodies_ = EntityBodyMap();
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnFixedUpdate()
    {
      SimulateStep(foundation::Frame::fixed_delta_time());
    }

    //-------------------------------------------------------------------------
    physics::PhysicsBody* PhysicsSystem::CreatePhysicsBody(Entity ent)
    {
      physics::PhysicsBody* body = GetPhysicsBody(ent);
      if (body != nullptr)
      {
        PS_LOG(Warning, "Attempted to create a physics body for an entity that already has one.");
        return body;
      }

      TransformComponent trans = ent.Get<TransformComponent>();
      body = physics_->AddPhysicsBody(trans.GetWorldPosition(), trans.GetWorldRotation());
      bodies_.emplace(eastl::pair<Entity, physics::PhysicsBody*>(ent, body));

      return body;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::DestroyPhysicsBody(Entity ent)
    {
      physics_->RemovePhysicsBody(bodies_.at(ent));
      bodies_.erase(ent);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::SimulateStep(float delta_time)
    {
      physics_->SimulateStep(delta_time);
      SyncPhysicsToEngine();
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::SetGlobalGravity(const glm::vec3& gravity)
    {
      physics_->SetGlobalGravity(gravity);
    }

    //-------------------------------------------------------------------------
    glm::vec3 PhysicsSystem::GetGlobalGravity() const
    {
      return physics_->GetGlobalGravity();
    }

    //-------------------------------------------------------------------------
    physics::PhysicsBody* PhysicsSystem::GetPhysicsBody(Entity ent)
    {
      return bodies_.find(ent) != bodies_.end() ? bodies_.at(ent) : nullptr;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::SyncPhysicsToEngine()
    {
      TransformComponent transform;
      physics::PhysicsBody* body;

      for (BodyIterator it = bodies_.begin(); it != bodies_.end(); ++it)
      {
        transform = it->first.Get<TransformComponent>();
        body = it->second;

        if (!body->IsSleeping())
        {
          transform.SetWorldPosition(body->GetTranslation());
          transform.SetWorldRotation(body->GetRotation());
        }
      }
    }
  }
}
