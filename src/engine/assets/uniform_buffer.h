#pragma once

#include "engine/assets/asset_interfaces.h"

#include <foundation/utils/type_definitions.h>
#include <foundation/logging/logger.h>
#include <foundation/containers/map.h>
#include <foundation/containers/string.h>

#include <glm/glm.hpp>

namespace sulphur
{
  namespace foundation
  {
    class ShaderData;
    class ShaderResource;
    struct Color;
  }
}

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @enum ShaderVarType
    * @brief Describes a type of variable
    * @see sulphur::engine::ShaderVariable
    */
    enum struct ShaderVarType
    {
      kUnknown,
      kBool,
      kInt,
      kUint,
      kUint8,
      kDouble,
      kFloat,
      kVec2,
      kVec3,
      kVec4,
      kMat3,
      kMat4
    };

    /**
    * @struct ShaderVariable
    * @brief A shader variable that stores a name, type and a pointer to its data
    * @author Jelle de Haan
    */
    struct ShaderVariable
    {
    friend class UniformBuffer;
    private:

      /**
      * @internal
      * @brief Checks if the type of T matches the type of this shader variable
      * @param[in] val (const T&) The type to check
      * @return (bool) True if the type is equal
      */
      template<typename T>
      inline constexpr const bool TypeCheck(const T& val) const
      {
        if (eastl::is_same<T, bool  >::value) { return type_ == ShaderVarType::kBool; }
        if (eastl::is_same<T, uint  >::value) { return type_ == ShaderVarType::kUint; }
        if (eastl::is_same<T, byte  >::value) { return type_ == ShaderVarType::kUint8; }
        if (eastl::is_same<T, int   >::value) { return type_ == ShaderVarType::kInt; }
        if (eastl::is_same<T, float >::value) { return type_ == ShaderVarType::kFloat; }
        if (eastl::is_same<T, double>::value) { return type_ == ShaderVarType::kDouble; }

        if (eastl::is_same<T, glm::ivec2>::value) { return type_ == ShaderVarType::kVec2; }
        if (eastl::is_same<T, glm::ivec3>::value) { return type_ == ShaderVarType::kVec3; }
        if (eastl::is_same<T, glm::ivec4>::value) { return type_ == ShaderVarType::kVec4; }

        if (eastl::is_same<T, glm::uvec2>::value) { return type_ == ShaderVarType::kVec2; }
        if (eastl::is_same<T, glm::uvec3>::value) { return type_ == ShaderVarType::kVec3; }
        if (eastl::is_same<T, glm::uvec4>::value) { return type_ == ShaderVarType::kVec4; }

        if (eastl::is_same<T, glm::vec2>::value) { return type_ == ShaderVarType::kVec2; }
        if (eastl::is_same<T, glm::vec3>::value) { return type_ == ShaderVarType::kVec3; }
        if (eastl::is_same<T, glm::vec4>::value) { return type_ == ShaderVarType::kVec4; }

        if (eastl::is_same<T, foundation::Color>::value) { return type_ == ShaderVarType::kVec4; }
        if (eastl::is_same<T, glm::mat3>::value) { return type_ == ShaderVarType::kMat3; }
        if (eastl::is_same<T, glm::mat4>::value) { return type_ == ShaderVarType::kMat4; }

		    return false;
      };

    public:
      /**
      * @brief Default empty constructor, see the other constructor to create a ShaderVariable
      */
      ShaderVariable();

      /**
      * @brief Creates a shader variable with a certain value type and a default value
      */
      ShaderVariable(ShaderVarType type, void* default_value = nullptr);

      /**
      * @brief Assign this data to the shader variable and perform a type check for type safety
      * @tparam T The type of your data, deduced automatically.
      * @param[in] new_data (const T&) The value to assign
      * @return (ShaderVariable&) The shader variable to assign the new data to
      * @remarks Checks for variable type mismatch in debug.
      */
      template<class T>
      ShaderVariable& operator=(const T& new_data)
      {
#ifdef _DEBUG
        assert(data_ != nullptr);
        if (!TypeCheck(new_data))
        {
          PS_LOG(Warning, "Shader variable type mismatch. Can't edit the data");
          return *this;
        }
#endif

        *reinterpret_cast<T*>(data_) = new_data;
        return *this;
      }

    private:
      ShaderVarType type_; //!< The type of variable this ShaderVarible contains
      void* data_; //!< A pointer to the raw data use to modify this variable inside of a uniform buffer
    };

    /**
    * @class UniformBuffer
    * @brief An interface for communicating with a uniform buffer
    * @author Jelle de Haan
    */
    class UniformBuffer
    {
    public:
      using MapPair = eastl::pair<const foundation::String, size_t>;
      using MapIterator = eastl::rbtree_iterator<MapPair, MapPair*, MapPair&>;
      using ConstMapIterator = eastl::rbtree_iterator<MapPair, const MapPair*, const MapPair&>;

      /**
      * @brief Constructs an empty uniform buffer, use the other constructor to create one from a shader
      */
      UniformBuffer();

      /**
      * @brief Constructs a new UniformBuffer, allocating a data buffer matching the shader data.
      * @param[in] shader_data (foundation::ShaderData&) The shader reflector data
      */
      UniformBuffer(foundation::ShaderData& shader_data);

      /**
      * @brief De-constructs the buffer, releasing its data buffer.
      */
      ~UniformBuffer();

      UniformBuffer(const UniformBuffer& other); //!< Copy constructor
      UniformBuffer& operator=(const UniformBuffer& other); //!< Copy assignment operator
      UniformBuffer(UniformBuffer&& other); //!< Move constructor
      UniformBuffer& operator=(UniformBuffer&& other); //!< Move assignment operator

      /**
      * @internal
      * @brief Returns a pointer to the raw data buffer. (Should only be used by the renderers)
      * @return (const void*) The const data pointer
      */
      const void* data() const { return data_; };

      /**
      * @internal
      * @brief Returns the size of the raw data buffer. (Should only be used by the renderers)
      * @return (const size_t) The data size
      */
      const size_t size() const { return size_; };

      /**
      * @brief Finds the shader variable by its name. ( Ex.: var.Find("lights[0].direction"); )
      * @param[in] variable_name (const foundation::String&) The name of the shader variable
      * @return (ShaderVariable*) The shader variable or null if not found
      * @see sulphur::engine::UniformBuffer::operator[]
      * @see sulphur::engine::ShaderVariable
      */
      ShaderVariable* Find(const foundation::String& variable_name)
      {
        return (*this)[variable_name];
      }

      /**
      * @brief Finds the shader variable by its name. ( Ex.: var.Find("lights[0].direction"); )
      * @param[in] variable_name (const foundation::String&) The name of the shader variable
      * @return (const ShaderVariable*) The read-only shader variable or null if not found
      * @see sulphur::engine::UniformBuffer::operator[]
      * @see sulphur::engine::ShaderVariable
      */
      const ShaderVariable* Find(const foundation::String& variable_name) const
      {
        return (*this)[variable_name];
      }

      /**
      * @brief Finds the shader variable by its name. ( Ex.: var["lights[0].direction"]; )
      * @param[in] variable_name (const foundation::String&) The name of the shader variable
      * @return (ShaderVariable*) The shader variable or null if not found
      * @see sulphur::engine::ShaderVariable
      */
      ShaderVariable* operator[](const foundation::String& variable_name)
      {
        MapIterator it = name_map_.find(variable_name);
        assert(it != name_map_.end()); // The variable name you are using does not exist
        return &data_map_[it->second];
      }

      /**
      * @brief Finds the shader variable by its name. ( Ex.: var["lights[0].direction"]; )
      * @param[in] variable_name (const foundation::String&) The name of the shader variable
      * @return (const ShaderVariable*) The read-only shader variable or null if not found
      * @see sulphur::engine::ShaderVariable
      */
      const ShaderVariable* operator[](const foundation::String& variable_name) const
      {
        ConstMapIterator it = name_map_.find(variable_name);
        assert(it != name_map_.end()); // The variable name you are using does not exist
        return &data_map_[it->second];
      }

    private:
      size_t size_; //<! The raw byte size of data_
      void* data_; //<! A pointer to the allocated buffer data
      
      foundation::Map<foundation::String, size_t> name_map_; //<! Links name to data_map_ index
      foundation::Vector<ShaderVariable> data_map_; //<! Raw vector of all shader variables


      /**
      * @brief Constructs the data maps of this uniform buffer using the shader reflector
      * @param[in] name_prefix (const foundation::String&) The prefix to put in front of any newly created name
      * @param[in] shader_reflection_data (const foundation::Vector <foundation::ShaderResource>&) The reflection data used to create the data maps
      */
      void ConstructDataMaps(const foundation::String& name_prefix, 
        const foundation::Vector<foundation::ShaderResource>& shader_reflection_data);

      /**
      * @brief Constructs the link from that shader variable to the raw data buffer
      */
      void ConstructDataLink();

    };
  }
}
