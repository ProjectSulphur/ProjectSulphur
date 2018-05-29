#pragma once

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @struct sulphur::foundation::Ray
    * @brief Used for raycasting/raytracing, casts a ray from an origin with a specific direction
    * @author Daniel Konings, Stan Pepels
    */
    struct Ray
    {
      /**
      * @brief Default constructor
      */
      Ray();

      /**
      * @brief Construct with an origin and direction
      * @param[in] origin (const glm::vec3&) The origin of the ray
      * @param[in] direction (const glm::vec3&) The direction of the ray
      */
      Ray(const glm::vec3& origin, const glm::vec3& direction);

      /**
      * @brief Creates a ray that originates at "from" and has a direction towards "to"
      * @param[in] from (const glm::vec3&) The origin position
      * @param[in] to (const glm::vec3&) The position to look towards,
      * determining the direction of the ray
      * @return (sulphur::foundation::Ray) The created ray
      */
      static Ray FromTo(const glm::vec3& from, const glm::vec3& to);

      /**
      *@brief Calculates the closest distance from this ray to another ray.
      *@param[in] other (const sulpur::engine::Ray&) Second ray to check against.
      *@param[out] t (float&) Out value that can be used to recalculate the point on this ray that has the smallest distance to the other ray.
      *@param[out] t_other (float&) Out value that can be used to recalculate the point on the other ray that has the smallest distance to this ray.
      *@return (float) Shortest distance between this ray and another ray.
      */
      float ClosedDistanceToRay(const Ray& other, float& t, float& t_other) const;

      /**
      *@brief Checks if this ray intersects a plane.
      *@param[in] normal (const glm::vec3&) Normal of the plane.
      *@param[in] p0 (const glm::vec3&) Point lying on the plane.
      *@param[out] t (float&) Out value that can be used to recompute the intersection point with the plane.
      *@return (bool) True if ray intersects the plane. False otherwise.
      */
      bool IntersectPlane(const glm::vec3& normal, const glm::vec3 &p0, float& t) const;

      /**
      *@brief Checks if this ray intersects an axis aligned bounding box.
      *@param[in] min_point (const glm::vec3&) Minimum describing the AABB.
      *@param[in] max_point (const glm::vec3&) Maximum describing the AABB.
      *@param[out] t_min (float&) Out value that can be used to recalculate the intersection point with the AABB that is closest to the ray origin.
      *@param[out] t_max (float&) Out value that can be used to recalculate the intersection point with the AABB that is furthest from the ray origin.
      *@return (bool) True if ray intersects the AABB. False otherwise.
      */
      bool IntersectAABB(
        const glm::vec3& min_point,
        const glm::vec3& max_point, float& t_min, float& t_max) const;

      /**
      *@brief Gets the closest distance from a point to a ray.
      *@param[in] point (const glm::vec3&) Point to check against.
      *@param[out] t (float&) Out value that can be used to recalculate the closest point on the ray.
      *@return (float) Shortest distance from ray to point.
      */
      float ClosestDistanceToPoint(const glm::vec3& point, float& t) const;

      /**
      *@brief Checks if this ray intersects with a sphere.
      *@param[in] position (const glm::vec3&) Position of the sphere.
      *@param[in] radius (float) Radius of the sphere.
      *@param[out] t_min (float&) Out value that can be used to recalculate the intersection point with the sphere that is closest to the ray origin.
      *@param[out] t_max (float&) Out value that can be used to recalculate the intersection point with the sphere that is furthest to the ray origin.
      */
      bool IntersectSphere(const glm::vec3& position, float radius, float& t_min, float& t_max) const;

      glm::vec3 origin; //!< The origin of the ray
      glm::vec3 direction; //!< The direction of the ray
    };
  }
}