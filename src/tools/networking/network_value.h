#pragma once
#include <glm/glm.hpp>

namespace sulphur
{
  namespace networking
  {
    const size_t kStringLength = 64; //!<The max size of the networked string

    /**
    * @enum sulphur::networking::NetworkValueType : uint8_t
    * @brief An enumerator of value types that are supported for syncing and rpcs
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
      kVec4, //!< type vector4
      kInt16, //!< type int 16
      kInt8, //!< type int 8
      kUnsignedInt16, //!< type unsigned int 16
      kUnsignedInt8, //!< type unsigned int 8
      kString //!< type string of "kStringLength" chars
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
        int16_t i16; //!< int16 handle in the union
        int8_t i8; //!< int8 handle in the union
        uint16_t ui16; //!< uint16 handle in the union
        uint8_t ui8; //!< uint8 handle in the union
        char str[kStringLength]; //!< char[kStringLength] handle in the union
      };

      /**
      * @brief Get the size of the value in bytes (sizeof)
      * @return (size_t) The size of the value
      */
      size_t GetSize() const
      {
        switch (type)
        {
        case NetworkValueType::kFloat:
          return sizeof(float);
        case NetworkValueType::kDouble:
          return sizeof(double);
        case NetworkValueType::kInt:
          return sizeof(int);
        case NetworkValueType::kUnsignedInt:
          return sizeof(unsigned int);
        case NetworkValueType::kBool:
          return sizeof(bool);
        case NetworkValueType::kVec2:
          return sizeof(glm::vec2);
        case NetworkValueType::kVec3:
          return sizeof(glm::vec3);
        case NetworkValueType::kVec4:
          return sizeof(glm::vec4);
        case NetworkValueType::kInt8:
          return sizeof(int8_t);
        case NetworkValueType::kInt16:
          return sizeof(int16_t);
        case NetworkValueType::kUnsignedInt8:
          return sizeof(uint8_t);
        case NetworkValueType::kUnsignedInt16:
          return sizeof(uint16_t);
        case NetworkValueType::kString:
          return sizeof(char) * strlen(str);
        default:
          return 0;
        }
      }

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
          case NetworkValueType::kInt8:
            return i8 == other.i8;
          case NetworkValueType::kInt16:
            return i16 == other.i16;
          case NetworkValueType::kUnsignedInt8:
            return ui8 == other.ui8;
          case NetworkValueType::kUnsignedInt16:
            return ui16 == other.ui16;
          case NetworkValueType::kString:
            return str == other.str;
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
  }
}