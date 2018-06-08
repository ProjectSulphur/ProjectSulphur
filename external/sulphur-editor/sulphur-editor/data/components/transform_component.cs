using System;
using System.Collections.Generic;
using System.Numerics;
using Newtonsoft.Json;

namespace sulphur.editor
{
  /**
   * @class sulphur.editor.TransformComponent
   * @brief Class containing the transform data for a world object.
   * @author Stan Pepels
   */
  [JsonObject(MemberSerialization.OptIn)]
  public class TransformComponent
  {
    /**
     * @brief Constructor initializing the transform with identity matrixes
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     */
    public TransformComponent(WorldObject entity)
    {
      entity_ = entity;
      scale_ = Vector3.One;
      position_ = Vector3.Zero;
      rotation_ = Quaternion.Identity;
    }

    /**
     * @brief Constructor initializing the transform with a position.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The world position of the object.
     */
    public TransformComponent(WorldObject entity, Vector3 position)
    {
      entity_ = entity;
      scale_ = Vector3.One;
      position_ = position;
      rotation_ = Quaternion.Identity;
    }

    /**
     * @brief Constructor initializing the transform with a position and rotation.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The world position of the object.
     * @param[in] rotation (Quaternion) The quaternion representing the world rotation of the object.
     */
    public TransformComponent(WorldObject entity, Vector3 position, Quaternion rotation)
    {
      entity_ = entity;
      scale_ = Vector3.One;
      position_ = position;
      rotation_ = rotation;

    }

    /**
     * @brief Constructor initializing the transform with a position, rotation and scale.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The world position of the object.
     * @param[in] rotation (Quaternion) The quaternion representing the world rotation of the object.
     * @param[in] scale (Vector3) The world scale of the object.
     */
    public TransformComponent(WorldObject entity, Vector3 position, Quaternion rotation, Vector3 scale)
    {
      entity_ = entity;
      scale_ = scale;
      position_ = position;
      rotation_ = rotation;
    }

    /**
     * @brief Constructor initializing the transform with identity matrixes and a parent.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     */
    public TransformComponent(WorldObject entity, TransformComponent parent)
    {
      entity_ = entity;
      parent_ = parent;
      scale_ = parent.scale_;
      position_ = parent.position_;
      rotation_ = parent.rotation_;
    }

    /**
     * @brief Constructor initializing the transform with a position.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The local position of the object.
     */
    public TransformComponent(WorldObject entity, TransformComponent parent, Vector3 position)
    {
      entity_ = entity;
      parent_ = parent;
      scale_ = parent.scale_;
      position_ = parent.position_ + position;
      rotation_ = parent.rotation_;

    }

    /**
     * @brief Constructor initializing the transform with a position,and rotation.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The local position of the object.
     * @param[in] rotation (Quaternion) The quaternion representing the local rotation of the object.
     */
    public TransformComponent(WorldObject entity, TransformComponent parent, Vector3 position, Quaternion rotation)
    {
      entity_ = entity;
      parent_ = parent;
      scale_ = parent.scale_;
      position_ = parent.position_ + position;
      rotation_ = parent.rotation_ * rotation;
    }

    /**
     * @brief Constructor initializing the transform with a position, rotation and scale.
     * @param[in] entity (WorldObject) Entity The transform belongs to.
     * @param[in] position (Vector3) The local position of the object.
     * @param[in] rotation (Quaternion) The quaternion representing the local rotation of the object.
     * @param[in] scale (Vector3) The local scale of the object.
     */
    public TransformComponent(WorldObject entity, TransformComponent parent, Vector3 position, Quaternion rotation, Vector3 scale)
    {
      entity_ = entity;
      parent_ = parent;
      scale_ = parent.scale_ * scale;
      position_ = parent.position_ + position;
      rotation_ = parent.rotation_ * rotation;
    }

    /**
     * Backing value for the position_ property
     */
    private Vector3 position_value_;

    /**
     * Backing value for the local_position_ property
     */
    private Vector3 local_position_value_;

    /**
     * Backing value for the rotation_ property
     */
    private Quaternion rotation_value_;
    /**
     * Backing value for the local_rotation_ property
     */
    private Quaternion local_rotation_value_;
    /**
     * Backing value for the scale_ property
     */
    private Vector3 scale_value_;
    /**
     * Backing value for the local_scale_ property
     */
    private Vector3 local_scale_value_;

    /**
     * @brief Property used to get and set the world position of the transform
     * @remark When this property changes the children will be rebuild.
     */
    [JsonProperty]
    public Vector3 position_
    {
      get { return position_value_; }
      set
      {
        if (parent_ == null)
        {
          local_position_ = value;
        }
        else
        {
          Matrix4x4 world_to_local = parent_.world_to_local_;
          Matrix4x4 local = Matrix4x4.CreateTranslation(value) * world_to_local;
          Vector3 local_position = local.Translation;
          local_position_ = local_position;
        }
      }
    }

    /**
     * @brief Property used to get and set the world rotation of the transform
     * @remark When this property changes the children will be rebuild.
     */
    [JsonProperty]
    public Quaternion rotation_
    {
      get { return local_rotation_value_; }
      set
      {
        if (parent_ == null)
        {
          local_rotation_ = value;
        }
        else
        {
          Quaternion parent_rotaion = parent_.rotation_;
          Quaternion local_rotation = Quaternion.Inverse(parent_rotaion) * value;
          local_rotation_ = local_rotation;
        }
      }
    }

    /**
     * @brief Property used to get and set the world scale of the transform
     * @remark When this property changes the children will be rebuild.
     */
    [JsonProperty]
    public Vector3 scale_
    {
      get { return scale_value_; }
      set
      {
        if (parent_ == null)
        {
          local_scale_ = value;
        }
        else
        {
          Vector3 scale = parent_.scale_;
          Vector3 local_scale = value / scale;
          local_scale_ = local_scale;
        }
      }
    }

    /**
     * @brief Property used to get and set the local position of the transform
     * @remark When this property changes the children will be rebuild.
     */
    public Vector3 local_position_
    {
      get { return local_position_value_; }
      set
      {
        local_to_parent_ = Matrix4x4.CreateScale(local_scale_);
        local_to_parent_ *= Matrix4x4.CreateFromQuaternion(local_rotation_);
        local_to_parent_ *= Matrix4x4.CreateTranslation(value);
        RebuildTransform();
      }
    }

    /**
      * @brief Property used to get and set the local rotation of the transform
      * @remark When this property changes the children will be rebuild.
      */
    public Quaternion local_rotation_
    {
      get { return local_rotation_value_; }
      set
      {
        local_to_parent_ = Matrix4x4.CreateScale(local_scale_);
        local_to_parent_ *= Matrix4x4.CreateFromQuaternion(value);
        local_to_parent_ *= Matrix4x4.CreateTranslation(local_position_);
        RebuildTransform();
      }
    }

    /**
     * @brief Property used to get and set the local scale of the transform
     * @remark When this property changes the children will be rebuild.
     */
    public Vector3 local_scale_
    {
      get { return local_scale_value_; }
      set
      {
        local_to_parent_ = Matrix4x4.CreateScale(value);
        local_to_parent_ *= Matrix4x4.CreateFromQuaternion(local_rotation_);
        local_to_parent_ *= Matrix4x4.CreateTranslation(local_position_);
        RebuildTransform();
      }
    }

    /**
     * @brief Matrix used to trasform a point from parent to local space. can be combined with the parent world heirarchy to get this objects world matrix.
     */
    public Matrix4x4 local_to_parent_ { get; private set; }

    /**
     * @brief Matrix used to transform a point from local space to world space. can also be retrieved by combining the parent world matrix with the parent to local matrix.
     */
    public Matrix4x4 local_to_world_ { get; private set; }

    /**
     * @brief Matrix used to transform from world space to local space.
     */
    public Matrix4x4 world_to_local_ { get; private set; }

    /**
     * @brief The parent transform.
     */
    public TransformComponent parent_ { get; private set; }

    /**
     * @brief The object this transform belongs to
     */
    public WorldObject entity_ { get; private set; }

    /**
     * @brief List of children of this transform
     */
    public List<TransformComponent> children_ { get; private set; } = new List<TransformComponent>();

    /**
     * @brief Get the forward vector of this transform.
     * @return (Vector3) The vector representing the forward direction of the object.
     */
    public Vector3 GetWorldForward()
    {
      return new Vector3(local_to_world_.M31, local_to_world_.M32, local_to_world_.M33);
    }

    /**
    * @brief Get the forward right of this transform.
    * @return (Vector3) The vector representing the right direction of the object.
    */
    public Vector3 GetWorldRight()
    {
      return new Vector3(local_to_world_.M11, local_to_world_.M12, local_to_world_.M13);
    }

    /**
    * @brief Get the up vector of this transform.
    * @return (Vector3) The vector representing the up direction of the object.
    */
    public Vector3 GetWorldUp()
    {
      return new Vector3(local_to_world_.M21, local_to_world_.M22, local_to_world_.M23);
    }

    /**
    * @brief Get the forward vector of this transform.
    * @return (Vector3) The vector representing the forward direction of the object.
    */
    public Vector3 GetLocalForward()
    {
      return new Vector3(local_to_parent_.M31, local_to_parent_.M32, local_to_parent_.M33);
    }

    /**
    * @brief Get the right vector of this transform.
    * @return (Vector3) The vector representing the right direction of the object.
    */
    public Vector3 GetLocalRight()
    {
      return new Vector3(local_to_parent_.M11, local_to_parent_.M12, local_to_parent_.M13);
    }

    /**
    * @brief Get the up vector of this transform.
    * @return (Vector3) The vector representing the up direction of the object.
    */
    public Vector3 GetLocalUp()
    {
      return new Vector3(local_to_parent_.M21, local_to_parent_.M22, local_to_parent_.M23);
    }

    /**
     * @brief Get the world rotation in euler angles.
     * @return (Vector3) The world rotation expressed in euler angles.
     */
    public Vector3 GetWorldRotationEuler()
    {
      return ToEuler(rotation_);
    }

    /**
    * @brief Get the local rotation in euler angles.
    * @return (Vector3) The local rotation expressed in euler angles.
    */
    public Vector3 GetLocalRotationEuler()
    {
      return ToEuler(local_rotation_);
    }

    /**
     * @brief Sets the parent for this transform.
     * @param[in] parent (TransformComponent) The new parent of this transform.
     */
    public void SetParent(TransformComponent parent)
    {
      parent_ = parent;
      local_to_parent_ = parent == null? local_to_world_ : Matrix4x4.Multiply(local_to_world_, parent.world_to_local_);
      RebuildTransform();
    }

    /**
     * @brief Adds a child to this transform
     * @param[in] child (TransformComponent) The child to add.
     */
    public void AttachChild(TransformComponent child)
    {
      child.SetParent(this);
      children_.Add(child);

    }

    /**
     * @brief Removes a child to this transform
     * @param[in] child (TransformComponent) The child to remove.
     */
    public void DetachChild(TransformComponent child)
    {
      child.SetParent(null);
      children_.Remove(child);
    }

    /**
     * @brief Moves the object by adding to the world position.
     * @param[in] translation(Vector3) translation to add.
     */
    public void TranslateWorld(Vector3 translation)
    {
      position_ += translation;
    }

    /**
    * @brief Moves the object by adding to the local position.
    * @param[in] translation(Vector3) translation to add.
    */
    public void TranslateLocal(Vector3 translation)
    {
      local_position_ += translation;
    }

    /**
    * @brief Moves the object by adding to the world rotation.
    * @param[in] rotation(Quaternion) rotation to add.
    */
    public void RotateWorld(Quaternion rotation)
    {
      rotation_ *= rotation;
    }

    /**
   * @brief Moves the object by adding to the local rotation.
   * @param[in] rotation(Quaternion) rotation to add.
   */
    public void RotateLocal(Quaternion rotation)
    {
      local_rotation_ *= rotation;
    }

    /**
   * @brief Moves the object by adding to the world scale.
   * @param[in] scale(Vector3) scale to add.
   */
    public void ScaleWorld(Vector3 scale)
    {
      scale_ *= scale;
    }

    /**
  * @brief Moves the object by adding to the local scale.
  * @param[in] scale(Vector3) scale to add.
  */
    public void ScaleLocal(Vector3 scale)
    {
      local_scale_ *= scale;
    }

    /**
     * @brief Recalculates the matrixes based on the parent world matrix and the local_to_parent matrix.
     */
    public void RebuildTransform()
    {
      if (parent_ == null)
      {
        local_to_world_ = local_to_parent_;
        Matrix4x4 result;
        Matrix4x4.Invert(local_to_world_, out result);
        world_to_local_ = result;
      }
      else
      {
        local_to_world_ = Matrix4x4.Multiply(local_to_parent_, parent_.local_to_world_);
        Matrix4x4 result;
        Matrix4x4.Invert(local_to_world_, out result);
        world_to_local_ = result;
      }

      Matrix4x4.Decompose(
        local_to_world_,
        out scale_value_,
        out rotation_value_,
        out position_value_);

      Matrix4x4.Decompose(
        local_to_parent_,
        out local_scale_value_,
        out local_rotation_value_,
        out local_position_value_);


      foreach (TransformComponent child in children_)
      {
        child.RebuildTransform();
      }
      entity_.is_dirty = true;
    }

    /**
     * @brief Converts a quaternio to its euler angles representation.
     * @param[in] q (Quaternion) Quaternion to convert to euler angles.
     * @return (Vector3) The euler angles representation of quaternion q.
     */
    private Vector3 ToEuler(Quaternion q)
    {
      Vector3 result = new Vector3();
      float qy2 = q.Y * q.Y;
      float qz2 = q.Y * q.Z;
      float qx2 = q.Y * q.X;
      result.X = (float)Math.Atan2(2 * q.Y * q.W - 2 * q.X * q.Z, 1 - 2 * qy2 - 2 * qz2);
      result.Y = (float)Math.Asin(2 * q.X * q.Y + 2 * q.Z * q.W);
      result.Z = (float)Math.Atan2(2 * q.X * q.W - 2 * q.Y * q.Z, 1 - 2 * qx2 - 2 * qz2);
      return result;
    }

  }
}
