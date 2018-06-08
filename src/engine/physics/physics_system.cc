#include "physics_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"
#include "engine/scripting/script_system.h"
#include "engine/assets/asset_system.h"

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
    PhysicsSystem::~PhysicsSystem()
    {
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      physics_ = &app.platform_physics();
      bodies_ = EntityBodyMap();
      entities_ = BodyEntityMap();
      changes_ = foundation::Vector<EntityBodyMapIt>();
      contact_history_ = static_cast<physics::PhysicsManifold*>(
        sulphur::foundation::Memory::Allocate(
          sizeof(physics::PhysicsManifold) * physics::IPhysics::kManifoldBufferLimit));
      contact_history_size_ = 0;

      fallback_mesh_ = AssetSystem::Instance().AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateCube()), "Physics fallback mesh");

      const auto fixed_update = [](PhysicsSystem& physics_system) {
        physics_system.SimulateStep(foundation::Frame::fixed_delta_time());
      };

      foundation::Job fixed_update_job = make_job(
        "physicssystem_fixedupdate",
        "fixed_update",
        fixed_update,
        bind_write(*this)
      );

      fixed_update_job.set_blocker("scriptsystem_fixedupdate");
      job_graph.Add(std::move(fixed_update_job));

      script_system_ = &app.GetService<ScriptSystem>();
      PhysicsSystem::self_ = this;
    
      callbacks_.bodies_ = static_cast<physics::PhysicsBody**>(
        sulphur::foundation::Memory::Allocate(
          sizeof(physics::PhysicsBody*) * physics::IPhysics::kManifoldBufferLimit));
      callbacks_.types_ = static_cast<CallbackType*>(
        sulphur::foundation::Memory::Allocate(
          sizeof(CallbackType) * physics::IPhysics::kManifoldBufferLimit));
      callbacks_.callbacks_ = static_cast<ScriptableCallback*>(
        sulphur::foundation::Memory::Allocate(
          sizeof(ScriptableCallback) * physics::IPhysics::kManifoldBufferLimit));
      callbacks_.size_ = 0;
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::OnTerminate()
    {
      sulphur::foundation::Memory::Deallocate(contact_history_);

      sulphur::foundation::Memory::Deallocate(callbacks_.bodies_);
      callbacks_.bodies_ = nullptr;

      sulphur::foundation::Memory::Deallocate(callbacks_.types_);
      callbacks_.types_ = nullptr;

      sulphur::foundation::Memory::Deallocate(callbacks_.callbacks_);
      callbacks_.callbacks_ = nullptr;

      callbacks_.size_ = 0;
      fallback_mesh_.Release();
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

      for(physics::IPhysicsShape* shape : primitive_shapes_)
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
    physics::IPhysicsConstraint* PhysicsSystem::CreateConstraint(Entity owner,
      physics::IPhysicsConstraint::ConstraintTypes type)
    {
      return physics_->AddConstraint(bodies_.at(owner), type);
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::DestroyConstraint(physics::IPhysicsConstraint* constraint)
    {
      physics_->RemoveConstraint(constraint);
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider* PhysicsSystem::CreatePrimitiveCollider(Entity entity,
                              physics::IPhysicsShape::ShapeTypes type)
    {
      physics::IPhysicsShape* new_shape;
      switch (type)
      {
      case physics::IPhysicsShape::ShapeTypes::kBox:
        new_shape = foundation::Memory::Construct<physics::PhysicsBoxShape>();
        break;
      case physics::IPhysicsShape::ShapeTypes::kSphere:
        new_shape = foundation::Memory::Construct<physics::PhysicsSphereShape>();
        break;
      case physics::IPhysicsShape::ShapeTypes::kCylinder:
        new_shape = foundation::Memory::Construct<physics::PhysicsCylinderShape>();
        break;
      case physics::IPhysicsShape::ShapeTypes::kCapsule:
        new_shape = foundation::Memory::Construct<physics::PhysicsCapsuleShape>();
        break;
      case physics::IPhysicsShape::ShapeTypes::kCone:
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
      if (mesh == MeshHandle())
      {
        mesh = fallback_mesh_;
      }

      physics::IPhysicsShape* shape = nullptr;
      
      if (convex == true)
      {
        shape = convex_shapes_.find(mesh) != convex_shapes_.end() ?
                convex_shapes_.at(mesh) : nullptr;
      }
      else
      {
        shape = concave_shapes_.find(mesh) != concave_shapes_.end() ?
          concave_shapes_.at(mesh) : nullptr;
      }
      
      if (shape == nullptr)
      {
        if (convex == true)
        {
          physics::PhysicsConvexShape* new_shape =
            foundation::Memory::Construct<physics::PhysicsConvexShape>();
          new_shape->SetVertices(&mesh->GetVertices()[0], mesh->GetVertexCount());
          convex_shapes_[mesh] = new_shape;
          shape = new_shape;
        }
        else
        {
          physics::PhysicsMeshShape* new_shape =
            foundation::Memory::Construct<physics::PhysicsMeshShape>();
          new_shape->SetMesh(mesh->GetVertices(), mesh->GetIndices());
          concave_shapes_[mesh] = new_shape;
          shape = new_shape;
        }
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
                              physics::IPhysicsShape* shape)
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
      physics::IPhysicsShape* shape = collider->shape();
      physics::IPhysicsBody* owner = collider->owner();
      owner->RemoveShape(collider);

      // If shape has no refs, destroy it
      if (shape->instances().size() == 0)
      {
        if (shape->GetType() == physics::IPhysicsShape::ShapeTypes::kConvex)
        {
          foundation::Map<MeshHandle, physics::IPhysicsShape*>::iterator it =
            convex_shapes_.begin();
          while (it != convex_shapes_.end())
          {
            if (it->second == shape)
            {
              convex_shapes_.erase(it);
              it = convex_shapes_.end();
            }
            else
            {
              ++it;
            }
          }
        }
        else if (shape->GetType() == physics::IPhysicsShape::ShapeTypes::kMesh)
        {
          foundation::Map<MeshHandle, physics::IPhysicsShape*>::iterator it =
            concave_shapes_.begin();
          while (it != concave_shapes_.end())
          {
            if (it->second == shape)
            {
              concave_shapes_.erase(it);
              it = concave_shapes_.end();
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
      // Sync to physics just before simulation:
      GatherChangedTransforms();
      for (foundation::Vector<EntityBodyMapIt>::iterator it = changes_.begin();
        it != changes_.end(); ++it)
      {
        TransformComponent trans = (*it)->first.Get<TransformComponent>();
        auto world_pos = trans.GetWorldPosition();
        auto world_rot = trans.GetWorldRotation();
        (*it)->second->SetTranslation(world_pos);
        (*it)->second->SetRotation(world_rot);
      }
      changes_.clear();

      physics_->SimulateStep(delta_time);
      SyncPhysicsToEngine();

      physics::PhysicsManifold* manifolds = physics_->GetManifolds();

      for (int i = 0; i < physics_->GetManifoldsSize(); ++i)
      {
        bool new_entry = true;

        for (int j = 0; j < contact_history_size_; ++j)
        {
          if ((contact_history_[j].body_a() == manifolds[i].body_a()
              && contact_history_[j].body_b() == manifolds[i].body_b())
              ||
              (contact_history_[j].body_b() == manifolds[i].body_a()
              && contact_history_[j].body_a() == manifolds[i].body_b()))
          {
            new_entry = false;
            break;
          }
        }

        for (int j = 0; j < callbacks_.size_; ++j)
        {
          // New entries
          if (new_entry == true)
          {
            if ((manifolds[i].body_a() == callbacks_.bodies_[j]
                || manifolds[i].body_b() == callbacks_.bodies_[j])
              && (callbacks_.types_[j] == CallbackType::kCollisionEnter
                || callbacks_.types_[j] == CallbackType::kOverlapEnter))
            {
              CallCallback(j, i, manifolds[i].body_b() == callbacks_.bodies_[j]);
            }
          }
          // Stays
          else if (new_entry == false)
          {
            if ((manifolds[i].body_a() == callbacks_.bodies_[j]
                || manifolds[i].body_b() == callbacks_.bodies_[j])
              && (callbacks_.types_[j] == CallbackType::kCollisionStay
                || callbacks_.types_[j] == CallbackType::kOverlapStay))
            {
              CallCallback(j, i, manifolds[i].body_b() == callbacks_.bodies_[j]);
            }
          }
        }
      }

      // Exits
      for (int i = 0; i < contact_history_size_; ++i)
      {
        bool unique = true;

        for (int j = 0; j < physics_->GetManifoldsSize(); ++j)
        {
          if ((contact_history_[i].body_a() == manifolds[j].body_a()
              && contact_history_[i].body_b() == manifolds[j].body_b())
              ||
              (contact_history_[i].body_b() == manifolds[j].body_a()
              && contact_history_[i].body_a() == manifolds[j].body_b()))
          {
            unique = false;
          }
        }

        for (int j = 0; j < callbacks_.size_ && unique; ++j)
        {
          if ((contact_history_[i].body_a() == callbacks_.bodies_[j]
              || contact_history_[i].body_b() == callbacks_.bodies_[j])
            && (callbacks_.types_[j] == CallbackType::kCollisionLeave
              || callbacks_.types_[j] == CallbackType::kOverlapLeave))
          {
            CallCallback(j, i, contact_history_[i].body_b() == callbacks_.bodies_[j]);
          }
        }
      }

      memcpy(contact_history_, manifolds,
        sizeof(physics::PhysicsManifold) * physics_->GetManifoldsSize());
      contact_history_size_ = physics_->GetManifoldsSize();
    }

    //-------------------------------------------------------------------------
    void PhysicsSystem::GatherChangedTransforms()
    {
      // Naive sync check approach, but it's functional.
      for (EntityBodyMapIt pair = bodies_.begin(); pair != bodies_.end(); ++pair)
      {
        TransformComponent trans = pair->first.Get<TransformComponent>();
        glm::vec3 world_pos = trans.GetWorldPosition();
        glm::quat world_rot = trans.GetWorldRotation();

        bool needs_sync = world_pos != pair->second->GetTranslation();
        if (needs_sync == false)
        {
          // Check rotation. Glm throws an uninitialised variable warning with normal quat compare 
          // so it's done like this instead.
          // Transform rotation is obtained by decomposing, hence the epsilon*2 is necessary
          // to catch near-identical rotations.
          glm::quat phys_rot = pair->second->GetRotation();
          needs_sync = glm::epsilonNotEqual(world_rot.x, phys_rot.x, FLT_EPSILON*2) ||
                       glm::epsilonNotEqual(world_rot.y, phys_rot.y, FLT_EPSILON*2) ||
                       glm::epsilonNotEqual(world_rot.z, phys_rot.z, FLT_EPSILON*2) ||
                       glm::epsilonNotEqual(world_rot.w, phys_rot.w, FLT_EPSILON*2);
        }

        if (needs_sync == true)
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
      if (callbacks_.size_ == physics::IPhysics::kManifoldBufferLimit)
      {
        assert(false && "We have reached the maximum amount of callbacks");
      }

      physics::PhysicsBody* body = bodies_.at(entity);

      if (body == nullptr)
      {
        assert(false && "The entity in question does not have a physicsbody");
      }

      for (int i = callbacks_.size_; i > -1; --i)
      {
        if (body > callbacks_.bodies_[i - 1] || i == 0)
        {
          callbacks_.bodies_[i] = body;
          callbacks_.types_[i] = type;
          callbacks_.callbacks_[i] = callback;

          callbacks_.size_++;
          physics_->SubscribeCallback(body);

          break;
        }

        callbacks_.bodies_[i] = callbacks_.bodies_[i - 1];
        callbacks_.types_[i] = callbacks_.types_[i - 1];
        callbacks_.callbacks_[i] = callbacks_.callbacks_[i - 1];
      }
    }

    void PhysicsSystem::RemoveCallback(
      Entity entity,
      ScriptableCallback callback, 
      CallbackType type)
    {
      physics::PhysicsBody* body = bodies_.at(entity);

      if (body == nullptr)
      {
        assert(false && "The entity in question does not have a physicsbody");
      }

      for (int i = 0; i < callbacks_.size_; ++i)
      {
        if (callbacks_.bodies_[i] == body)
        {
          for (int j = i; j < callbacks_.size_ - 1; ++j)
          {
            callbacks_.bodies_[j] = callbacks_.bodies_[j + 1];
          }

          physics_->UnSubscribeCallback(body);
          callbacks_.size_--;
          break;
        }
      }
    }

    void PhysicsSystem::CallCallback(
      int callback, int manifold_index, bool swap_manifold_order)
    {
      physics::PhysicsManifold* manifold =
        (callbacks_.types_[callback] == CallbackType::kCollisionLeave
          || callbacks_.types_[callback] == CallbackType::kOverlapLeave)
        ? contact_history_ + manifold_index
        : physics_->GetManifolds() + manifold_index;

      ScriptTableHandle contact_points =
        ScriptUtils::CreateTable(self_->script_system_->script_state());

      for (int i = 0; i < manifold->contact_points().size(); ++i)
      {
        ScriptTableHandle contact_point =
          ScriptUtils::CreateTable(self_->script_system_->script_state());

        contact_point->Insert("point", manifold->contact_points()[i].point_);
        contact_point->Insert("normal", manifold->contact_points()[i].normal_);
        contact_point->Insert("seperation", manifold->contact_points()[i].seperation_);

        contact_points->Insert(i, contact_point);
      }

      Entity a = entities_.at(manifold->body_a());
      Entity b = entities_.at(manifold->body_b());

      ScriptTableHandle manifold_table = ScriptUtils::CreateTable(
        self_->script_system_->script_state());
      manifold_table->Insert("body_a", a);
      manifold_table->Insert("body_a_id", a.handle);
      manifold_table->Insert("body_b", b);
      manifold_table->Insert("body_b_id", b.handle);
      manifold_table->Insert("contact_points", contact_points);

      callbacks_.callbacks_[callback].Call({ manifold_table });
    }

    //-------------------------------------------------------------------------
    ScriptHandle PhysicsSystem::RayCast(const foundation::Ray& ray, float distance)
    {
      physics::RaycastHitInfo hit_info;
      
      self_->physics_->Raycast(ray, &hit_info, distance);

      ScriptTableHandle raycast_info_handle =
        ScriptUtils::CreateTable(self_->script_system_->script_state());
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
      ScriptTableHandle raycasthits_info_handle = 
        ScriptUtils::CreateTable(self_->script_system_->script_state());

      for (int i = 0; i < hits.size(); ++i)
      {
        ScriptTableHandle raycast_info_handle =
          ScriptUtils::CreateTable(self_->script_system_->script_state());
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
