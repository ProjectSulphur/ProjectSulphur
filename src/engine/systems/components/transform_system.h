#pragma once

#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/utilities/layer.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <foundation/containers/vector.h>
#include <foundation/containers/array.h>

#include <functional>

namespace sulphur
{
  namespace engine
  {
    class TransformSystem;
    using SortingLayerEntityMap = foundation::Array<foundation::Vector<Entity>, SortingLayer::kLayerCount>; //!< SortingLayer to Entity map

    /**
    * @class sulphur::engine::TransformComponent : public sulphur::engine::ComponentHandleBase
    * @brief An entity component representing a transformation relative to the world root
    * @see sulphur::engine::TransformSystem
    * @author Maarten ten Velden
    */
    class TransformComponent : public ComponentHandleBase
    {
    public:
      using System = TransformSystem; //!< The associate component system
      
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      TransformComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::TransformComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      TransformComponent(System& system, size_t handle);

      /**
      * @brief Retrieve the matrix representing a transformation from the node's local to its parent's space (defaults to the root)
      * @return (glm::mat4) A copy of the resulting matrix
      */
      glm::mat4 GetLocal() const;
      /**
      * @brief Retrieve the matrix representing a transformation from the node's local to the root's space
      * @return (glm::mat4) A copy of the resulting matrix
      */
      glm::mat4 GetLocalToWorld();
      /**
      * @brief Retrieve the matrix representing a transformation from the root's to the node's local space
      * @remarks When any of the nodes in this node's parent-hierarchy (including itself) has a local scale of 0 on any of its axes
      *          the results are undefined. Execution will not be altered, but the result is very likely undesired.
      * @return (glm::mat4) A copy of the resulting matrix
      */
      glm::mat4 GetWorldToLocal();

      /**
      * @brief Set the parent of the node
      * @param[in] parent (sulphur::engine::TransformComponent) The target parent component
      */
      void SetParent(TransformComponent parent);
      /**
      * @brief Set the parent of the node to the root node.
      */
      void UnsetParent();
      /**
      * @brief Sets the parent of the child node to this node
      * @see sulphur::engine::TransformComponent::SetParent
      */
      void AttachChild(TransformComponent child);
      /**
      * @brief Set the parent of the child node to the root node
      * @see sulphur::engine::TransformComponent::UnsetParent
      */
      void DetachChild(TransformComponent child);

      /**
      * @brief Test if the node's parent is not the root node
      * @return (bool) True if the node's parent is not the root node
      */
      bool HasParent() const;

      /**
      * @brief Test if the transform has changed
      * @return (bool) True if the transform has changed
      */
      bool HasChanged() const;

      /**
      * @brief Gets the top most node of this transform
      * @returns (TransformComponent) The root node
      */
      TransformComponent GetRoot() const;

      /**
      * @brief Retrieve the node's parent
      * @see sulphur::engine::TransformComponent::HasParent
      * @remark If called on the root node results are undefined
      */
      TransformComponent GetParent() const;

      /**
      * @brief Retrieve a list of sibling nodes (nodes that share the same parent as this node)
      * @return (sulphur::foundation::Vector <sulphur::engine::TransformComponent>) The list of siblings
      * @remarks The self-node is excluded from the list
      */
      foundation::Vector<TransformComponent> GetSiblings();

      /**
      * @brief Returns the current index at which this node occurs in the parents hierarchy
      * @return (size_t) The sibling index
      */
      size_t GetSiblingIndex();

      /**
      * @brief Sets the index at which this node occurs in the parents hierarchy
      * @param[in] index (size_t) The new sibling index
      */
      void SetSiblingIndex(size_t index);

      /**
      * @brief Retrieve a list of child nodes (nodes whose parent is this node)
      * @return (sulphur::foundation::Vector <sulphur::engine::TransformComponent>) The list of children
      */
      foundation::Vector<TransformComponent> GetChildren(bool recursive = false) const;

      /**
      * @brief Returns the amount of children this node has
      * @param[in] recursive (bool) Also add the child count of it's children
      * @return (size_t) The amount of children
      */
      size_t GetChildCount(bool recursive = false) const;

      /**
      * @brief Set the position of this node relative to the root
      * @param[in] position (const glm::vec3&) The vector representing the new position relative to the root node
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldPosition(const glm::vec3& position);
      /**
      * @brief Set the rotation of this node relative to the root
      * @param[in] rotation (const glm::quat&) The quaternion representing the new rotation relative to the root node
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldRotation(const glm::quat& rotation);
      /**
      * @brief Set the scale of this node relative to the root
      * @param[in] scale (const glm::vec3&) The vector representing the new scale relative to the root node
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldScale(const glm::vec3& scale);
      /**
      * @brief Set the rotation of this node relative to the root
      * @param[in] angles (const glm::vec3&) The euler angles representing the new rotation relative to the root node
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent SetWorldRotationEuler(const glm::vec3& angles);
      /**
      * @brief Set the position of this node relative to its parent
      * @param[in] position (const glm::vec3&) The vector representing the new position relative to the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalPosition(const glm::vec3& position);
      /**
      * @brief Set the rotation of this node relative to its parent
      * @param[in] rotation (const glm::quat&) The quaternion representing the new rotation relative to the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalRotation(const glm::quat& rotation);
      /**
      * @brief Set the scale of this node relative to its parent
      * @param[in] scale (const glm::vec3&) The vector representing the new scale relative to the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalScale(const glm::vec3& scale);
      /**
      * @brief Set the position of this node in world space
      * @param[in] angles (const glm::vec3&) The euler angles representing the new rotation relative to the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent SetLocalRotationEuler(const glm::vec3& angles);

      /**
      * @brief Retrieve the position of this node relative to the root
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetWorldPosition();
      /**
      * @brief Retrieve the rotation of this node relative to the root
      * @return (glm::quat) A copy of the resulting quaternion
      */
      glm::quat GetWorldRotation();
      /**
      * @brief Retrieve the scale of this node relative to the root
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetWorldScale();
      /**
      * @brief Retrieve the rotation (represented in euler angles) of this node relative to the root
      * @return (glm::vec3) A copy of the resulting vector
      * @remarks Euler angles are represented in radians
      */
      glm::vec3 GetWorldRotationEuler();
      /**
      * @brief Retrieve the position of this node relative to its parent
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetLocalPosition() const;
      /**
      * @brief Retrieve the rotation of this node relative to its parent
      * @return (glm::quat) A copy of the resulting quaternion
      */
      glm::quat GetLocalRotation() const;
      /**
      * @brief Retrieve the scale of this node relative to its parent
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetLocalScale() const;
      /**
      * @brief Retrieve the rotation (represented in euler angles) of this node relative to its parent
      * @return (glm::vec3) A copy of the resulting vector
      * @remarks Euler angles are represented in radians
      */
      glm::vec3 GetLocalRotationEuler() const;

      /**
      * @brief Set the rotation of this node based on a new up direction in the root node's coordinate space
      * @param[in] up (const glm::vec3&) The new up in the coordinate space of the root node
      * @param[in] right (glm::vec3) A reference right vector in the root's coordinates space
      * @remarks Recalculates the right and forward vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldUp(const glm::vec3& up, glm::vec3 right = glm::vec3(1.f, 0.f, 0.f));
      /**
      * @brief Set the rotation of this node by providing a new right direction in the root node's coordinate space
      * @param[in] right (const glm::vec3&) The new right vector in the coordinate space of the root node
      * @param[in] forward (glm::vec3) A reference forward vector in the coordinate space of the root node
      * @remarks Recalculates the forward and up vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldRight(const glm::vec3& right, glm::vec3 forward = glm::vec3(0.f, 0.f, 1.f));
      /**
      * @brief Set the rotation of this node based on a new forward direction in the root node's coordinate space
      * @param[in] forward (const glm::vec3&) The new forward vector in the coordinate space of the root node
      * @param[in] up (glm::vec3) A reference up vector in the coordinate space of the root node
      * @remarks Recalculates the up and right vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetWorldForward(const glm::vec3& forward, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));
      /**
      * @brief Set the rotation of this node based on a new up direction in the node's parent's coordinate space
      * @param[in] up (const glm::vec3&) The new up vector in the coordinate space of the node's parent
      * @param[in] right (glm::vec3) A reference right vector in the coordinate space of the node's parent
      * @remarks Recalculates the right and forward vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalUp(const glm::vec3& up, glm::vec3 right = glm::vec3(1.f, 0.f, 0.f));
      /**
      * @brief Set the rotation of this node based on a new forward direction in the node's parent's coordinate space
      * @param[in] right (const glm::vec3&) The new forward vector in the coordinate space of the node's parent
      * @param[in] forward (glm::vec3) A reference up vector in the coordinate space of the node's parent
      * @remarks Recalculates the forward and up vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalRight(const glm::vec3& right, glm::vec3 forward = glm::vec3(0.f, 0.f, 1.f));
      /**
      * @brief Set the rotation of this node based on a new forward direction in the node's parent's coordinate space
      * @param[in] forward (const glm::vec3&) The new forward vector in the coordinate space of the node's parent
      * @param[in] up (glm::vec3) A reference up vector in the coordinate space of the node's parent
      * @remarks Recalculates the up and right vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent SetLocalForward(const glm::vec3& forward, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));

      /**
      * @brief Retrieve the normalized vector pointing in the up direction in the coordinate space of the root node
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetWorldUp();

      /**
      * @brief Retrieve the normalized vector pointing in the right direction in the coordinate space of the root node
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetWorldRight();
      /**
      * @brief Retrieve the normalized vector pointing in the forward direction in the coordinate space of the root node
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetWorldForward();
      /**
      * @brief Retrieve the normalized vector pointing in the up direction in the coordinate space of the node's parent
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetLocalUp() const;
      /**
      * @brief Retrieve the normalized vector pointing in the right direction in the coordinate space of the node's parent
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetLocalRight() const;
      /**
      * @brief Retrieve the normalized vector pointing in the forward direction in the coordinate space of the node's parent
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 GetLocalForward() const;

      /**
      * @brief Offset the node's position in the coordinate space of the root node
      * @param[in] offset (const glm::vec3&) The offset to add to the current position, both in the coordinate space of the root
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent TranslateWorld(const glm::vec3& offset);
      /**
      * @brief Rotate the node in the coordinate space of the root node
      * @param[in] rotation (const glm::quat&) The rotation to multiply the current rotation with, both in the coordinate space of the root
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent RotateWorld(const glm::quat& rotation);
      /**
      * @brief Multiply the node's scale in the coordinate space of the root node
      * @param[in] scale (const glm::vec3&) The scale with which to multiply the current scale, both in the coordinate space of the root
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent ScaleWorld(const glm::vec3& scale);
      /**
      * @brief Rotate the node in the coordinate space of the root node
      * @param[in] angles (const glm::vec3&) The rotation in euler angles to multiply the current rotation with, both in the coordinate space of the root
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateWorldEuler(const glm::vec3& angles);
      /**
      * @brief Offset the node's position in the coordinate space of the node's parent
      * @param[in] offset (const glm::vec3&) The offset to add to the current position, both in the coordinate space of the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent TranslateLocal(const glm::vec3& offset);
      /**
      * @brief Rotate the node in the coordinate space of the node's parent
      * @param[in] rotation (const glm::quat&) The rotation to multiply the current rotation with, both in the coordinate space of the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent RotateLocal(const glm::quat& rotation);
      /**
      * @brief Multiply the node's scale in the coordinate space of the node's parent
      * @param[in] scale (const glm::vec3&) The scale with which to multiply the current scale, both in the coordinate space of the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent ScaleLocal(const glm::vec3& scale);
      /**
      * @brief Rotate the node in the coordinate space of the node's parent
      * @param[in] angles (const glm::vec3&) The rotation in euler angles to multiply the current rotation with, both in the coordinate space of the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateLocalEuler(const glm::vec3& angles);

      /**
      * @brief Rotate the node along the right orthonormal basis vector of the root node's coordinate system
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateWorldX(float angle);
      /**
      * @brief Rotate the node along the up orthonormal basis vector of the root node's coordinate system
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateWorldY(float angle);
      /**
      * @brief Rotate the node along the forward orthonormal basis vector of the root node's coordinate system
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateWorldZ(float angle);
      /**
      * @brief Rotate the node around a center-point
      * @param[in] center (const glm::vec3&) The center-point in the coordinate space of the root node
      * @param[in] axis (const glm::vec3&) The axis to rotate around in the coordinate space of the root node
      * @param[in] angle (float) The rotation in euler angles by which to rotate around the axis
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateAroundWorld(const glm::vec3& center, const glm::vec3& axis, float angle);
      /**
      * @brief Rotate the node around a center-point
      * @param[in] center (const glm::vec3&) The center-point in the coordinate space of the root node
      * @param[in] rotation (const glm::quat&) The rotation by which to rotate around the center-point
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateAroundWorld(const glm::vec3& center, const glm::quat& rotation);
      /**
      * @brief Rotate the node along the right orthonormal basis vector of the coordinate system of the node's parent
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateLocalX(float angle);
      /**
      * @brief Rotate the node along the up orthonormal basis vector of the coordinate system of the node's parent
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateLocalY(float angle);
      /**
      * @brief Rotate the node along the forward orthonormal basis vector of the coordinate system of the node's parent
      * @param[in] angle (float) The rotation in euler angles
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateLocalZ(float angle);
      /**
      * @brief Rotate the node around a center-point
      * @param[in] center (const glm::vec3&) The center-point in the coordinate space of the node's parent
      * @param[in] axis (const glm::vec3&) The axis to rotate around in the coordinate space of the node's parent
      * @param[in] angle (float) The rotation in euler angles by which to rotate around the axis
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateAroundLocal(const glm::vec3& center, const glm::vec3& axis, float angle);
      /**
      * @brief Rotate the node around a center-point
      * @param[in] center (const glm::vec3&) The center-point in the coordinate space of the node's parent
      * @param[in] rotation (const glm::quat&) The rotation by which to rotate around the node's parent
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      * @remarks Euler angles are represented in radians
      */
      TransformComponent RotateAroundLocal(const glm::vec3& center, const glm::quat& rotation);

      /**
      * @brief Transform (scale, rotate, translate) a point from the coordinate system of the node to that of the root node
      * @param[in] point (const glm::vec3&) The point to transform
      * @return (glm::vec3) A copy of the resulting point
      */
      glm::vec3 TransformPoint(const glm::vec3& point);
      /**
      * @brief Transform (rotate) a vector from the coordinate system of the node to that of the root node
      * @param[in] vector (const glm::vec3&) The vector to transform
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 TransformVector(const glm::vec3& vector);
      /**
      * @brief Transform (scale, rotate, translate) a point from the coordinate system of the node to that of its parent
      * @param[in] point (const glm::vec3&) The point to transform
      * @return (glm::vec3) A copy of the resulting point
      */
      glm::vec3 TransformLocalPoint(const glm::vec3& point) const;
      /**
      * @brief Transform (scale, rotate, translate) a point from the coordinate system of the node to that of its parent
      * @param[in] vector (const glm::vec3&) The vector to transform
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 TransformLocalVector(const glm::vec3& vector) const;
      /**
      * @brief Transform (scale, rotate, translate) a point from the coordinate system of the root node to that of the node
      * @param[in] point (const glm::vec3&) The point to transform
      * @return (glm::vec3) A copy of the resulting point
      */
      glm::vec3 InverseTransformPoint(const glm::vec3& point);
      /**
      * @brief Transform (rotate) a vector from the coordinate system of the root node to that of the node
      * @param[in] vector (const glm::vec3&) The vector to transform
      * @return (glm::vec3) A copy of the resulting vector
      */
      glm::vec3 InverseTransformVector(const glm::vec3& vector);

      /**
      * @brief Apply the transformation (scale, rotation, translation) of another node to this node
      * @param[in] other (sulphur::engine::TransformComponent) The transform component that is the applied transformation
      * @todo Still has to be implemented
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent Concatenate(TransformComponent other);
      /**
      * @brief Partially apply the transformation (scale, rotation, translation) of another node to this node
      * @param[in] target (sulphur::engine::TransformComponent) The transform component that is the target transformation
      * @param[in] alpha (float) A value between 0 and 1 where 1 results in the node being in the position of the target transformation
      * @todo Still has to be implemented
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent Blend(TransformComponent target, float alpha);
      /**
      * @brief Set the rotation of this node by setting its forward vector to point at a target position
      * @param[in] target (const glm::vec3&) The target position in the coordinate space of the root node
      * @param[in] up (glm::vec3) A reference up vector in the coordinate space of the root node
      * @remarks Recalculates the up and right vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent LookAt(const glm::vec3& target, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));
      /**
      * @brief Set the rotation of this node by setting its forward vector to point at a target position
      * @param[in] target (const glm::vec3&) The target position in the coordinate space of the node's parent
      * @param[in] up (glm::vec3) A reference up vector in the coordinate space of the node's parent
      * @remarks Recalculates the up and right vectors to ensure orthonormality
      * @return (sulphur::engine::TransformComponent) The caller node (for chaining)
      */
      TransformComponent LookAtLocal(const glm::vec3& target, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));

      /*
      * @brief Get the sorting layer attached to this transform
      * @return (sulphur::engine::SortingLayer) The sorting layer
      * @see sulphur::engine::SortingLayer
      */
      SortingLayer GetSortingLayer() const;

      /*
      * @brief Set the sorting layer this transform should use
      * @param[in] sorting_layer (const sulphur::engine::SortingLayer&) The sorting layer
      * @see sulphur::engine::SortingLayer
      */
      void SetSortingLayer(const SortingLayer& sorting_layer);

    private:
      TransformSystem* system_;

    };

    /**
    * @class sulphur::engine::TransformSystem : public sulphur::engine::IComponentSystem<sulphur::engine::TransformComponent>
    * @brief Manages the internal data and lifetime for all instances of sulphur::engine::TransformComponent
    * @author Maarten ten Velden
    */
    class TransformSystem : public IComponentSystem<TransformComponent>
    {
    public:
      /**
      * @brief Default constructor
      */
      TransformSystem();

      /**
      * @see sulphur::engine::IComponentSystem< sulphur::engine::TransformComponent >::Create
      */
      TransformComponent Create(Entity entity) override;
      /**
      * @see sulphur::engine::IComponentSystem <sulphur::engine::TransformComponent>::Destroy
      */
      void Destroy(TransformComponent handle) override;

      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      void OnUpdate(float delta) override;
      
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @see sulphur::engine::TransformComponent::GetLocal
      * @remarks If any transformion data of this node has changed since last access, the cached matrices will be re-calculated.
      */
      glm::mat4 GetLocal(TransformComponent handle) const;
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @see sulphur::engine::TransformComponent::GetLocalToWorld
      * @remarks If any transformion data of this node or one if its parents has changed since last access, the cached matrices will be re-calculated.
      */
      glm::mat4 GetLocalToWorld(TransformComponent handle);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @see sulphur::engine::TransformComponent::GetWorldToLocal
      * @remarks If any transformion data of this node or one if its parents has changed since last access, the cached matrices will be re-calculated.
      */
      glm::mat4 GetWorldToLocal(TransformComponent handle);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] parent (sulphur::engine::TransformComponent)
      * @see sulphur::engine::TransformComponent::SetParent
      */
      void SetParent(TransformComponent handle, TransformComponent parent);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @see sulphur::engine::TransformComponent::UnsetParent
      */
      void UnsetParent(TransformComponent handle);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @see sulphur::engine::TransformComponent::HasParent
      */
      bool HasParent(TransformComponent handle) const;

      /**
      * @brief Test if the transform has changed
      * @return (bool) True if the transform has changed
      */
      bool HasChanged(TransformComponent handle) const;

      /**
      * @brief Gets the top most node of this transform
      * @param[in] child_node (const TransformComponent&) The child node of which to get the root node from
      * @returns (TransformComponent)
      * @see sulphur::engine::TransformComponent::GetRoot
      */
      TransformComponent GetRoot(const TransformComponent& child_node);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (sulphur::engine::TransformComponent)
      * @see sulphur::engine::TransformComponent::GetParent
      */
      TransformComponent GetParent(TransformComponent handle) const;

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (sulphur::foundation::Vector <sulphur::engine::TransformComponent>
      * @see sulphur::engine::TransformComponent::GetSiblings
      */
      foundation::Vector<TransformComponent> GetSiblings(TransformComponent handle);

      /**
      * @brief Returns the current index at which this node occurs in the parents hierarchy
      * @return (size_t) The sibling index
      */
      size_t GetSiblingIndex(TransformComponent handle);

      /**
      * @brief Sets the index at which this node occurs in the parents hierarchy
      * @param[in] index (size_t) The new sibling index
      */
      void SetSiblingIndex(TransformComponent handle, size_t index);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (sulphur::foundation::Vector <sulphur::engine::TransformComponent>
      * @see sulphur::engine::TransformComponent::GetChildren
      */
      foundation::Vector<TransformComponent> GetChildren(TransformComponent handle, bool recursive = false);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] recursive (bool)
      * @return (size_t)
      * @see sulphur::engine::TransformComponent::GetChildCount
      */
      size_t GetChildCount(TransformComponent handle, bool recursive = false);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] child (sulphur::engine::TransformComponent)
      * @see sulphur::engine::TransformComponent::AttachChild
      */
      void AttachChild(TransformComponent handle, TransformComponent child);
      
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] child (sulphur::engine::TransformComponent)
      * @see sulphur::engine::TransformComponent::SetLocalPosition
      */
      void SetLocalPosition(TransformComponent handle, const glm::vec3& position);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] rotation (sulphur::engine::TransformComponent)
      * @see sulphur::engine::TransformComponent::SetLocalRotation
      */
      void SetLocalRotation(TransformComponent handle, const glm::quat& rotation);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @param[in] scale (const glm::vec3&)
      * @see sulphur::engine::TransformComponent::SetLocalScale
      */
      void SetLocalScale(TransformComponent handle, const glm::vec3& scale);

      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetWorldPosition
      */
      glm::vec3 GetWorldPosition(TransformComponent handle);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetWorldRotation
      */
      glm::quat GetWorldRotation(TransformComponent handle);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetWorldScale
      */
      glm::vec3 GetWorldScale(TransformComponent handle);
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetLocalPosition
      */
      glm::vec3 GetLocalPosition(TransformComponent handle) const;
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetLocalRotation
      */
      glm::quat GetLocalRotation(TransformComponent handle) const;
      /**
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      * @return (glm::vec3)
      * @see sulphur::engine::TransformComponent::GetLocalScale
      */
      glm::vec3 GetLocalScale(TransformComponent handle) const;

      /*
      * @brief Get the sorting layer attached to this transform
      * @param[in] handle (TransformComponent) The transform handle to get the value from
      * @return (sulphur::engine::SortingLayer) The sorting layer
      * @see sulphur::engine::SortingLayer
      */
      SortingLayer GetSortingLayer(TransformComponent handle) const;

      /*
      * @brief Set the sorting layer this transform should use
      * @param[in] handle (TransformComponent) The transform handle to set the value on
      * @param[in] sorting_layer (const sulphur::engine::SortingLayer&) The sorting layer
      * @see sulphur::engine::SortingLayer
      */
      void SetSortingLayer(TransformComponent handle, const SortingLayer& sorting_layer);
      
      // Make public so we can access the data in other classes using const ref
      struct TransformData
      {
        /// Disable copying to prevent accidental mistakes
        TransformData() = default;
        TransformData(const TransformData&) = delete;
        TransformData(TransformData&&) = default;
        TransformData& operator=(TransformData&&) = default;

        TransformComponent parent;
        size_t child_count;

        /// Cached values (don't edit directly)
        glm::vec3 cached_local_position;
        glm::quat cached_local_rotation;
        glm::vec3 cached_local_scale;
        glm::vec3 cached_world_position;
        glm::quat cached_world_rotation;
        glm::vec3 cached_world_scale;

        glm::mat4 local_to_parent;
        glm::mat4 cached_local_to_world;
        glm::mat4 cached_world_to_local;

        int flags; //!< Any of the sulphur::engine::DirtyFlags or'ed together

        bool changed;

        foundation::String name;

        Entity entity;
        SortingLayer sorting_layer;
      };

      /**
      * @brief Returns a constant reference to the raw transform data for quick iteration
      * @remarks The order of the data is the order of the hierarchy, to know the indent recursively loop using child_count
      * @return (const foundation::Vector<TransformData>&) The raw transform data vector
      */
      const foundation::Vector<TransformData>& raw_transforms() const { return *data_; }

    private:
      /**
      * @brief An enumerator for marking the parts of a component that require recalculating
      */
      enum struct DirtyFlags : int
      {
        kClean = 0, //!< Default state, nothing needs to be rebuilt
        kLocal = 1 << 0, //!< The node's local transformation components have changed
        kParent = 1 << 1, //!< The node has received a new parent
      };
      /**
      * @brief An enumerator to indicate which components of a node require recalculating
      */
      enum struct CleanFlags : int
      {
        kNone = 0, //!< Nothing will be recalculated
        kLocal = 1 << 0,//!< Local transformation data will be recalculated
        kWorld = 1 << 1, //!< All transformation data and that of any of the node's parents will be recalculated
      };
      
      /**
      * @brief Calculates the child count starting at root node
      * @param[in] root_node (foundation::Vector<TransformData>::iterator&) Pointer to the node for which to calculte the child count
      * @param[out] out_child_count (size_t) The amount of children this root node has
      * @remarks This is excluding the root node
      */
      void CalculateChildCount(foundation::Vector<TransformData>::iterator& root_node, size_t& out_child_count);

      /**
      * @brief Finds the top-most node of this child
      * @param[in] child_node (TransformData&) Pointer to the child node to find the root for
      * @return The top-most node of this child
      */
      TransformData& FindRootNode(TransformData& child_node);

      foundation::Vector<SparseHandle> sparse_array_; //!< @todo Delegate to specialized class
      foundation::Vector<DenseHandle> dense_to_sparse_array_; //!< @Todo Delegate to specialized class
      foundation::Resource<foundation::Vector<TransformData>> data_ = foundation::Resource<foundation::Vector<TransformData>>("TransformData"); //!< @todo Delegate to specialized class
      static TransformComponent root_; //!< A handle-representation of the root node

      /**
      * @brief Rebuild the data of a transform node
      * @param[in] data (sulphur::engine::TransformData&) The internal data to clean
      * @param[in] flag (sulphur::engine::CleanFlags) Any of the sulphur::engine::CleanFlags
      * @remarks If sulphur::engine::CleanFlags::kWorld is specified the cleaning process will clean the node's parents as well
      */
      void CleanIfDirty(TransformData& data, CleanFlags flag);
      /**
      * @brief Look up the internal data of associated with a component handle
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      */
      const TransformData& LookUpData(TransformComponent handle) const
      {
        assert(handle != root_ && "Attempted to access/modify the root node");
        assert(handle.Handle() < sparse_array_.size());
        assert(sparse_array_[handle.Handle()].handle < data_->size());
        return (*data_)[sparse_array_[handle.Handle()].handle];
      }
      /**
      * @brief Look up the internal data of associated with a component handle
      * @param[in] handle (sulphur::engine::TransformComponent) The component of which to look-up the data
      */
      TransformData& LookUpData(TransformComponent handle)
      {
        return const_cast<TransformData&>(
          static_cast<const TransformSystem*>(this)->LookUpData(handle));
      }

      /**
      * @brief Moves transform data between `first` and `last` offset spaces
      * @param[in] first_it (size_t) The index of the first item to move
      * @param[in] last_it (size_t) The index of the last item to move
      * @param[in] offset (int) The amount of spaces to move the data
      * @remarks Fills gap with the data that was already there so no data is lost
      */
      void MoveData(size_t first_it, size_t last_it, int offset);
    };

    //-------------------------------------------------------------------------
    inline glm::mat4 TransformSystem::GetLocal(TransformComponent handle) const
    {
      return LookUpData(handle).local_to_parent;
    }
    inline glm::mat4 TransformSystem::GetLocalToWorld(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);
      return data.cached_local_to_world;
    }
    inline glm::mat4 TransformSystem::GetWorldToLocal(TransformComponent handle)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);

      const bool scale_zero =
        data.cached_world_scale.x == 0.0f ||
        data.cached_world_scale.y == 0.0f ||
        data.cached_world_scale.z == 0.0f;
      PS_LOG_IF(scale_zero, Warning,
        "Accessed world-to-local matrix of an object with an effective scale of 0");

      return data.cached_world_to_local;
    }

    //-------------------------------------------------------------------------
    inline bool TransformSystem::HasParent(TransformComponent handle) const
    {
      return LookUpData(handle).parent != root_;
    }
    inline TransformComponent TransformSystem::GetParent(TransformComponent handle) const
    {
      return LookUpData(handle).parent;
    }

    //-------------------------------------------------------------------------
    inline bool TransformSystem::HasChanged(TransformComponent handle) const
    {
      return LookUpData(handle).changed;
    }

    ///------------------------------------------------------------------------
    inline size_t TransformSystem::GetChildCount(TransformComponent handle, bool recursive)
    {
      if (recursive)
      {
        size_t index = sparse_array_[handle.Handle()].handle;

        TransformData* begin = data_->begin() + index;
        size_t child_count = 0;
        CalculateChildCount(begin, child_count);

        return child_count;
      }
      else
      {
        return LookUpData(handle).child_count;
      }
    }

    //-------------------------------------------------------------------------
    inline void TransformSystem::AttachChild(TransformComponent handle, TransformComponent child)
    {
      SetParent(child, handle);
    }

    //-------------------------------------------------------------------------
    inline void TransformSystem::SetLocalPosition(TransformComponent handle, const glm::vec3& position)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);

      data.local_to_parent = glm::translate(glm::mat4(1.0f), position);
      data.local_to_parent *= glm::mat4_cast(data.cached_local_rotation);
      data.local_to_parent = glm::scale(data.local_to_parent, data.cached_local_scale);

      data.flags |= static_cast<int>(DirtyFlags::kLocal);
      data.changed = true;


      size_t parent_index = sparse_array_[handle.Handle()].handle;
      size_t child_count = GetChildCount(handle, true);

      TransformData* begin = data_->begin() + parent_index;
      TransformData* end = begin + child_count;

      for (TransformData* it = begin; it != end; ++it)
      {
        it->flags |= static_cast<int>(DirtyFlags::kLocal);
        it->changed = true;
      }
    }
    inline void TransformSystem::SetLocalRotation(TransformComponent handle, const glm::quat& rotation)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);

      data.local_to_parent = glm::translate(glm::mat4(1.0f), data.cached_local_position);
      data.local_to_parent *= glm::mat4_cast(rotation);
      data.local_to_parent = glm::scale(data.local_to_parent, data.cached_local_scale);
      
      data.flags |= static_cast<int>(DirtyFlags::kLocal);
      data.changed = true;

      size_t parent_index = sparse_array_[handle.Handle()].handle;
      size_t child_count = GetChildCount(handle, true);

      TransformData* begin = data_->begin() + parent_index;
      TransformData* end = begin + child_count;

      for (TransformData* it = begin; it != end; ++it)
      {
        it->flags |= static_cast<int>(DirtyFlags::kLocal);
        it->changed = true;
      }
    }
    inline void TransformSystem::SetLocalScale(TransformComponent handle, const glm::vec3& scale)
    {
      TransformData& data = LookUpData(handle);
      CleanIfDirty(data, CleanFlags::kWorld);

      data.local_to_parent = glm::translate(glm::mat4(1.0f), data.cached_local_position);
      data.local_to_parent *= glm::mat4_cast(data.cached_local_rotation);
      data.local_to_parent = glm::scale(data.local_to_parent, scale);

      data.flags |= static_cast<int>(DirtyFlags::kLocal);
      data.changed = true;

      size_t parent_index = sparse_array_[handle.Handle()].handle;
      size_t child_count = GetChildCount(handle, true);

      TransformData* begin = data_->begin() + parent_index;
      TransformData* end = begin + child_count;

      for (TransformData* it = begin; it != end; ++it)
      {
        it->flags |= static_cast<int>(DirtyFlags::kLocal);
        it->changed = true;
      }
    }

    //-------------------------------------------------------------------------
    inline glm::vec3 TransformSystem::GetLocalPosition(TransformComponent handle) const
    {
      return LookUpData(handle).cached_local_position;
    }
    inline glm::quat TransformSystem::GetLocalRotation(TransformComponent handle) const
    {
      return LookUpData(handle).cached_local_rotation;
    }
    inline glm::vec3 TransformSystem::GetLocalScale(TransformComponent handle) const
    {
      return LookUpData(handle).cached_local_scale;
    }
  }
}
