#pragma once
#include "foundation/containers/vector.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace sulphur
{
  namespace foundation
  {
    struct AABB;

    /**
    * @struct sulphur::foundation::Sphere
    * @brief Describes a sphere.
    * @author Timo van Hees
    */
    struct Sphere
    {
      glm::vec3 center; //!< The center of the sphere.
      float radius;     //!< The radius of the sphere.

      /**
       * @brief Default constructor.
       */
      Sphere();
      /**
       * @brief Constructor. Creates a sphere from a center position and a radius.
       * @param[in] center (const glm::vec3&) The center of the sphere.
       * @param[in] radius (float) The radius of the sphere.
       */
      Sphere(const glm::vec3& center, float radius);

      /**
       * @brief Addition operator. Combines the spheres.
       */
      Sphere operator+(const Sphere& rhs) const;
      /**
      * @brief Addition assignment operator. Combines the spheres.
      */
      void operator+=(const Sphere& rhs);

      /**
       * @brief Calculates a sphere that encapsulates both spheres.
       * @param[in] s1 (const sulphur::foundation::Sphere&) sphere one.
       * @param[in] s2 (const sulphur::foundation::Sphere&) sphere two.
       * @return (sulphur::foundation::Sphere) The combined sphere.
       */
      static Sphere Combine(const Sphere& s1, const Sphere& s2);

      /**
       * @brief Calculates the bounding sphere of a set of points.
       * @param[in] points (const sulphur::foundation::Vector <glm::vec3>&) Set of points.
       * @return (sulphur::foundation::Sphere) The bounding sphere.
       */
      static Sphere EncapsulatePoints(const Vector<glm::vec3>& points);

      /**
       * @brief Transforms the sphere by a matrix.
       * @param[in] matrix (const glm::mat4&) The matrix.
       * @return (sulphur::foundation::Sphere) Transformed sphere.
       */
      Sphere Transform(const glm::mat4& matrix) const;

      /**
      * @brief Transforms the sphere by translation and scale vectors.
      * @param[in] translation (const glm::vec3&) The translation.
      * @param[in] scale (const glm::vec3&) The scale.
      * @return (sulphur::foundation::Sphere) Transformed sphere.
      */
      Sphere Transform(const glm::vec3& translation, const glm::vec3& scale) const;

      /**
      * @brief Creates a box that encapsulates this sphere.
      * @return (sulphur::foundation::AABB) The box.
      */
      AABB ToBox() const;
    };

    /**
    * @struct sulphur::foundation::AABB
    * @brief Describes an axis aligned box.
    * @author Timo van Hees
    */
    struct AABB
    {
      glm::vec3 min;  //!< The minimum corner of the box.
      glm::vec3 max;  //!< The maximum corner of the box.

      /**
       * @brief Default constructor.
       */
      AABB();
      /**
       * @brief Constructor. Create AABB from min and max positions.
       * @param[in] min (const glm::vec3&) The minumum corner of the box.
       * @param[in] max (const glm::vec3&) The maximum corner of the box.
       */
      AABB(const glm::vec3& min, const glm::vec3& max);

      /**
      * @brief Addition operator. Combines the boxes.
      */
      AABB operator+(const AABB& rhs) const;
      /**
      * @brief Addition assignment operator. Combines the boxes.
      */
      void operator+=(const AABB& rhs);

      /**
       * @brief Calculates an AABB that encapsulates both boxes.
       * @param b1 (const sulphur::foundation::AABB&) box one.
       * @param b2 (const sulphur::foundation::AABB&) box two.
       * @return (sulphur::foundation::AABB) The combined box.
       */
      static AABB Combine(const AABB& b1, const AABB& b2);

      /**
      * @brief Calculates the bounding box of a set of points.
      * @param[in] points (const sulphur::foundation::Vector <glm::vec3>&) Set of points.
      * @return (sulphur::foundation::AABB) The bounding box.
      */
      static AABB EncapsulatePoints(const Vector<glm::vec3>& points);

      /**
      * @brief Transforms the box by a matrix.
      * @param[in] matrix (const glm::mat4&) The matrix.
      * @return (sulphur::foundation::AABB) Transformed box.
      */
      AABB Transform(const glm::mat4& matrix) const;

      /**
      * @brief Transforms the box by translation and matrix.
      * @param[in] translation (const glm::vec3&) The translation.
      * @param[in] matrix (const glm::mat3&) The rotation and scale matrix.
      * @return (sulphur::foundation::AABB) Transformed box.
      */
      AABB Transform(const glm::vec3& translation, const glm::mat3& matrix) const;

      /**
       * @brief Creates a sphere that encapsulates this box.
       * @return (sulphur::foundation::Sphere) The sphere.
       */
      Sphere ToSphere() const;
    };

    /**
     * @struct sulphur::foundation::Plane
     * @brief Describes a plane.
     * @author Timo van Hees
     */
    struct Plane
    {
      glm::vec3 normal;  //!< The normal of the plane.
      float distance;    //!< The distance from the origin to the plane.

      /**
       * @brief Default contructor.
       * @remark Creates a plane at y=0, facing up.
       */
      Plane();
      /**
       * @brief Constructor. Creates a plane from a normal and a point.
       * @param[in] normal (const glm::vec3&) The normal of the plane.
       * @param[in] point (const glm::vec3&) A point in that lies on the plane.
       */
      Plane(const glm::vec3& normal, const glm::vec3& point);
      /**
       * @brief Constructor. Create a plane from a normal and a distance.
       * @param normal (const glm::vec3&) The normal of the plane.
       * @param distance (float) The distance from the origin.
       */
      Plane(const glm::vec3& normal, float distance);

      /**
      * @brief Sets the plane from a normal and a point.
      * @param[in] normal (const glm::vec3&) The normal of the plane.
      * @param[in] point (const glm::vec3&) A point in that lies on the plane.
      */
      void Set(const glm::vec3& normal, const glm::vec3& point);
      /**
      * @brief Sets the plane from a normal and a distance.
      * @param normal (const glm::vec3&) The normal of the plane.
      * @param distance (float) The distance from the origin.
      */
      void Set(const glm::vec3& normal, float distance);

      /**
       * @brief Calculates the distance from the plane to a point.
       * @param[in] point (const glm::vec3&) A point.
       * @return (float) Signed distance to the plane.
       */
      float DistanceToPoint(const glm::vec3& point) const;
    };

    /**
     * @class sulphur::foundation::Frustum
     * @brief Describes a view frustum.
     * @author Timo van Hees
     */
    class Frustum
    {
    public:
      /**
       * @brief Default constructor. 
       */
      Frustum();
      /**
       * @brief Creates a frustum setting only the camera properties.
       * @param[in] fov (float) The fov of the camera.
       * @param[in] aspect_ratio (float) The aspect ratio of the camera.
       * @param[in] near_z (float) The distance of the near plane of the camera.
       * @param[in] far_z (float) The distance of the far plane of the camera.
       */
      Frustum(float fov, float aspect_ratio, float near_z, float far_z);
      /**
      * @brief Creates a frustum.
      * @param[in] fov (float) The fov of the camera.
      * @param[in] aspect_ratio (float) The aspect ratio of the camera.
      * @param[in] near_z (float) The distance of the near plane of the camera.
      * @param[in] far_z (float) The distance of the far plane of the camera.
      * @param[in] position (const glm::vec3&) The position of the camera.
      * @param[in] look_at (const glm::vec3&) The point the camera is looking at.
      * @param[in] up (const glm::vec3&) The up vector of the camera.
      */
      Frustum(float fov, float aspect_ratio, float near_z, float far_z, 
        const glm::vec3& position, const glm::vec3& look_at, const glm::vec3& up);

      /**
      * @brief Sets the camera properties.
      * @param[in] fov (float) The fov of the camera.
      * @param[in] aspect_ratio (float) The aspect ratio of the camera.
      * @param[in] near_z (float) The distance of the near plane of the camera.
      * @param[in] far_z (float) The distance of the far plane of the camera.
      * @remark Does not update the frustum until MoveCamera is called.
      */
      void SetCameraProperties(float fov, float aspect_ratio, float near_z, float far_z);
      /**
       * @brief Moves the view frustum.
       * @param[in] position (const glm::vec3&) The position of the camera.
       * @param[in] look_at (const glm::vec3&) The point the camera is looking at.
       * @param[in] up (const glm::vec3&) The up vector of the camera.
       * @remark Camera properties need to be set first.
       */
      void MoveCamera(const glm::vec3& position, const glm::vec3& look_at, const glm::vec3& up);

      /**
       * @brief Tests if a point is inside the frustum.
       * @param[in] point (const glm::vec3&) A point.
       * @return (bool) True if the point is inside the frustum.
       */
      bool Intersects(const glm::vec3& point) const;
      /**
      * @brief Tests if a box is inside the frustum.
      * @param[in] box (const sulphur::foundation::AABB&) A box.
      * @return (bool) True if the box is inside the frustum.
      */
      bool Intersects(const AABB& box) const;
      /**
      * @brief Tests if a sphere is inside the frustum.
      * @param[in] sphere (const sulphur::foundation::Sphere&) A sphere.
      * @return (bool) True if the sphere is inside the frustum.
      */
      bool Intersects(const Sphere& sphere) const;

    private:
      enum Planes { kTop, kBottom, kLeft, kRight, kNear, kFar };  //!< Enum to index into the planes array
      Plane planes_[6];   //!< The frustum planes
      float near_z_;      //!< The distance of the near plane
      float far_z_;       //!< The distance of the far plane
      float near_height_; //!< The calculated height of the near plane
      float near_width_;  //!< The calculated width of the near plane
      float far_height_;  //!< The calculated height of the far plane
      float far_width_;   //!< The calculated width of the far plane
    };
  }
}
