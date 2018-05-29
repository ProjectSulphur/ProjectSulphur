#pragma once

#include "physics/platform_physics_body.h"

#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::PhysicsShape
    * @brief The platform-independent base class for every shape to use
    * @author Daniel Konings, Angelo van der Mark
    */
    class PhysicsShape
    {
    public:

      /**
      * @brief The different shape types that exist
      */
      enum struct ShapeTypes
      {
        kBox, //!< A box shape with extents from the origin.
        kSphere, //!< A spherical shape with a radius.
        kCylinder, //!< A cylinder shape with a radius and height.
        kCapsule, //!< A capsule shape, basically a cylinder with spherically capped ends.
        kCone, //!< A cone shape, with a radius and height.
        kConvex, //!< A convex mesh shape, defined by a set of points.
        kMesh //!< A concave mesh shape, consisting of multiple triangles.
      };

      /**
      * @brief Virtual destructor.
      */
      virtual ~PhysicsShape() = default;

      /**
      * @brief Returns the type of this shape.
      * @return (sulphur::physics::ShapeTypes) The type of this shape.
      */
      virtual ShapeTypes GetType() const = 0;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      virtual void* GetInternalShape() const = 0;

      /**
      * @brief Returns a list of colliders that are currently using this shape.
      * @return (const foundation::Vector<PhysicsCollider*>&) The list of colliders.
      */
      const foundation::Vector<PhysicsCollider*>& instances() const;

      static const float kDefaultExtents; //!< The default extents for various shapes

    protected:
      friend class PhysicsCollider;

      /**
      * @brief Calls sulphur::physics::PhysicsCollider::OnShapeUpdate for all colliders using this shape.
      * Use is intended for communicating changes to this shape, and usage is up to the shape's implementation.
      */
      void UpdateColliders();

      /**
      * @brief Adds a collider instance to the collider list.
      * @param[in] instance (sulphur::physics::PhysicsCollider*) The new instance.
      */
      void AddCollider(PhysicsCollider* instance);

      /**
      * @brief Removes a collider instance from the collider list.
      * @param[in] instance (sulphur::physics::PhysicsCollider*) The instance to remove.
      */
      void RemoveCollider(PhysicsCollider* instance);

      foundation::Vector<PhysicsCollider*> instances_; //!< List of collider instances using this shape.
    };
  }
}