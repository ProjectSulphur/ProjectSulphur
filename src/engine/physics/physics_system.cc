#include "physics_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"
#include "engine/scripting/script_system.h"

#include <foundation/utils/frame.h>
#include <foundation/memory/memory.h>
#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <physics/platform_physics_shape.h>

#include <lua-classes/physics_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    PhysicsSystem* PhysicsSystem::self_ = nullptr;

    //-------------------------------------------------------------------------
    PhysicsSystem::PhysicsSystem() :
      IServiceSystem("PhysicsSystem"),
      script_system_(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      physics_ = &app.platform_physics();
      bodies_ = EntityBodyMap();
      entities_ = BodyEntityMap();
      changes_ = foundation::Vector<EntityBodyMapIt>();
      callbacks_ = CallbackMap();

      const auto fixed_update = [](PhysicsSystem& physics_system) {
        physics_system.SimulateStep(foundation::Frame::fixed_delta_time());
      };

      const auto update = [](PhysicsSystem& physics_system) {
        physics_system.GatherChangedTransforms();
      };

      foundation::Job fixed_update_job = make_job(
        "physicssystem_fixedupdate",
        "fixed_update",
        fixed_update,
        bind_write(*this)
      );

      foundation::Job update_job = make_job(
        "physicssystem_gatherchangedtransforms",
        "render",
        update,
        bind_write(*this)
      );

      fixed_update_job.set_blocker("scriptsystem_fixedupdate");
      update_job.set_blocker("renderer_endframe");
      job_graph.Add(std::move(fixed_update_job));
      job_graph.Add(std::move(update_job));

      script_system_ = &app.GetService<ScriptSystem>();
      PhysicsSystem::self_ = this;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    physics::PhysicsBody* PhysicsSystem::CreatePhysicsBody(Entity entity)
    {
      physics::PhysicsBody* body = GetPhysicsBody(entity);
      if (body != nullptr)
      {
        PS_LOG(Warning, "Attempted to create a physics body for an entity that already has one.");
        return body;
      }

      TransformComponent trans = entity.Get<TransformComponent>();
      body = physics_->AddPhysicsBody(trans.GetWorldPosition(), trans.GetWorldRotation());
      bodies_.emplace(eastl::pair<Entity, physics::PhysicsBody*>(entity, body));
      entities_.emplace(eastl::pair<physics::PhysicsBody*, Entity>(body, entity));

      return body;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::DestroyPhysicsBody(Entity entity)
    {
      //PhysicsBody deletion hotfix:
      physics::IPhysicsBody* body = reinterpret_cast<physics::IPhysicsBody*>(bodies_.at(entity));
      if (body->GetPhysicsBodyType() != physics::PhysicsBodyType::kStatic)
        return;

      for(physics::PhysicsShape* shape : primitive_shapes_)
      {
        for (physics::PhysicsCollider* collider : shape->instances())
        {
          if (collider->owner() == body)
          {
            return; // Body has a collider, so don't destroy body.
          }
        }
      }


      physics_->RemovePhysicsBody(bodies_.at(entity));
      entities_.erase(bodies_.at(entity));
      bodies_.erase(entity);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider* PhysicsSystem::CreatePrimitiveCollider(Entity entity,
                              physics::PhysicsShape::ShapeTypes type)
    {
      physics::PhysicsShape* new_shape;
      switch (type)
      {
      case physics::PhysicsShape::ShapeTypes::kBox:
        new_shape = foundation::Memory::Construct<physics::PhysicsBoxShape>();
        break;
      case physics::PhysicsShape::ShapeTypes::kSphere:
        new_shape = foundation::Memory::Construct<physics::PhysicsSphereShape>();
        break;
      case physics::PhysicsShape::ShapeTypes::kCylinder:
        new_shape = foundation::Memory::Construct<physics::PhysicsCylinderShape>();
        break;
      case physics::PhysicsShape::ShapeTypes::kCapsule:
        new_shape = foundation::Memory::Construct<physics::PhysicsCapsuleShape>();
        break;
      case physics::PhysicsShape::ShapeTypes::kCone:
        new_shape = foundation::Memory::Construct<physics::PhysicsConeShape>();
        break;
      default:
        PS_LOG(Error, "Unable to instantiate primitive collision shape.");
        return nullptr;
      }

      physics::PhysicsBody* owner = GetPhysicsBody(entity);
      if (owner == nullptr)
      {
        owner = CreatePhysicsBody(entity);
      }

      physics::PhysicsCollider* new_collider = owner->AddShape(new_shape);

      primitive_shapes_.push_back(new_shape);

      return new_collider;
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider* PhysicsSystem::CreateMeshCollider(Entity entity, MeshHandle mesh,
                                                                bool convex)
    {
      if (convex == false)
      {
        PS_LOG(Error, "Attempted to create concave mesh collider, which is currently not yet supported.");
        convex = true; // Hah!
      }

      physics::PhysicsShape* shape = nullptr;
      
      if (mesh_shapes_.find(mesh) != mesh_shapes_.end())
      {
        shape = mesh_shapes_[mesh]; // Later on, this will need an isConvex check, or something.
      }
      
      if (shape == nullptr)
      {
        physics::PhysicsConvexShape* new_shape = foundation::Memory::Construct<physics::PhysicsConvexShape>();
        if (mesh == MeshHandle())
        {
          // Handle is invalid, time for fallback unit cube!
          // This is only suitable for convex shapes, for now.
          foundation::Vector<glm::vec3> verts;
          verts.push_back(glm::vec3( 0.5f, 0.5f, 0.5f));
          verts.push_back(glm::vec3( 0.5f, 0.5f,-0.5f));
          verts.push_back(glm::vec3( 0.5f,-0.5f, 0.5f));
          verts.push_back(glm::vec3( 0.5f,-0.5f,-0.5f));
          verts.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
          verts.push_back(glm::vec3(-0.5f, 0.5f,-0.5f));
          verts.push_back(glm::vec3(-0.5f,-0.5f, 0.5f));
          verts.push_back(glm::vec3(-0.5f,-0.5f,-0.5f));
          new_shape->SetVertices(&verts[0], 8);
        }
        else
        {
          new_shape->SetVertices(&mesh->GetVertices()[0], mesh.GetRaw()->GetVertexCount());
        }
        mesh_shapes_[mesh] = new_shape;
        shape = new_shape;
      }

      physics::PhysicsBody* owner = GetPhysicsBody(entity);
      if (owner == nullptr)
      {
        owner = CreatePhysicsBody(entity);
      }

      physics::PhysicsCollider* new_collider = owner->AddShape(shape);

      return new_collider;
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider* PhysicsSystem::CreateCollider(Entity entity,
                              physics::PhysicsShape* shape)
    {
      physics::PhysicsBody* owner = GetPhysicsBody(entity);
      if (owner == nullptr)
      {
        owner = CreatePhysicsBody(entity);
      }

      return owner->AddShape(shape);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::DestroyCollider(physics::PhysicsCollider* collider)
    {
      physics::PhysicsShape* shape = collider->shape();
      physics::IPhysicsBody* owner = collider->owner();
      owner->RemoveShape(collider);

      // If shape has no refs, destroy it
      if (shape->instances().size() == 0)
      {
        if (shape->GetType() == physics::PhysicsShape::ShapeTypes::kConvex ||
            shape->GetType() == physics::PhysicsShape::ShapeTypes::kMesh)
        {
          foundation::Map<MeshHandle, physics::PhysicsShape*>::iterator it = mesh_shapes_.begin();
          while (it != mesh_shapes_.end())
          {
            if (it->second == shape)
            {
              mesh_shapes_.erase(it);
              it = mesh_shapes_.end();
            }
            else
            {
              ++it;
            }
          }
        }
        else
        {
          for (unsigned int i = 0; i < primitive_shapes_.size(); ++i)
          {
            if (shape == primitive_shapes_[i])
            {
              primitive_shapes_.erase(primitive_shapes_.begin() + i);
              return;
            }
          }
        }
      }
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::SimulateStep(float delta_time)
    {
      for (BodyPairIterator it = changes_.begin(); it != changes_.end(); it++)
      {
        TransformComponent trans = (*it)->first.Get<TransformComponent>();
        auto world_pos = trans.GetWorldPosition();
        auto world_rot = trans.GetWorldRotation();
        (*it)->second->ForceWake();
        (*it)->second->SetTranslation(world_pos);
        (*it)->second->SetRotation(world_rot);
      }

      changes_.clear();

      physics_->SimulateStep(delta_time);
      SyncPhysicsToEngine();

      physics::Manifolds& manifolds = physics_->GetManifolds();

      for (physics::Manifolds::iterator it = manifolds.begin();
        it != manifolds.end(); it++)
      {
        if (callbacks_.count(entities_.at(it->first)) == 1)
        {
          Entity ent = entities_.at(it->first);
          if (contact_history_.find(it->first) != contact_history_.end())
          {
            CallCallback(ent, CallbackType::kCollisionStay, it->second);
            CallCallback(ent, CallbackType::kOverlapStay, it->second);
          }
          else
          {
            CallCallback(ent, CallbackType::kCollisionEnter, it->second);
            CallCallback(ent, CallbackType::kOverlapEnter, it->second);
          }
        }
      }

      for (physics::Manifolds::iterator it = contact_history_.begin();
        it != contact_history_.end(); it++)
      {
        if (callbacks_.count(entities_.at(it->first)) != 1)
        {
          Entity ent = entities_.at(it->first);
          if (contact_history_.find(it->first) == contact_history_.end())
          {
            CallCallback(ent, CallbackType::kCollisionLeave, it->second);
            CallCallback(ent, CallbackType::kOverlapLeave, it->second);
          }
        }
      }

      contact_history_.clear();
      contact_history_ = manifolds;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::GatherChangedTransforms()
    {
      // Update moved bodies, maybe cache the transforms?
      for (EntityBodyMapIt pair = bodies_.begin(); pair != bodies_.end(); pair++)
      {
        TransformComponent trans = pair->first.Get<TransformComponent>();

        if (trans.HasChanged() &&
          eastl::find(changes_.begin(), changes_.end(), trans, Finder()) == changes_.end())
        {
          changes_.push_back(pair);
        }
      }
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
    physics::PhysicsBody* PhysicsSystem::GetPhysicsBody(Entity entity)
    {
      return bodies_.find(entity) != bodies_.end() ? bodies_.at(entity) : nullptr;
    }

    void PhysicsSystem::AddCollisionEnterCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kCollisionEnter);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddCollisionStayCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kCollisionStay);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddCollisionLeaveCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kCollisionLeave);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddOverlapEnterCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kOverlapEnter);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddOverlapStayCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kOverlapStay);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddOverlapLeaveCallback(Entity entity, ScriptableCallback callback)
    {
      AddCallback(entity, callback, CallbackType::kOverlapLeave);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::AddCallback(Entity entity, ScriptableCallback callback, CallbackType type)
    {
      if (callbacks_.count(entity) == 0 || callbacks_.at(entity).count(type) == 0)
      {
        EntityCallbackMap ecm = EntityCallbackMap();
        foundation::Vector<ScriptableCallback> callbacks =
          foundation::Vector<ScriptableCallback>();
        callbacks.push_back(callback);
        ecm.emplace(type, callbacks);
        callbacks_.emplace(entity, ecm);
      }
      else
      {
        callbacks_.at(entity).at(type).push_back(callback);
      }
    }

    void PhysicsSystem::RemoveCallback(Entity entity, ScriptableCallback callback,
                                       CallbackType type)
    {
      if (callbacks_.count(entity) != 0)
      {
        if (callbacks_.at(entity).count(type) != 0)
        {
          foundation::Vector<ScriptableCallback>::iterator callback_it = 
            eastl::find(
              callbacks_.at(entity).at(type).begin(),
              callbacks_.at(entity).at(type).end(),
              callback);

          if (callback_it != callbacks_.at(entity).at(type).end())
          {
            callbacks_.at(entity).at(type).erase(callback_it);
          }
          
          if (callbacks_.at(entity).at(type).size() == 0)
          {
            callbacks_.at(entity).erase(type);
          }
        }
        if (callbacks_.at(entity).size() == 0)
        {
          callbacks_.erase(entity);
        }
      }
    }

    // Lol this is so borken, pls fix
    inline void PhysicsSystem::CallCallback(Entity entity,
      CallbackType type,
      foundation::Vector<physics::PhysicsManifold>& manifolds)
    {
      if (callbacks_.at(entity).count(type) == 1)
      {
        for (foundation::Vector<ScriptableCallback>::iterator script_it =
          callbacks_.at(entity).at(type).begin();
          script_it != callbacks_.at(entity).at(type).end(); ++script_it)
        {
          for (foundation::Vector<physics::PhysicsManifold>::iterator man_it =
            manifolds.begin(); man_it != manifolds.end(); ++man_it)
          {
            script_it->Call({/*man_it*/ }); // todo(benjamin) arguments
          }
        }
      }
    }

    //-------------------------------------------------------------------------
    ScriptHandle PhysicsSystem::RayCast(const foundation::Ray& ray, float distance)
    {
      physics::RaycastHitInfo hit_info;
      
      self_->physics_->Raycast(ray, &hit_info, distance);

      ScriptTableHandle raycast_info_handle = ScriptUtils::CreateTable(self_->script_system_->script_state());
      raycast_info_handle->Insert("hit", hit_info.hit);
      raycast_info_handle->Insert("point", hit_info.point);
      raycast_info_handle->Insert("normal", hit_info.normal);
      raycast_info_handle->Insert("distance", hit_info.distance);
      raycast_info_handle->Insert("ray", hit_info.ray);

      return eastl::static_pointer_cast<ScriptableTable, ScriptableValue>(raycast_info_handle);
    }

    //-------------------------------------------------------------------------
    ScriptHandle PhysicsSystem::RayCastAll(const foundation::Ray & ray, float distance)
    {
      bool hit;
      physics::RaycastHits hits = self_->physics_->RaycastAll(ray, &hit, distance);
      ScriptTableHandle raycasthits_info_handle = ScriptUtils::CreateTable(self_->script_system_->script_state());

      for (int i = 0; i < hits.size(); ++i)
      {
        ScriptTableHandle raycast_info_handle = ScriptUtils::CreateTable(self_->script_system_->script_state());
        raycast_info_handle->Insert("hit",      hits[i].hit);
        raycast_info_handle->Insert("point",    hits[i].point);
        raycast_info_handle->Insert("normal",   hits[i].normal);
        raycast_info_handle->Insert("distance", hits[i].distance);
        raycast_info_handle->Insert("ray",      hits[i].ray);
        raycasthits_info_handle->Insert(i, raycast_info_handle);
      }

      return raycasthits_info_handle;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveCollisionEnterCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kCollisionEnter);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveCollisionStayCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kCollisionStay);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveCollisionLeaveCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kCollisionLeave);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveOverlapEnterCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kOverlapEnter);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveOverlapStayCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kOverlapStay);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::RemoveOverlapLeaveCallback(Entity entity, ScriptableCallback callback)
    {
      RemoveCallback(entity, callback, CallbackType::kOverlapLeave);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::SyncPhysicsToEngine()
    {
      TransformComponent transform;
      physics::PhysicsBody* body;

      for (EntityBodyMapIt it = bodies_.begin(); it != bodies_.end(); ++it)
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
