#include "transform_system.h"

#include "engine/application/application.h"
#include "engine/core/world.h"

#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>

#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/imgui/imgui.h>

#include <functional>
#include <algorithm>

#define PS_FAST_QUATERNION_INVERSE

namespace sulphur
{
  namespace engine
  {
    // There doesn't seem to be any quaternion "look at" in glm
    glm::quat LookRotation(const glm::vec3& forward, const glm::vec3& up)
    {
      if (forward == glm::vec3())
        return glm::quat(1, 0, 0, 0);

      const glm::vec3 new_forward = glm::normalize(forward);
      const glm::vec3 new_right = glm::normalize(glm::cross(up, new_forward));
      const glm::vec3 new_up = glm::cross(new_forward, new_right);

      const float m00 = new_right.x;
      const float m01 = new_right.y;
      const float m02 = new_right.z;
      const float m10 = new_up.x;
      const float m11 = new_up.y;
      const float m12 = new_up.z;
      const float m20 = new_forward.x;
      const float m21 = new_forward.y;
      const float m22 = new_forward.z;

      const float num8 = (m00 + m11) + m22;
      glm::quat quaternion;
      if (num8 > 0.0f)
      {
        float num = sqrtf(num8 + 1.0f);
        quaternion.w = num * 0.5f;
        num = 0.5f / num;
        quaternion.x = (m12 - m21) * num;
        quaternion.y = (m20 - m02) * num;
        quaternion.z = (m01 - m10) * num;
        return quaternion;
      }
      if ((m00 >= m11) && (m00 >= m22))
      {
        const float num7 = sqrtf(((1.0f + m00) - m11) - m22);
        const float num4 = 0.5f / num7;
        quaternion.x = 0.5f * num7;
        quaternion.y = (m01 + m10) * num4;
        quaternion.z = (m02 + m20) * num4;
        quaternion.w = (m12 - m21) * num4;
        return quaternion;
      }
      if (m11 > m22)
      {
        const float num6 = sqrtf(((1.0f + m11) - m00) - m22);
        const float num3 = 0.5f / num6;
        quaternion.x = (m10 + m01) * num3;
        quaternion.y = 0.5f * num6;
        quaternion.z = (m21 + m12) * num3;
        quaternion.w = (m20 - m02) * num3;
        return quaternion;
      }
      const float num5 = sqrtf(((1.0f + m22) - m00) - m11);
      const float num2 = 0.5f / num5;
      quaternion.x = (m20 + m02) * num2;
      quaternion.y = (m21 + m12) * num2;
      quaternion.z = 0.5f * num5;
      quaternion.w = (m01 - m10) * num2;
      return quaternion;
    }
    
    // Source glm::decompose();
    void Decompose(glm::mat4 const& model_matrix, glm::vec3& scale, glm::quat& orientation, glm::vec3& translation)
    {
      glm::mat4 local_matrix(model_matrix);
            
      // Next take care of translation (easy).
      translation = glm::vec3(local_matrix[3]);
      local_matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, local_matrix[3][3]);

      glm::vec3 row[3];

      // Now get scale and shear.
      for (glm::length_t i = 0; i < 3; ++i)
      {
        for (glm::length_t j = 0; j < 3; ++j)
        {
          row[i][j] = local_matrix[i][j];
        }
      }

      // Compute X scale factor and normalize first row.
      scale.x = glm::length(row[0]);
      row[0] /= scale.x;

      // Now, compute Y scale and normalize 2nd row.
      scale.y = glm::length(row[1]);
      row[1] /= scale.y;
      
      // Next, get Z scale and normalize 3rd row.
      scale.z = glm::length(row[2]);
      row[2] /= scale.z;
      
      // At this point, the matrix (in rows[]) is orthonormal.
      // Check for a coordinate system flip.  If the determinant
      // is -1, then negate the matrix and the scaling factors.
      glm::vec3 p_dum3 = glm::cross(row[1], row[2]);
      if (glm::dot(row[0], p_dum3) < 0)
      {
        for (glm::length_t i = 0; i < 3; i++)
        {
          scale[i] *= -1.0f;
          row[i] *= -1.0f;
        }
      }

      float root, trace = row[0].x + row[1].y + row[2].z;
      if (trace > 0.0f)
      {
        root = glm::sqrt(trace + 1.0f);
        orientation.w = 0.5f * root;
        root = 0.5f / root;
        orientation.x = root * (row[1].z - row[2].y);
        orientation.y = root * (row[2].x - row[0].z);
        orientation.z = root * (row[0].y - row[1].x);
      } // End if > 0
      else
      {
        constexpr int next[3] = { 1, 2, 0 };

        int i = 0;

        if (row[1].y > row[0].x)
        {
          i = 1;
        }

        if (row[2].z > row[i][i])
        {
          i = 2;
        }

        int j = next[i];
        int k = next[j];

        root = glm::sqrt(row[i][i] - row[j][j] - row[k][k] + 1.0f);

        orientation[i] = 0.5f * root;
        root = 0.5f / root;
        orientation[j] = root * (row[i][j] + row[j][i]);
        orientation[k] = root * (row[i][k] + row[k][i]);
        orientation.w = root * (row[j][k] - row[k][j]);
      } // End if <= 0
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformSystem::root_;

    //-------------------------------------------------------------------------
    TransformComponent::TransformComponent() :
      system_(nullptr)
    {
    }
    TransformComponent::TransformComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    glm::mat4 TransformComponent::GetLocal() const
    {
      return system_->GetLocal(*this);
    }
    glm::mat4 TransformComponent::GetLocalToWorld()
    {
      return system_->GetLocalToWorld(*this);
    }
    glm::mat4 TransformComponent::GetWorldToLocal()
    {
      return system_->GetWorldToLocal(*this);
    }

    //-------------------------------------------------------------------------
    void TransformComponent::SetParent(TransformComponent parent)
    {
      system_->SetParent(*this, parent);
    }
    void TransformComponent::UnsetParent()
    {
      system_->UnsetParent(*this);
    }
    void TransformComponent::AttachChild(TransformComponent child)
    {
      system_->AttachChild(*this, child);
    }
    void TransformComponent::DetachChild(TransformComponent child)
    {
      system_->UnsetParent(child);
    }

    //-------------------------------------------------------------------------
    bool TransformComponent::HasParent() const
    {
      return system_->HasParent(*this);
    }

    //-------------------------------------------------------------------------
    bool TransformComponent::HasChanged() const
    {
      return system_->HasChanged(*this);
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::GetRoot() const
    {
      return system_->GetRoot(*this);
    }
    TransformComponent TransformComponent::GetParent() const
    {
      return system_->GetParent(*this);
    }
    foundation::Vector<TransformComponent> TransformComponent::GetSiblings()
    {
      return system_->GetSiblings(*this);
    }
    size_t TransformComponent::GetSiblingIndex()
    {
      return system_->GetSiblingIndex(*this);
    }
    void TransformComponent::SetSiblingIndex(size_t index)
    {
      system_->SetSiblingIndex(*this, index);
    }
    foundation::Vector<TransformComponent> TransformComponent::GetChildren(bool recursive) const
    {
      return system_->GetChildren(*this, recursive);
    }
    size_t TransformComponent::GetChildCount(bool recursive) const
    {
      return system_->GetChildCount(*this, recursive);
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::SetWorldPosition(const glm::vec3& position)
    {
      if (HasParent() == false)
      {
        return SetLocalPosition(position);
      }

      const glm::mat4 world_to_local = GetParent().GetWorldToLocal();
      const glm::vec3 local_position = world_to_local * glm::vec4(position, 1.0f);
      return SetLocalPosition(local_position);
    }
    TransformComponent TransformComponent::SetWorldRotation(const glm::quat& rotation)
    {
      if (HasParent() == false)
      {
        return SetLocalRotation(rotation);
      }

      // NOTE: Should assert if not-normalized
      const glm::quat parent_rotation = system_->GetParent(*this).GetWorldRotation();
      // q' represents the rotation from the rotation q1 to the rotation q2
      // q' = q1^-1 * q2
      const glm::quat local_rotation = glm::inverse(parent_rotation) * rotation;
      return SetLocalRotation(local_rotation);

    }
    TransformComponent TransformComponent::SetWorldScale(const glm::vec3& scale)
    {
      if (HasParent() == false)
      {
        return SetLocalScale(scale);
      }

      const glm::vec3 parent_scale = system_->GetParent(*this).GetWorldScale();
      const glm::vec3 local_scale = scale / parent_scale;
      return SetLocalScale(local_scale);
    }
    TransformComponent TransformComponent::SetLocalPosition(const glm::vec3& position)
    {
      system_->SetLocalPosition(*this, position);
      return *this;
    }
    TransformComponent TransformComponent::SetLocalRotation(const glm::quat& rotation)
    {
      system_->SetLocalRotation(*this, rotation);
      return *this;
    }
    TransformComponent TransformComponent::SetLocalScale(const glm::vec3& scale)
    {
      system_->SetLocalScale(*this, scale);
      return *this;
    }

    //-------------------------------------------------------------------------
    glm::vec3 TransformComponent::GetWorldPosition()
    {
      return system_->GetWorldPosition(*this);
    }
    glm::quat TransformComponent::GetWorldRotation()
    {
      return system_->GetWorldRotation(*this);
    }
    glm::vec3 TransformComponent::GetWorldScale()
    {
      return system_->GetWorldScale(*this);
    }
    glm::vec3 TransformComponent::GetLocalPosition() const
    {
      return system_->GetLocalPosition(*this);
    }
    glm::quat TransformComponent::GetLocalRotation() const
    {
      return system_->GetLocalRotation(*this);
    }
    glm::vec3 TransformComponent::GetLocalScale() const
    {
      return system_->GetLocalScale(*this);
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::SetWorldUp(const glm::vec3& up, glm::vec3 right)
    {
      const glm::quat forward_to_up(.7f, .7f, 0, 0);
      return SetWorldRotation(LookRotation(up, right) * forward_to_up);
    }
    TransformComponent TransformComponent::SetWorldRight(const glm::vec3& right, glm::vec3 forward)
    {
      const glm::quat forward_to_right(-.7f, 0, .7f, 0);
      return SetWorldRotation(LookRotation(right, forward) * forward_to_right);
    }
    TransformComponent TransformComponent::SetWorldForward(const glm::vec3& forward, glm::vec3 up)
    {
      return SetWorldRotation(LookRotation(forward, up));
    }
    TransformComponent TransformComponent::SetLocalUp(const glm::vec3& up, glm::vec3 right)
    {
      const glm::quat forward_to_up(.7f, .7f, 0, 0);
      return SetLocalRotation(LookRotation(up, right) * forward_to_up);
    }
    TransformComponent TransformComponent::SetLocalRight(const glm::vec3& right, glm::vec3 forward)
    {
      const glm::quat forward_to_right(-.7f, 0, .7f, 0);
      return SetLocalRotation(LookRotation(right, forward) * forward_to_right);
    }
    TransformComponent TransformComponent::SetLocalForward(const glm::vec3& forward, glm::vec3 up)
    {
      return SetLocalRotation(LookRotation(forward, up));
    }

    //-------------------------------------------------------------------------
    glm::vec3 TransformComponent::GetWorldUp()
    {
      return TransformVector(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    glm::vec3 TransformComponent::GetWorldRight()
    {
      return TransformVector(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    glm::vec3 TransformComponent::GetWorldForward()
    {
      return TransformVector(glm::vec3(0.0f, 0.0f, -1.0f));
    }
    glm::vec3 TransformComponent::GetLocalUp() const
    {
      return TransformLocalVector(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    glm::vec3 TransformComponent::GetLocalRight() const
    {
      return TransformLocalVector(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    glm::vec3 TransformComponent::GetLocalForward() const
    {
      return TransformLocalVector(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::TranslateWorld(const glm::vec3& offset)
    {
      return SetWorldPosition(GetWorldPosition() + offset);
    }
    TransformComponent TransformComponent::RotateWorld(const glm::quat& rotation)
    {
      return SetWorldRotation(GetWorldRotation() * rotation);
    }
    TransformComponent TransformComponent::ScaleWorld(const glm::vec3& scale)
    {
      return SetWorldScale(GetWorldScale() * scale);
    }
    TransformComponent TransformComponent::RotateWorldEuler(const glm::vec3& angles)
    {
      return RotateWorld(glm::quat(angles));
    }
    TransformComponent TransformComponent::TranslateLocal(const glm::vec3& offset)
    {
      return SetLocalPosition(GetLocalPosition() + offset);
    }
    TransformComponent TransformComponent::RotateLocal(const glm::quat& rotation)
    {
      return SetLocalRotation(GetLocalRotation() * rotation);
    }
    TransformComponent TransformComponent::ScaleLocal(const glm::vec3& scale)
    {
      return SetLocalScale(GetLocalScale() * scale);
    }
    TransformComponent TransformComponent::RotateLocalEuler(const glm::vec3& angles)
    {
      return RotateLocal(glm::quat(angles));
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::RotateWorldX(float angle)
    {
      return SetWorldRotation(GetWorldRotation() * glm::quat(glm::vec3(angle, 0.0f, 0.0f)));
    }
    TransformComponent TransformComponent::RotateWorldY(float angle)
    {
      return SetWorldRotation(GetWorldRotation() * glm::quat(glm::vec3(0.0f, angle, 0.0f)));
    }
    TransformComponent TransformComponent::RotateWorldZ(float angle)
    {
      return SetWorldRotation(GetWorldRotation() * glm::quat(glm::vec3(0.0f, 0.0f, -angle)));
    }
    TransformComponent TransformComponent::RotateAroundWorld(const glm::vec3& center, const glm::vec3& axis, float angle)
    {
      return RotateAroundWorld(center, glm::angleAxis(angle, axis));
    }
    TransformComponent TransformComponent::RotateAroundWorld(const glm::vec3& center, const glm::quat& rotation)
    {
      const glm::vec3 dir = rotation * (GetWorldPosition() - center);
      SetWorldPosition(center + dir);

      glm::quat temp_rot = GetWorldRotation();
      return RotateWorld(glm::inverse(temp_rot) * rotation * temp_rot);
    }
    TransformComponent TransformComponent::RotateLocalX(float angle)
    {
      return SetLocalRotation(GetLocalRotation() * glm::quat(glm::vec3(angle, 0.0f, 0.0f)));
    }
    TransformComponent TransformComponent::RotateLocalY(float angle)
    {
      return SetLocalRotation(GetLocalRotation() * glm::quat(glm::vec3(0.0f, angle, 0.0f)));
    }
    TransformComponent TransformComponent::RotateLocalZ(float angle)
    {
      return SetLocalRotation(GetLocalRotation() * glm::quat(glm::vec3(0.0f, 0.0f, -angle)));
    }
    TransformComponent TransformComponent::RotateAroundLocal(const glm::vec3& center, const glm::vec3& axis, float angle)
    {
      return RotateAroundLocal(center, glm::angleAxis(angle, axis));
    }
    TransformComponent TransformComponent::RotateAroundLocal(const glm::vec3& center, const glm::quat& rotation)
    {
      const glm::vec3 dir = rotation * (GetLocalPosition() - center);
      SetLocalPosition(center + dir);

      glm::quat temp_rot = GetLocalRotation();
      return RotateLocal(glm::inverse(temp_rot) * rotation * temp_rot);
    }

    //-------------------------------------------------------------------------
    glm::vec3 TransformComponent::TransformPoint(const glm::vec3& point)
    {
      return GetLocalToWorld() * glm::vec4(point, 1.0f);
    }
    glm::vec3 TransformComponent::TransformVector(const glm::vec3& vector)
    {
      return GetLocalToWorld() * glm::vec4(vector, 0.0f);
    }
    glm::vec3 TransformComponent::TransformLocalPoint(const glm::vec3& point) const
    {
      return GetLocal() * glm::vec4(point, 0.0f);
    }
    glm::vec3 TransformComponent::TransformLocalVector(const glm::vec3& vector) const
    {
      return GetLocal() * glm::vec4(vector, 0.0f);
    }
    glm::vec3 TransformComponent::InverseTransformPoint(const glm::vec3& point)
    {
      return GetWorldToLocal() * glm::vec4(point, 1.0f);
    }
    glm::vec3 TransformComponent::InverseTransformVector(const glm::vec3& vector)
    {
      return GetWorldToLocal() * glm::vec4(vector, 0.0f);
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::Concatenate(TransformComponent /*other*/)
    {
      assert(false && "Not implemented");
      return{};
    }
    TransformComponent TransformComponent::Blend(TransformComponent /*target*/, float /*alpha*/)
    {
      assert(false && "Not implemented");
      return{};
    }
    TransformComponent TransformComponent::LookAt(const glm::vec3& target, glm::vec3 up)
    {
      const glm::vec3 direction = glm::normalize(target - GetWorldPosition());
      return SetLocalRotation(LookRotation(direction, up));
    }
    TransformComponent TransformComponent::LookAtLocal(const glm::vec3& target, glm::vec3 up)
    {
      const glm::vec3 direction = glm::normalize(target - GetLocalPosition());
      return SetLocalRotation(LookRotation(direction, up));
    }

    //-------------------------------------------------------------------------
    SortingLayer TransformComponent::GetSortingLayer() const
    {
      return system_->GetSortingLayer(*this);
    }
    void TransformComponent::SetSortingLayer(const SortingLayer& sorting_layer)
    {
      system_->SetSortingLayer(*this, sorting_layer);
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformComponent::SetWorldRotationEuler(const glm::vec3& angles)
    {
      SetWorldRotation(glm::quat(angles));
      return *this;
    }
    TransformComponent TransformComponent::SetLocalRotationEuler(const glm::vec3& angles)
    {
      SetLocalRotation(glm::quat(angles));
      return *this;
    }
    glm::vec3 TransformComponent::GetWorldRotationEuler()
    {
      return glm::eulerAngles(GetWorldRotation());
    }
    glm::vec3 TransformComponent::GetLocalRotationEuler() const
    {
      return glm::eulerAngles(GetLocalRotation());
    }

    //-------------------------------------------------------------------------
    TransformSystem::TransformSystem() :
      IComponentSystem("TransformSystem")
    {
      root_.handle = PS_UINT_MAX;
    }

    //-------------------------------------------------------------------------
    void TransformSystem::OnInitialize(Application&, foundation::JobGraph&)
    {
    }
    
    ///------------------------------------------------------------------------
    TransformComponent TransformSystem::GetRoot(const TransformComponent& child_node)
    {
      const TransformComponent& parent = child_node.GetParent();
      if (parent != root_)
      {
        return GetRoot(parent);
      }
      else
      {
        return child_node;
      }
    }

    //-------------------------------------------------------------------------
    void TransformSystem::OnUpdate(float)
    {
      for (TransformData& t : *(data_))
      {
        t.changed = false;
      }
    }

    //-------------------------------------------------------------------------
    TransformComponent TransformSystem::Create(Entity entity)
    {
      TransformComponent ret(*this, sparse_array_.size());
      
      TransformData new_data;
      new_data.flags |= static_cast<int>(DirtyFlags::kLocal);
      new_data.flags |= static_cast<int>(DirtyFlags::kParent);
      new_data.local_to_parent = glm::mat4(1.0f);
      new_data.parent = root_;
      new_data.child_count = 0;
      new_data.entity = entity;

      new_data.name = "Transform " + foundation::to_string(ret.handle);

      
      dense_to_sparse_array_.emplace_back(ret.handle);
      sparse_array_.emplace_back(data_->size());
      data_->emplace_back(eastl::move(new_data));
      return ret;
    }
    void TransformSystem::Destroy(TransformComponent handle)
    {
      if (!handle.IsValid())
      {
        return;
      }
      
      // First decrease parents child count
      TransformData& data = LookUpData(handle);
      if (data.parent != root_)
      {
        TransformData& old_parent_data = LookUpData(data.parent);
        old_parent_data.child_count--;
      }

      const size_t removed_idx = sparse_array_[handle.Handle()].handle;
      TransformData* data_begin = data_->begin();

      // Recursively remove all children
      for (int i = 0; i < data.child_count; ++i)
      {
        TransformComponent h;
        h.handle = dense_to_sparse_array_[removed_idx + 1 + i].handle;

        TransformData& d = *(data_begin + h.handle);
        d.entity.Remove<TransformComponent>(h);
      }

      // Delete myself
      // Move everything one up, remove one element from the back

      const size_t first_it = removed_idx + 1;
      const size_t last_it = data_->size();
      const int offset = -1;

      
      DenseHandle* dense_begin = dense_to_sparse_array_.begin();

      // Move data `offset` spaces in inverse order
      for (int i = (int)last_it - 1; i >= (int)first_it; --i)
      {
        const DenseHandle& dense_a = *(dense_begin + i);
        const DenseHandle& dense_b = *(dense_begin + offset + i);

        SparseHandle& sparse_a = sparse_array_[dense_a.handle];
        const SparseHandle& sparse_b = sparse_array_[dense_b.handle];

        sparse_a = sparse_b;
      }

      // Move data `offset` spaces
      eastl::move(data_begin + first_it, data_begin + last_it, data_begin + first_it + offset);
      eastl::move(dense_begin + first_it, dense_begin + last_it, dense_begin + first_it + offset);

      sparse_array_[handle.Handle()] = PS_UINT_MAX;
      data_->pop_back();
      dense_to_sparse_array_.pop_back();
    }

    ///------------------------------------------------------------------------
    void TransformSystem::CalculateChildCount(foundation::Vector<TransformData>::iterator& transform_data, size_t& out_child_count)
    {
      out_child_count += transform_data->child_count;

      const size_t child_count = transform_data->child_count;
      for (size_t i = 0; i < child_count; ++i) // Last child count is always 0
      {
        CalculateChildCount(++transform_data, out_child_count);
      }
    }

    ///------------------------------------------------------------------------
    TransformSystem::TransformData& TransformSystem::FindRootNode(TransformData& child_node)
    {
      TransformData* node = &child_node;

      while (node->parent != root_)
      {
        --node;
      }

      return *node;
    }
    
    ///------------------------------------------------------------------------
    void TransformSystem::MoveData(size_t first_it, size_t last_it, int offset)
    {
      if (offset == 0)
        return;

      const size_t temp_room = std::abs(offset);
      foundation::Vector<TransformData> temp_storage;
      foundation::Vector<DenseHandle> temp_storage_dense;
      foundation::Vector<SparseHandle> temp_storage_sparse;

      temp_storage.reserve(temp_room);
      temp_storage_dense.reserve(temp_room);
      temp_storage_sparse.reserve(temp_room);
      
      TransformData* data_begin = data_->begin();
      DenseHandle* dense_begin = dense_to_sparse_array_.begin();
      if (offset < 0)
      {
        // Move up
        
        // First modify the sparse handles
        // Store offset into temp storage
        for (size_t i = last_it + offset; i < last_it; ++i)
        {
          const DenseHandle& dense = *(dense_begin + i);
          SparseHandle& spase = sparse_array_[dense.handle];

          temp_storage_sparse.emplace_back(eastl::move(spase));
        }

        // Move data `offset` spaces in inverse order
        for (int i = (int)last_it - 1; i >= (int)first_it; --i)
        {
          const DenseHandle& dense_a = *(dense_begin + i);
          const DenseHandle& dense_b = *(dense_begin + offset + i);

          SparseHandle& sparse_a = sparse_array_[dense_a.handle];
          const SparseHandle& sparse_b = sparse_array_[dense_b.handle];

          sparse_a = sparse_b;
        }

        // Restore temp_storage
        for (size_t i = 0; i < temp_room; ++i)
        {
          const DenseHandle& dense_a = *(dense_begin + first_it + offset + i);

          SparseHandle& sparse_a = sparse_array_[dense_a.handle];
          const SparseHandle& sparse_b = temp_storage_sparse[i];

          sparse_a = sparse_b;
        }

        // Then modify the data and dense handles
        // Store offset into temp storage
        for (size_t i = first_it + offset; i < first_it; ++i)
        {
          TransformData& t = *(data_begin + i);
          temp_storage.emplace_back(eastl::move(t));

          DenseHandle& dense = *(dense_begin + i);
          temp_storage_dense.emplace_back(eastl::move(dense));
        }

        // Move data `offset` spaces
        eastl::move(data_begin + first_it, data_begin + last_it, data_begin + first_it + offset);
        eastl::move(dense_begin + first_it, dense_begin + last_it, dense_begin + first_it + offset);

        // Restore temp_storage
        eastl::move(temp_storage.begin(), temp_storage.end(), data_begin + last_it + offset);
        eastl::move(temp_storage_dense.begin(), temp_storage_dense.end(), dense_begin + last_it + offset);
      }
      else
      {
        // Move down

        // First modify the sparse handles
        // Store offset into temp storage
        for (size_t i = first_it; i < first_it + offset; ++i)
        {
          const DenseHandle& dense = *(dense_begin + i);
          const SparseHandle& spase = sparse_array_[dense.handle];

          temp_storage_sparse.emplace_back(eastl::move(spase));
        }

        // Move data `offset` spaces
        for (size_t i = first_it; i < last_it; ++i)
        {
          const DenseHandle& dense_a = *(dense_begin + i);
          const DenseHandle& dense_b = *(dense_begin + offset + i);

          SparseHandle& sparse_a = sparse_array_[dense_a.handle];
          const SparseHandle& sparse_b = sparse_array_[dense_b.handle];

          sparse_a = sparse_b;
        }

        // Restore temp_storage
        for (size_t i = 0; i < temp_room; ++i)
        {
          const DenseHandle& dense = *(dense_begin + last_it + i);

          SparseHandle& sparse_a = sparse_array_[dense.handle];
          const SparseHandle& sparse_b = temp_storage_sparse[i];
          
          sparse_a = sparse_b;
        }

        // Then modify the data and dense handles
        // Store offset into temp storage
        for (size_t i = last_it; i < last_it + offset; ++i)
        {
          TransformData& t = *(data_begin + i);
          temp_storage.emplace_back(eastl::move(t));

          const DenseHandle& dense = *(dense_begin + i);
          temp_storage_dense.emplace_back(eastl::move(dense));
        }

        // Move data `offset` spaces down in inverse order
        for (int i = (int)last_it - 1; i >= (int)first_it; --i)
        {
          *(data_begin + offset + i) = eastl::move(*(data_begin + i));
          *(dense_begin + offset + i) = eastl::move(*(dense_begin + i));
        }

        // Restore temp_storage
        eastl::move(temp_storage.begin(), temp_storage.end(), data_begin + first_it);
        eastl::move(temp_storage_dense.begin(), temp_storage_dense.end(), dense_begin + first_it);
      }
    }

    //-------------------------------------------------------------------------
    void TransformSystem::SetParent(TransformComponent handle, TransformComponent parent)
    {
      if (handle == parent)
      {
        PS_LOG(Warning, "A transform node can't be its own parent");
        return;
      }

      TransformData& data = LookUpData(handle);
      if (data.parent == parent || !parent.IsValid())
      {
        return;
      }
      CleanIfDirty(data, CleanFlags::kWorld);

      if (data.parent != root_)
      {
        TransformData& old_parent_data = LookUpData(data.parent);
        old_parent_data.child_count--;
      }

      TransformData& parent_data = LookUpData(parent);
      data.local_to_parent = parent_data.cached_world_to_local * data.cached_local_to_world;

      data.flags |= static_cast<int>(DirtyFlags::kParent);
      data.parent = parent;

      const size_t parent_idx = sparse_array_[parent.Handle()].handle;
      const size_t child_idx = sparse_array_[handle.Handle()].handle;


      TransformData* begin = data_->begin() + child_idx;
      size_t child_count = 1;
      CalculateChildCount(begin, child_count);

      size_t parent_offset = 1;
      begin = data_->begin() + parent_idx;
      CalculateChildCount(begin, parent_offset);

      parent_data.child_count++;

      size_t first_it = child_idx;
      size_t last_it = child_idx + child_count;
      int offset = (int)(parent_idx + parent_offset) - (int)child_idx;

      if (parent_idx > child_idx)
        offset -= (int)child_count;

      MoveData(first_it, last_it, offset);
    }
    
    void TransformSystem::UnsetParent(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);

      if (data.parent == root_)
      {
        return;
      }

      TransformComponent root = GetRoot(handle); // Calculate root before changing any data
      const size_t root_idx = sparse_array_[root.Handle()].handle;
      TransformData* root_begin = data_->begin() + root_idx;
      size_t root_child_count = 1;
      CalculateChildCount(root_begin, root_child_count);

      const size_t parent_idx = sparse_array_[data.parent.Handle()].handle;

      CleanIfDirty(data, CleanFlags::kWorld);
      data.local_to_parent = data.cached_local_to_world;

      TransformData& parent_data = LookUpData(data.parent);
      
      data.flags |= static_cast<int>(DirtyFlags::kParent);
      data.parent = root_;

      parent_data.child_count--;

      size_t parent_offset = 1;
      TransformData* begin = data_->begin() + parent_idx;
      CalculateChildCount(begin, parent_offset);

      // Move data to after the last child in this tree node
      
      const size_t child_idx = sparse_array_[handle.Handle()].handle;
      begin = data_->begin() + child_idx;
      size_t child_child_count = 1;
      CalculateChildCount(begin, child_child_count);

      size_t first_it = child_idx;
      size_t last_it = child_idx + child_child_count;
      int offset = (int)(data_->size() - child_child_count) - (int)child_idx;
      
      MoveData(first_it, last_it, offset);
    }

    //-------------------------------------------------------------------------
    foundation::Vector<TransformComponent> TransformSystem::GetSiblings(TransformComponent handle)
    {
#ifdef _DEBUG
      if (!handle.IsValid())
      {
        return foundation::Vector<TransformComponent>();
      }
#endif
      // TODO: Fix if parent == root_

      TransformComponent parent = handle.GetParent();

      size_t parent_index = sparse_array_[parent.Handle()].handle;
      size_t child_count = GetChildCount(parent, true);

      foundation::Vector<TransformComponent> siblings;

      for (size_t i = 0; i < child_count; ++i)
      {
        if ((*data_)[parent_index + 1 + i].parent != parent)
        {
          continue;
        }

        TransformComponent h;
        h.handle = dense_to_sparse_array_[parent_index + 1 + i].handle;
        siblings.emplace_back(eastl::move(h));
      }

      return siblings;
    }
    size_t TransformSystem::GetSiblingIndex(TransformComponent handle)
    {
#ifdef _DEBUG
      if (!handle.IsValid())
      {
        return PS_SIZE_MAX;
      }
#endif
      // TODO: Fix if parent == root_

      size_t child_index = sparse_array_[handle.Handle()].handle;

      TransformComponent parent = handle.GetParent();

      size_t parent_index = sparse_array_[parent.Handle()].handle;
      size_t child_count = GetChildCount(parent, true);
      
      size_t sibling_index = 0;
      for (size_t i = 0; i < child_count; ++i)
      {
        if ((*data_)[parent_index + 1 + i].parent != parent)
        {
          continue;
        }

        if (parent_index + 1 + i == child_index)
        {
          return sibling_index;
        }

        sibling_index++;
      }

      return PS_SIZE_MAX;
    }
    void TransformSystem::SetSiblingIndex(TransformComponent /*handle*/, size_t /*index*/)
    {
      //JELLE: FIXME
      assert(false && "Not implemented");
    }
    foundation::Vector<TransformComponent> TransformSystem::GetChildren(TransformComponent handle, bool recursive)
    {
#ifdef _DEBUG
      if (!handle.IsValid())
      {
        return foundation::Vector<TransformComponent>();
      }
#endif
      size_t parent_index = sparse_array_[handle.Handle()].handle;
      size_t child_count = GetChildCount(handle, recursive);
      
      foundation::Vector<TransformComponent> children;
      children.reserve(child_count);

      for (size_t i = 0; i < child_count; ++i)
      {
        TransformComponent h;
        h.handle = dense_to_sparse_array_[parent_index + 1 + i].handle;
        children.emplace_back(eastl::move(h));
      }

      return children;
    }

    //-------------------------------------------------------------------------
    glm::vec3 TransformSystem::GetWorldPosition(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);
      return data.cached_world_position;
    }
    glm::quat TransformSystem::GetWorldRotation(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);
      return data.cached_world_rotation;
    }
    glm::vec3 TransformSystem::GetWorldScale(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);
      return data.cached_world_scale;
    }

    //-------------------------------------------------------------------------
    SortingLayer TransformSystem::GetSortingLayer(TransformComponent handle) const
    {
      return LookUpData(handle).sorting_layer;
    }

    void TransformSystem::SetSortingLayer(TransformComponent handle, const SortingLayer& sorting_layer)
    {
      LookUpData(handle).sorting_layer = sorting_layer;
    }

    //-------------------------------------------------------------------------
    void TransformSystem::CleanIfDirty(TransformData& data, CleanFlags flag)
    {
      auto rebuild_world = [](TransformData& data, TransformData* parent = nullptr)
      {
        if (parent == nullptr)
        {
          data.cached_local_to_world = data.local_to_parent;
          data.cached_world_to_local = glm::inverse(data.cached_local_to_world);
        }
        else
        {
          data.cached_local_to_world = parent->cached_local_to_world * data.local_to_parent;
          data.cached_world_to_local = glm::inverse(data.cached_local_to_world);
        }

        Decompose(
          data.cached_local_to_world, 
          data.cached_world_scale,
          data.cached_world_rotation,
          data.cached_world_position);

        Decompose(
          data.local_to_parent,
          data.cached_local_scale,
          data.cached_local_rotation,
          data.cached_local_position);
      };
      
      if (flag != CleanFlags::kNone)
      {
        TransformData* begin = &FindRootNode(data);
        TransformData* end = &data; ++end;

        TransformData* parent = nullptr;
        bool rebuild = false;
        for (TransformData* it = begin; it != end; ++it)
        {
          if (rebuild == false && it->flags == 0)
          {
            continue;
          }
          rebuild = true;

          parent = it->parent == root_ ? nullptr : &LookUpData(it->parent); // TODO: Get parent directly
          rebuild_world(*it, parent);

          it->flags = 0;
        }
      }
    }
  }
}
