#include "foundation/math/ray.h"
#include "glm/common.hpp"

namespace sulphur
{
  namespace foundation
  {
    //-------------------------------------------------------------------------
    Ray::Ray() :
      origin(glm::vec3{ 0.0f, 0.0f, 0.0f }),
      direction(glm::vec3{ 0.0f, 1.0f, 0.0f })
    {

    }

    //-------------------------------------------------------------------------
    Ray::Ray(const glm::vec3& o, const glm::vec3& d) :
      origin(o),
      direction(d)
    {

    }

    //-------------------------------------------------------------------------
    Ray Ray::FromTo(const glm::vec3& from, const glm::vec3& to)
    {
      Ray ray;
      ray.origin = from;
      ray.direction = glm::normalize(to - from);

      return ray;
    }

    //------------------------------------------------------------------------------------------------------
    float Ray::ClosedDistanceToRay(const Ray& other, float& t, float& t_other) const
    {
      const glm::vec3 dp = other.origin - origin;
      const float v12 = glm::dot(direction, direction);
      const float v22 = glm::dot(other.direction, other.direction);
      const float v1v2 = glm::dot(direction, other.direction);

      const float det = v1v2 * v1v2 - v12 * v22;

      if (std::abs(det) > FLT_MIN)
      {
        const float inv_det = 1.f / det;

        const float dpv1 = glm::dot(dp, direction);
        const float dpv2 = glm::dot(dp, other.direction);

        t = inv_det * (v22 * dpv1 - v1v2 * dpv2);
        t_other = inv_det * (v1v2 * dpv1 - v12 * dpv2);

        return glm::length(dp + other.direction * t_other - direction * t);
      }
      else
      {
        const glm::vec3 a = glm::cross(dp, direction);
        return std::sqrt(dot(a, a) / v12);
      }   
    }

    //------------------------------------------------------------------------------------------------------
    bool Ray::IntersectPlane(const glm::vec3& normal,
      const glm::vec3& p0,
      float& t) const
    {
      glm::vec3 norm = normal;
      float denom = glm::dot(norm, direction);

      // if plane normal is in the opposite direction flip it
      if (denom < FLT_MIN)
      {
        denom = glm::dot(-normal, direction);
        norm = -normal;
      }

      if (denom > FLT_MIN) {
        glm::vec3 p0l0 = p0 - origin;
        t = glm::dot(p0l0, norm) / denom;
        return (t >= 0);
      }
      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool Ray::IntersectAABB(
      const glm::vec3& min_point,
      const glm::vec3& max_point,
      float& t_min,
      float& t_max) const
    {
      glm::vec3 inv_dir = 1.0f / direction;
      glm::vec3 minV = (min_point - origin) * inv_dir;
      glm::vec3 maxV = (max_point - origin) * inv_dir;

      t_min = glm::max(glm::max(glm::min(minV.x, maxV.x),
        glm::min(minV.y, maxV.y)), glm::min(minV.z, maxV.z));

      t_max = glm::min(glm::min(glm::max(minV.x, maxV.x),
        glm::max(minV.y, maxV.y)), glm::max(minV.z, maxV.z));

      return t_max >= t_min;
    }

    //------------------------------------------------------------------------------------------------------
    float Ray::ClosestDistanceToPoint(const glm::vec3& point, float& t) const
    {
      glm::vec3 from_to = point - origin;
      t = glm::dot(from_to, direction);
      return glm::length(point - (origin + direction * t));
    }

    //------------------------------------------------------------------------------------------------------
    bool Ray::IntersectSphere(const glm::vec3& position, float radius, float& t_min, float& t_max) const
    {
      glm::vec3 hypotenuse = position - origin;
      float t_shortest = glm::dot(hypotenuse, direction);
      if (t_shortest < 0.0f)
      {
        return false;
      }

      float a = glm::length(position - (origin + direction * t_shortest));
      if (a > radius)
      {
        return false;
      }

      float c = radius;
      float b = glm::sqrt(c * c - a * a);
      t_min = t_shortest - b;
      t_max = t_shortest + b;
      return true;
    }
  }
}