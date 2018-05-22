#pragma once
#include <glm/glm.hpp>

namespace sulphur
{
  namespace networking
  {
    /**
    * @enum sulphur::networking::NetworkValueType
    * @brief An enumerator of value types that are supported for syncing
    * @author Joren Bolhuis
    */
    enum struct NetworkValueType : uint8_t
    {
      kFloat, //!< type float
      kDouble, //!< type double
      kInt, //!< type int
      kUnsignedInt, //!< type unsigned int
      kBool, //!< type bool
      kVec2, //!< type vector2
      kVec3, //!< type vector3
      kVec4 //!< type vector4
    };
    
    /**
    * @class sulphur::networking::NetworkValue
    * @brief network value description
    * @author Joren Bolhuis
    */
    struct NetworkValue
    {
      NetworkValueType type; //!< type specifier
      union
      {
        float f; //!< float handle in the union
        double d; //!< double handle in the union
        int i; //!< int handle in the union
        unsigned int ui; //!< unsigned int handle in the union
        bool b; //!< bool handle in the union
        glm::vec2 v2; //!< vector2 handle in the union
        glm::vec3 v3; //!< vector3 handle in the union
        glm::vec4 v4; //!< vector4 handle in the union
      };

      /**
      * @brief Check if 2 values are the same
      * @param[in] other (const sulphur::networking::NetworkValue& other) The value to compare
      * @return (bool) Whether they are the same
      */
      bool operator==(const NetworkValue& other) 
      {
        if (type == other.type)
        {
          switch (type)
          {
          case NetworkValueType::kFloat:
            return f == other.f;
          case NetworkValueType::kDouble:
            return d == other.d;
          case NetworkValueType::kInt:
            return i == other.i;
          case NetworkValueType::kUnsignedInt:
            return ui == other.ui;
          case NetworkValueType::kBool:
            return b == other.b;
          case NetworkValueType::kVec2:
            return v2 == other.v2;
          case NetworkValueType::kVec3:
            return v3 == other.v3;
          case NetworkValueType::kVec4:
            return v4 == other.v4;
          default:
            return false;
          }
        }
        else
        {
          return false;
        }
      }

      /**
      * @brief Check if 2 values are not the same
      * @param[in] other (const sulphur::networking::NetworkValue& other) The value to compare
      * @return (bool) Whether they are not the same
      */
      bool operator!=(const NetworkValue& other)
      {
        return !(*this == other);
      }
    };

    /**
    * @class sulphur::networking::SyncableNetworkValue
    * @brief syncable network value description
    * @author Joren Bolhuis
    */
    struct SyncableNetworkValue
    {
      void* location; //!< Location of the value we want to sync
      NetworkValue value; //!< A copy of the data including the type, used for comparing
    };
  }
}