#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinearMath/btVector3.h>
#include <LinearMath/btMatrixX.h>
#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletConversions
    * @brief A collection of functions for converting between Glm and Bullet math.
    */
    class BulletConversions
    {
      public:
      /**
      * @brief Converts a Bullet vector3 to the GLM equivalent.
      * @param[in] vector (const btVector3&) Vector to be converted.
      * @return (glm::vec3) The converted vector.
      */
      static glm::vec3 ToGlm(const btVector3& v);

      /**
      * @brief Converts a Bullet vector4 to the GLM equivalent.
      * @param[in] vector (const btVector4&) Vector to be converted.
      * @return (glm::vec4) The converted vector.
      */
      static glm::vec4 ToGlm(const btVector4& v);

      /**
      * @brief Converts a Bullet 3x3 matrix to the GLM equivalent.
      * @param[in] matrix (const btMatrix3x3&) Matrix to be converted.
      * @return (glm::mat3x3) The converted matrix.
      */
      static glm::mat3x3 ToGlm(const btMatrix3x3& mat);

      /**
      * @brief Converts a Bullet quaternion to the GLM equivalent.
      * @param[in] quat (const btQuaternion&) Quaternion to be converted.
      * @return (glm::quat) The converted quaternion.
      */
      static glm::quat ToGlm(const btQuaternion& quat);

      /**
      * @brief Converts a btTransform to a glm::mat4x4
      * @param[in] transform (const btTransform&)
      * @return (glm::mat4x4)
      */
      static glm::mat4x4 ToGlm(const btTransform& transform);

      /**
      * @brief Converts a GLM vector3 to the Bullet equivalent.
      * @param[in] vector (const glm::vec3&) Vector to be converted.
      * @return (btVector3) The converted vector.
      */
      static btVector3 ToBt(const glm::vec3& v);

      /**
      * @brief Converts a GLM vector4 to the Bullet equivalent.
      * @param[in] vector (const glm::vec4&) Vector to be converted.
      * @return (btVector4) The converted vector.
      */
      static btVector4 ToBt(const glm::vec4& v);
      
      /**
      * @brief Converts a GLM 3x3 matrix to the Bullet equivalent.
      * @param[in] matrix (const glm::mat3x3&) Matrix to be converted.
      * @return (btMatrix3x3) The converted matrix.
      */
      static btMatrix3x3 ToBt(const glm::mat3x3& mat);

      /**
      * @brief Converts a GLM quaternion to the Bullet equivalent.
      * @param[in] quat (const glm::quat&) Quaternion to be converted.
      * @return (btQuaternion) The converted quaternion.
      */
      static btQuaternion ToBt(const glm::quat& quat);

      /**
      * @brief Converts a glm::mat4x4 to a btTransform
      * @param[in] transform (glm::mat4x4)
      * @return (const btTransform&)
      */
      static btTransform ToBt(const glm::mat4x4& transform);
    };
  }
}
