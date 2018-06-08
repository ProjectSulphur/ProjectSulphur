#include "uniform_buffer.h"

#include <foundation/pipeline-assets/shader.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    inline size_t GetTypeSize(const ShaderVarType type)
    {
      switch (type)
      {
      case ShaderVarType::kBool: return sizeof(bool);
      case ShaderVarType::kDouble: return sizeof(double);
      case ShaderVarType::kFloat: return sizeof(float);
      case ShaderVarType::kInt: return sizeof(int32_t);
      case ShaderVarType::kUint: return sizeof(uint32_t);
      case ShaderVarType::kUint8: return sizeof(uint8_t);
      case ShaderVarType::kVec2: return sizeof(glm::vec2);
      case ShaderVarType::kVec3: return sizeof(glm::vec3);
      case ShaderVarType::kVec4: return sizeof(glm::vec4);
      case ShaderVarType::kMat3: return sizeof(glm::mat3);
      case ShaderVarType::kMat4: return sizeof(glm::mat4);
      default: assert(false && "unknown type"); return 0u;
      }
    }

    //--------------------------------------------------------------------------------
    ShaderVarType ConvertVariableType(foundation::ShaderResource::ConcreteType type)
    {
      switch (type)
      {
      case foundation::ShaderResource::ConcreteType::kBool:
        return ShaderVarType::kBool;
      case foundation::ShaderResource::ConcreteType::kChar:
        return ShaderVarType::kUint8;
      case foundation::ShaderResource::ConcreteType::kDouble:
        return ShaderVarType::kDouble;
      case foundation::ShaderResource::ConcreteType::kFloat:
        return ShaderVarType::kFloat;
      case foundation::ShaderResource::ConcreteType::kInt:
        return ShaderVarType::kInt;
      case foundation::ShaderResource::ConcreteType::kMat3x3:
        return ShaderVarType::kMat3;
      case foundation::ShaderResource::ConcreteType::kMat4x4:
        return ShaderVarType::kMat4;
      case foundation::ShaderResource::ConcreteType::kUInt:
        return ShaderVarType::kUint;
      case foundation::ShaderResource::ConcreteType::kVec2:
        return ShaderVarType::kVec2;
      case foundation::ShaderResource::ConcreteType::kVec3:
        return ShaderVarType::kVec3;
      case foundation::ShaderResource::ConcreteType::kVec4:
        return ShaderVarType::kVec4;
      default:
        return ShaderVarType::kUnknown;
      }
    }

    ShaderVariable::ShaderVariable() :
      type_(ShaderVarType::kUnknown),
      data_(nullptr)
    {}

    ShaderVariable::ShaderVariable(ShaderVarType type, void* default_value) :
      type_(type),
      data_(default_value)
    {}

    void UniformBuffer::ConstructDataMaps(const foundation::String& name_prefix,
      const foundation::Vector<foundation::ShaderResource>& shader_reflection_data)
    {
      for (const foundation::ShaderResource& buffer : shader_reflection_data)
      {
        // TODO:
        // properly handle structs
        // properly handle arrays

        if (buffer.concrete_type == foundation::ShaderResource::ConcreteType::kStruct)
        {
          ConstructDataMaps(name_prefix + buffer.name + ".", buffer.members);
          return;
        }

        ShaderVarType var_type = ConvertVariableType(buffer.concrete_type);

        name_map_.emplace(eastl::make_pair(
          buffer.name, data_map_.size()));
        data_map_.emplace_back(var_type);
        
        size_ += GetTypeSize(var_type);
      }
    };

    //--------------------------------------------------------------------------------
    UniformBuffer::UniformBuffer() :
      size_(0u),
      data_(nullptr)
    {}

    //--------------------------------------------------------------------------------
    UniformBuffer::UniformBuffer(foundation::ShaderData& shader_data) :
      size_(0u),
      data_(nullptr)
    {
      // Construct data / name maps
      ConstructDataMaps("", shader_data.uniform_buffers);

      if (size_ > 0)
      {
        data_ = foundation::Memory::Allocate(size_);
      }

      // fill data with reflector

      ConstructDataLink();
    }

    //--------------------------------------------------------------------------------
    UniformBuffer::~UniformBuffer()
    {
      if (data_ != nullptr)
      {
        foundation::Memory::Deallocate(data_);
        data_ = nullptr;
      }
    }

    //--------------------------------------------------------------------------------
    UniformBuffer::UniformBuffer(const UniformBuffer& other) :
      size_(other.size_),
      data_(nullptr)
    {
      // Copy data over to new buffer
      if (size_ > 0)
      {
        data_ = foundation::Memory::Allocate(size_);
        memcpy(data_, other.data_, size_);
      }
    }

    //--------------------------------------------------------------------------------
    UniformBuffer& UniformBuffer::operator=(const UniformBuffer& other)
    {
      if (this == &other)
      {
        return *this;
      }

      if (data_ != nullptr) // free existing memory
      {
        foundation::Memory::Deallocate(data_);
        data_ = nullptr;
      }

      size_ = other.size_;
      // Allocate some more data and copy the data over to a new place
      if (size_ > 0)
      {
        data_ = foundation::Memory::Allocate(size_);
        memcpy(data_, other.data_, size_);
      }
      return *this;
    }

    //--------------------------------------------------------------------------------
    UniformBuffer::UniformBuffer(UniformBuffer&& other) :
      size_(other.size_),
      data_(other.data_)
    {
      other.data_ = nullptr;
      other.size_ = 0u;
    }

    //--------------------------------------------------------------------------------
    UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other)
    {
      if (this != &other)
      {
        return *this;
      }

      if (data_ != nullptr) // free existing memory
      {
        foundation::Memory::Deallocate(data_);
        data_ = nullptr;
      }

      data_ = other.data_;
      size_ = other.size_;

      other.data_ = nullptr;
      other.size_ = 0u;

      return *this;
    }

    //--------------------------------------------------------------------------------
    void UniformBuffer::ConstructDataLink()
    {
      if (data_ == nullptr)
      {
        return;
      }

      char* data = reinterpret_cast<char*>(data_);
      size_t offset = 0u;

      for (ShaderVariable& var : data_map_)
      {
        switch (var.type_)
        {
        case ShaderVarType::kBool:
          *reinterpret_cast<bool*>(data + offset) = var.data_ ? *reinterpret_cast<bool*>(var.data_) : false;
          var.data_ = (data + offset);
          offset += sizeof(bool);
          break;
        case ShaderVarType::kDouble:
          *reinterpret_cast<double*>(data + offset) = var.data_ ? *reinterpret_cast<double*>(var.data_) : 0.0;
          var.data_ = (data + offset);
          offset += sizeof(double);
          break;
        case ShaderVarType::kFloat:
          *reinterpret_cast<float*>(data + offset) = var.data_ ? *reinterpret_cast<float*>(var.data_) : 0.0f;
          var.data_ = (data + offset);
          offset += sizeof(float);
          break;
        case ShaderVarType::kInt:
          *reinterpret_cast<int32_t*>(data + offset) = var.data_ ? *reinterpret_cast<int32_t*>(var.data_) : 0;
          var.data_ = (data + offset);
          offset += sizeof(int32_t);
          break;
        case ShaderVarType::kUint:
          *reinterpret_cast<uint32_t*>(data + offset) = var.data_ ? *reinterpret_cast<uint32_t*>(var.data_) : 0u;
          var.data_ = (data + offset);
          offset += sizeof(uint32_t);
          break;
        case ShaderVarType::kUint8:
          *reinterpret_cast<uint8_t*>(data + offset) = var.data_ ? *reinterpret_cast<uint8_t*>(var.data_) : 0u;
          var.data_ = (data + offset);
          offset += sizeof(uint8_t);
          break;
        case ShaderVarType::kVec2:
          *reinterpret_cast<glm::vec2*>(data + offset) = var.data_ ? *reinterpret_cast<glm::vec2*>(var.data_) : glm::vec2{};
          var.data_ = (data + offset);
          offset += sizeof(glm::vec2);
          break;
        case ShaderVarType::kVec3:
          *reinterpret_cast<glm::vec3*>(data + offset) = var.data_ ? *reinterpret_cast<glm::vec3*>(var.data_) : glm::vec3{};
          var.data_ = (data + offset);
          offset += sizeof(glm::vec3);
          break;
        case ShaderVarType::kVec4:
          *reinterpret_cast<glm::vec4*>(data + offset) = var.data_ ? *reinterpret_cast<glm::vec4*>(var.data_) : glm::vec4{};
          var.data_ = (data + offset);
          offset += sizeof(glm::vec4);
          break;
        case ShaderVarType::kMat3:
          *reinterpret_cast<glm::mat3*>(data + offset) = var.data_ ? *reinterpret_cast<glm::mat3*>(var.data_) : glm::mat3{};
          var.data_ = (data + offset);
          offset += sizeof(glm::mat3);
          break;
        case ShaderVarType::kMat4:
          *reinterpret_cast<glm::mat4*>(data + offset) = var.data_ ? *reinterpret_cast<glm::mat4*>(var.data_) : glm::mat4{};
          var.data_ = (data + offset);
          offset += sizeof(glm::mat4);
          break;
        default:
          assert(false && "unknown type");
          break;
        }
      }
    }
  }
}