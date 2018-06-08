#include "foundation/utils/shapes.h"

#include <glm/gtx/norm.hpp>

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------------
    Sphere::Sphere() :
      center(0.0f),
      radius(0.0f)
    {
    }

    //--------------------------------------------------------------------------------
    Sphere::Sphere(const glm::vec3& center, float radius) :
      center(center),
      radius(radius)
    {
    }

    //--------------------------------------------------------------------------------
    Sphere Sphere::operator+(const Sphere& rhs) const
    {
      return Combine(*this, rhs);
    }

    //--------------------------------------------------------------------------------
    void Sphere::operator+=(const Sphere& rhs)
    {
      *this = Combine(*this, rhs);
    }

    //--------------------------------------------------------------------------------
    Sphere Sphere::Combine(const Sphere& s1, const Sphere& s2)
    {
      const float center_distance = glm::distance(s1.center, s2.center);

      if (center_distance + s2.radius <= s1.radius)
      {
        return s1;
      }

      if (center_distance + s1.radius <= s2.radius)
      {
        return s2;
      }

      const float new_radius = (s1.radius + s2.radius + center_distance) * 0.5f;
      const glm::vec3 new_center = s1.center + (s2.center - s1.center) *
        (new_radius - s1.radius) / center_distance;

      Sphere result = {};
      result.center = new_center;
      result.radius = new_radius;

      return result;
    }

    //--------------------------------------------------------------------------------
    Sphere Sphere::EncapsulatePoints(const Vector<glm::vec3>& points)
    {
      Sphere sphere = {};

      if (points.empty() == true)
      {
        return sphere;
      }

      if (points.size() == 1)
      {
        sphere.center = points[0];
        return sphere;
      }

      if (points.size() == 2)
      {
        sphere.center = (points[0] + points[1]) * 0.5f;
        sphere.radius = glm::distance(points[0], points[1]) * 0.5f;
        return sphere;
      }

      const glm::vec3 a = points[0]; // Initial point
      glm::vec3 b = a; // Point furthest from a

      for (int i = 0; i < points.size(); ++i)
      {
        const glm::vec3& v = points[i];

        if (glm::distance2(a, b) < glm::distance2(a, v))
        {
          b = v;
        }
      }

      glm::vec3 c = b; // Point furthest from b

      for (int i = 0; i < points.size(); ++i)
      {
        const glm::vec3& v = points[i];

        if (glm::distance2(b, c) < glm::distance2(b, v))
        {
          c = v;
        }
      }

      sphere.center = (b + c) * 0.5f;
      sphere.radius = glm::distance(b, c) * 0.5f;
      return sphere;
    }

    //--------------------------------------------------------------------------------
    Sphere Sphere::Transform(const glm::mat4& matrix) const
    {
      Sphere result;
      result.center = matrix * glm::vec4(1.0f, 1.0f, 1.0f, 0.0f); //!< Uses center for temp storage
      result.radius = glm::max(glm::max(result.center.x, result.center.y), 
        result.center.z) * radius;
      result.center = glm::vec3(matrix * glm::vec4(center, 1.0f));
      return result;
    }

    //--------------------------------------------------------------------------------
    Sphere Sphere::Transform(const glm::vec3& translation, const glm::vec3& scale) const
    {
      Sphere result;
      result.center += translation;
      result.radius = glm::max(glm::max(scale.x, scale.y), scale.z) * radius;
      return result;
    }

    //--------------------------------------------------------------------------------
    AABB Sphere::ToBox() const
    {
      AABB box = {};
      box.min = center - glm::vec3(radius);
      box.max = center + glm::vec3(radius);
      return box;
    }

    //--------------------------------------------------------------------------------
    AABB::AABB() :
      min(0.0f),
      max(0.0f)
    {
    }

    //--------------------------------------------------------------------------------
    AABB::AABB(const glm::vec3& min, const glm::vec3& max) :
      min(min),
      max(max)
    {
    }

    //--------------------------------------------------------------------------------
    AABB AABB::operator+(const AABB& rhs) const
    {
      return Combine(*this, rhs);
    }

    //--------------------------------------------------------------------------------
    void AABB::operator+=(const AABB& rhs)
    {
      *this = Combine(*this, rhs);
    }

    //--------------------------------------------------------------------------------
    AABB AABB::Combine(const AABB& b1, const AABB& b2)
    {
      AABB result = {};
      result.min = glm::min(b1.min, b2.min);
      result.max = glm::max(b1.max, b2.max);
      return result;
    }

    //--------------------------------------------------------------------------------
    AABB AABB::EncapsulatePoints(const Vector<glm::vec3>& points)
    {
      AABB box = {};
      if (points.empty() == true)
      {
        return box;
      }

      box.min = points[0];
      box.max = points[0];

      for (int i = 1; i < points.size(); ++i)
      {
        box.min = glm::min(box.min, points[i]);
        box.max = glm::max(box.max, points[i]);
      }

      return box;
    }

    //--------------------------------------------------------------------------------
    AABB AABB::Transform(const glm::mat4& matrix) const
    {
      AABB result;
      const glm::vec3 temp = glm::vec3(matrix * glm::vec4(min, 1.0f));
      result.max = glm::vec3(matrix * glm::vec4(max, 1.0f));
      result.min = glm::min(temp, result.max);
      result.max = glm::max(temp, result.max);
      return result;
    }

    //--------------------------------------------------------------------------------
    AABB AABB::Transform(const glm::vec3& translation, const glm::mat3& matrix) const
    {
      AABB result;
      const glm::vec3 temp = matrix * min;
      result.max = matrix * max;
      result.min = glm::min(temp, result.max);
      result.max = glm::max(temp, result.max);
      result.min += translation;
      result.max += translation;
      return result;
    }

    //--------------------------------------------------------------------------------
    Sphere AABB::ToSphere() const
    {
      Sphere sphere = {};
      sphere.center = (min + max) * 0.5f;
      sphere.radius = glm::distance(sphere.center, max);
      return sphere;
    }

    //--------------------------------------------------------------------------------
    Plane::Plane()
    {
      Set(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));
    }

    //--------------------------------------------------------------------------------
    Plane::Plane(const glm::vec3& new_normal, const glm::vec3& point)
    {
      Set(new_normal, point);
    }

    //--------------------------------------------------------------------------------
    Plane::Plane(const glm::vec3& new_normal, float new_distance)
    {
      Set(new_normal, new_distance);
    }

    //--------------------------------------------------------------------------------
    void Plane::Set(const glm::vec3& new_normal, const glm::vec3& point)
    {
      normal = glm::normalize(new_normal);
      distance = -glm::dot(normal, point);
    }

    //--------------------------------------------------------------------------------
    void Plane::Set(const glm::vec3& new_normal, float new_distance)
    {
      normal = new_normal;
      distance = new_distance;
    }

    //--------------------------------------------------------------------------------
    float Plane::DistanceToPoint(const glm::vec3& point) const
    {
      return distance + glm::dot(normal, point);
    }

    //--------------------------------------------------------------------------------
    Frustum::Frustum() :
      near_z_(0),
      far_z_(0),
      near_height_(0),
      near_width_(0),
      far_height_(0),
      far_width_(0)
    {
    }

    //--------------------------------------------------------------------------------
    Frustum::Frustum(float fov, float aspect_ratio, float near_z, float far_z)
    {
      SetCameraProperties(fov, aspect_ratio, near_z, far_z);
    }

    //--------------------------------------------------------------------------------
    Frustum::Frustum(float fov, float aspect_ratio, float near_z, float far_z,
      const glm::vec3& position, const glm::vec3& look_at, const glm::vec3& up)
    {
      SetCameraProperties(fov, aspect_ratio, near_z, far_z);
      MoveCamera(position, look_at, up);
    }

    //--------------------------------------------------------------------------------
    void Frustum::SetCameraProperties(float fov, float aspect_ratio,
      float near_z, float far_z)
    {
      near_z_ = near_z;
      far_z_ = far_z;

      const float tang = static_cast<float>(glm::tan(glm::radians(fov * 0.5f)));
      near_height_ = near_z * tang;
      near_width_ = near_height_ * aspect_ratio;
      far_height_ = far_z  * tang;
      far_width_ = far_height_ * aspect_ratio;
    }

    //--------------------------------------------------------------------------------
    void Frustum::MoveCamera(const glm::vec3& position, const glm::vec3& look_at, 
      const glm::vec3& up)
    {
      const glm::vec3 z = glm::normalize(look_at - position);
      const glm::vec3 x = glm::normalize(glm::cross(up, z));
      const glm::vec3 y = glm::cross(z, x);
      const glm::vec3 near_center = position + z * near_z_;
      const glm::vec3 far_center = position + z * far_z_;

      planes_[kNear] = Plane(z, near_center);
      planes_[kFar] = Plane(-z, far_center);

      glm::vec3 normal = glm::normalize(near_center + y * near_height_ - position);
      normal = glm::normalize(glm::cross(x, normal));
      planes_[kTop] = Plane(normal, near_center + y * near_height_);

      normal = glm::normalize(near_center - y * near_height_ - position);
      normal = glm::normalize(glm::cross(normal, x));
      planes_[kBottom] = Plane(normal, near_center - y * near_height_);

      normal = glm::normalize(near_center - x * near_width_ - position);
      normal = glm::normalize(glm::cross(y, normal));
      planes_[kLeft] = Plane(normal, near_center - x * near_width_);

      normal = glm::normalize(near_center + x * near_width_ - position);
      normal = glm::normalize(glm::cross(normal, y));
      planes_[kRight] = Plane(normal, near_center + x * near_width_);
    }

    //--------------------------------------------------------------------------------
    bool Frustum::Intersects(const glm::vec3& p) const
    {
      for (int i = 0; i < 6; ++i)
      {
        if (planes_[i].DistanceToPoint(p) < 0.0f)
        {
          return false;
        }
      }
      return true;
    }

    //--------------------------------------------------------------------------------
    bool Frustum::Intersects(const AABB& box) const
    {
      glm::vec3 p = {};
      for (int i = 0; i < 6; ++i)
      {
        const glm::vec3& normal = planes_[i].normal;
        p = box.min;
        if (normal.x > 0)
        {
          p.x = box.max.x;
        }
        if (normal.y > 0)
        {
          p.y = box.max.y;
        }
        if (normal.z > 0)
        {
          p.z = box.max.z;
        }

        if (planes_[i].DistanceToPoint(p) < 0.0f)
        {
          return false;
        }
      }
      return true;
    }

    //--------------------------------------------------------------------------------
    bool Frustum::Intersects(const Sphere& sphere) const
    {
      float distance = 0.0f;
      for (int i = 0; i < 6; ++i)
      {
        distance = planes_[i].DistanceToPoint(sphere.center);
        if (distance < -sphere.radius)
        {
          return false;
        }
      }
      return true;
    }
  }
}
