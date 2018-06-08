using sulphur.editor;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace sulphur.editor.data
{

  /**
  * @enum ShaderVarType
  * @brief Describes a type of variable
  * @see sulphur::engine::ShaderVariable
  * @remarks This enum is a copy of the data structure from sulphur::engine::ShaderVarType
  */
  public enum ShaderVarType
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
  public class ShaderVariable
  {
    /**
    * @internal
    * @brief Checks if the type of T matches the type of this shader variable
    * @param[in] val (const T&) The type to check
    * @return (bool) True if the type is equal
    */
    private bool TypeCheck(object val)
    {
      if (val.GetType() == typeof(bool)) { return type_ == ShaderVarType.kBool; }
      if (val.GetType() == typeof(uint)) { return type_ == ShaderVarType.kUint; }
      if (val.GetType() == typeof(byte)) { return type_ == ShaderVarType.kUint8; }
      if (val.GetType() == typeof(int)) { return type_ == ShaderVarType.kInt; }
      if (val.GetType() == typeof(float)) { return type_ == ShaderVarType.kFloat; }
      if (val.GetType() == typeof(double)) { return type_ == ShaderVarType.kDouble; }

      //if (val.GetType() == typeof(glm::ivec2)) { return type_ == ShaderVarType.kVec2; }
      //if (val.GetType() == typeof(glm::ivec3)) { return type_ == ShaderVarType.kVec3; }
      //if (val.GetType() == typeof(glm::ivec4)) { return type_ == ShaderVarType.kVec4; }

      //if (val.GetType() == typeof(glm::uvec2)) { return type_ == ShaderVarType.kVec2; }
      //if (val.GetType() == typeof(glm::uvec3)) { return type_ == ShaderVarType.kVec3; }
      //if (val.GetType() == typeof(glm::uvec4)) { return type_ == ShaderVarType.kVec4; }

      if (val.GetType() == typeof(Vector2)) { return type_ == ShaderVarType.kVec2; }
      if (val.GetType() == typeof(Vector3)) { return type_ == ShaderVarType.kVec3; }
      if (val.GetType() == typeof(Vector4)) { return type_ == ShaderVarType.kVec4; }

      if (val.GetType() == typeof(Color)) { return type_ == ShaderVarType.kVec4; }
      //if (val.GetType() == typeof(Matrix3x3)) { return type_ == ShaderVarType.kMat3; }
      if (val.GetType() == typeof(Matrix4x4)) { return type_ == ShaderVarType.kMat4; }

      return false;
    }

    /**
    * @brief Default empty constructor, see the other constructor to create a ShaderVariable
    */
    public ShaderVariable()
    {
      type_ = ShaderVarType.kUnknown;
      data_ = null;
    }

    /**
    * @brief Creates a shader variable with a certain value type and a default value
    */
    public ShaderVariable(ShaderVarType type, object default_value = null)
    {
      type_ = type;
      data_ = default_value;
    }

    /**
    * @brief Assign this data to the shader variable and perform a type check for type safety
    * @tparam T The type of your data, deduced automatically.
    * @param[in] new_data (const T&) The value to assign
    * @return (ShaderVariable&) The shader variable to assign the new data to
    * @remarks Checks for variable type mismatch in debug.
    */
    public ShaderVariable Set(object new_data)
    {
      Debug.Assert(data_ != null);
      if (!TypeCheck(new_data))
      {
        Log.Write(Log.Verbosity.kWarning, "Shader variable type mismatch. Can't edit the data");
        return this;
      }

      data_ = new_data;
      return this;
    }

    public ShaderVarType type_ { get; private set; } //!< The type of variable this ShaderVarible contains
    public object data_ { get; set; } //!< A pointer to the raw data use to modify this variable inside of a uniform buffer
  };

  /**
  * @class UniformBuffer
  * @brief An interface for communicating with a uniform buffer
  * @author Jelle de Haan
  */
  public class UniformBuffer
  {
    //using MapPair = eastl::pair<const foundation::String, Sizet>;
    //using MapIterator = eastl::rbtree_iterator<MapPair, MapPair*, MapPair&>;
    //using ConstMapIterator = eastl::rbtree_iterator<MapPair, const MapPair*, const MapPair&>;

    /**
    * @brief Constructs an empty uniform buffer, use the other constructor to create one from a shader
    */
    public UniformBuffer()
    {
      Size = 0;
    }

    /**
    * @brief Constructs a new UniformBuffer, allocating a data buffer matching the shader data.
    * @param[in] shader_data (foundation::ShaderData&) The shader reflector data
    */
    public UniformBuffer(ShaderData shader_data)
    {
      // Construct data / name maps
      ConstructDataMaps("", shader_data.uniform_buffers);
    }

    /**
    * @brief Constructs a new UniformBuffer, allocating a data buffer matching the shader data.
    * @param[in] shader_data (ShaderData) The shader reflector data
    * @param[in] data_buffer (byte[]) The raw data to fill the uniform buffer with
    */
    public UniformBuffer(ShaderData shader_data, byte[] data_buffer)
    {
      // Construct data / name maps
      ConstructDataMaps("", shader_data.uniform_buffers);

      if (Size == 0)
      {
        return;
      }

      FromDataBuffer(data_buffer);
    }

    /**
    * @brief Finds the shader variable by its name. ( Ex.: var.Find("lights[0].direction"); )
    * @param[in] variable_name (const foundation::String&) The name of the shader variable
    * @return (ShaderVariable*) The shader variable or null if not found
    * @see sulphur::engine::UniformBuffer::operator[]
    * @see sulphur::engine::ShaderVariable
    */
    public ShaderVariable Find(string variable_name)
    {
      return this[variable_name];
    }

    /**
    * @brief Finds the shader variable by its name. ( Ex.: var["lights[0].direction"]; )
    * @param[in] variable_name (const foundation::String&) The name of the shader variable
    * @return (ShaderVariable*) The shader variable or null if not found
    * @see sulphur::engine::ShaderVariable
    */
    public ShaderVariable this[string variable_name]
    {
      get
      {
        int index = name_map_[variable_name];
        return data_map_[index];
      }

      set
      {
        int index = name_map_[variable_name];
        data_map_[index].Set(value);
      }
    }

    public int Size { get; private set; } //<! The raw byte size of Data

    public Dictionary<string, int> name_map_; //<! Links name to data_map_ index
    public List<ShaderVariable> data_map_; //<! Raw vector of all shader variables

    private Int32 GetTypeSize(ShaderVarType type)
    {
      switch (type)
      {
        case ShaderVarType.kBool: return sizeof(bool);
        case ShaderVarType.kDouble: return sizeof(double);
        case ShaderVarType.kFloat: return sizeof(float);
        case ShaderVarType.kInt: return sizeof(Int32);
        case ShaderVarType.kUint: return sizeof(UInt32);
        case ShaderVarType.kUint8: return sizeof(byte);
        case ShaderVarType.kVec2: return sizeof(float) * 2;
        case ShaderVarType.kVec3: return sizeof(float) * 3;
        case ShaderVarType.kVec4: return sizeof(float) * 4;
        case ShaderVarType.kMat3: return sizeof(float) * 3 * 3;
        case ShaderVarType.kMat4: return sizeof(float) * 4 * 4;
        default: Debug.Assert(false, "unknown type"); return 0;
      }
    }

    private ShaderVarType ConvertVariableType(ShaderResource.ConcreteTypes type)
    {
      switch (type)
      {
        case ShaderResource.ConcreteTypes.kBool:
          return ShaderVarType.kBool;
        case ShaderResource.ConcreteTypes.kChar:
          return ShaderVarType.kUint8;
        case ShaderResource.ConcreteTypes.kDouble:
          return ShaderVarType.kDouble;
        case ShaderResource.ConcreteTypes.kFloat:
          return ShaderVarType.kFloat;
        case ShaderResource.ConcreteTypes.kInt:
          return ShaderVarType.kInt;
        case ShaderResource.ConcreteTypes.kMat3x3:
          return ShaderVarType.kMat3;
        case ShaderResource.ConcreteTypes.kMat4x4:
          return ShaderVarType.kMat4;
        case ShaderResource.ConcreteTypes.kUInt:
          return ShaderVarType.kUint;
        case ShaderResource.ConcreteTypes.kVec2:
          return ShaderVarType.kVec2;
        case ShaderResource.ConcreteTypes.kVec3:
          return ShaderVarType.kVec3;
        case ShaderResource.ConcreteTypes.kVec4:
          return ShaderVarType.kVec4;
        default:
          return ShaderVarType.kUnknown;
      }
    }

    /**
    * @brief Constructs the data maps of this uniform buffer using the shader reflector
    * @param[in] name_prefix (const foundation::String&) The prefix to put in front of any newly created name
    * @param[in] shader_reflection_data (const foundation::Vector <foundation::ShaderResource>&) The reflection data used to create the data maps
    */
    private void ConstructDataMaps(string name_prefix, List<ShaderResource> shader_reflection_data)
    {
      name_map_ = new Dictionary<string, int>();
      data_map_ = new List<ShaderVariable>();

      foreach (ShaderResource buffer in shader_reflection_data)
      {
        // TODO:
        // properly handle structs
        // properly handle arrays

        if (buffer.concrete_type == ShaderResource.ConcreteTypes.kStruct)
        {
          ConstructDataMaps(name_prefix + buffer.name + ".", buffer.members);
          return;
        }

        ShaderVarType var_type = ConvertVariableType(buffer.concrete_type);

        name_map_.Add(buffer.name, data_map_.Count);
        data_map_.Add(new ShaderVariable(var_type));


        Size += GetTypeSize(var_type);
      }
    }

    /**
    * @brief Constructs the shader variables from a raw data buffer
    * @remarks the data maps need to be constructed before running this function
    */
    private unsafe void FromDataBuffer(byte[] data_buffer)
    {
      fixed (byte* data = data_buffer)
      {
        int offset = 0;

        foreach (ShaderVariable var in data_map_)
        {
          switch (var.type_)
          {
            case ShaderVarType.kBool:
              var.data_ = *(bool*)(data + offset);
              offset += sizeof(bool);
              break;
            case ShaderVarType.kDouble:
              var.data_ = *(double*)(data + offset);
              offset += sizeof(double);
              break;
            case ShaderVarType.kFloat:
              var.data_ = *(float*)(data + offset);
              offset += sizeof(float);
              break;
            case ShaderVarType.kInt:
              var.data_ = *(Int32*)(data + offset);
              offset += sizeof(Int32);
              break;
            case ShaderVarType.kUint:
              var.data_ = *(UInt32*)(data + offset);
              offset += sizeof(UInt32);
              break;
            case ShaderVarType.kUint8:
              var.data_ = *(byte*)(data + offset);
              offset += sizeof(byte);
              break;
            case ShaderVarType.kVec2:
              var.data_ = *(Vector2*)(data + offset);
              offset += sizeof(Vector2);
              break;
            case ShaderVarType.kVec3:
              var.data_ = *(Vector3*)(data + offset);
              offset += sizeof(Vector3);
              break;
            case ShaderVarType.kVec4:
              var.data_ = *(Vector4*)(data + offset);
              offset += sizeof(Vector4);
              break;
            /*case ShaderVarType.kMat3:
              var.data_ = *(Matrix3x3)(data + offset);
              offset += sizeof(Matrix3x3);
              break;*/
            case ShaderVarType.kMat4:
              var.data_ = *(Matrix4x4*)(data + offset);
              offset += sizeof(Matrix4x4);
              break;
            default:
              Debug.Assert(false, "unknown type");
              break;
          }
        }
      }
    }

    /**
    * @brief Constructs the raw byte buffer to store on disk
    */
    public unsafe byte[] ConstructDataBuffer()
    {
      if(Size == 0)
      {
        return null;
      }

      byte[] Data = new byte[Size];

      fixed (byte* data = Data)
      {
        int offset = 0;

        foreach (ShaderVariable var in data_map_)
        {
          switch (var.type_)
          {
            case ShaderVarType.kBool:
              *(bool*)(data + offset) = var.data_ != null ? (bool)(var.data_) : false;
              offset += sizeof(bool);
              break;
            case ShaderVarType.kDouble:
              *(double*)(data + offset) = var.data_ != null ? (double)(var.data_) : 0.0;
              offset += sizeof(double);
              break;
            case ShaderVarType.kFloat:
              *(float*)(data + offset) = var.data_ != null ? (float)(var.data_) : 0.0f;
              offset += sizeof(float);
              break;
            case ShaderVarType.kInt:
              *(Int32*)(data + offset) = var.data_ != null ? (Int32)(var.data_) : 0;
              offset += sizeof(Int32);
              break;
            case ShaderVarType.kUint:
              *(UInt32*)(data + offset) = var.data_ != null ? (UInt32)(var.data_) : 0u;
              offset += sizeof(UInt32);
              break;
            case ShaderVarType.kUint8:
              *(byte*)(data + offset) = var.data_ != null ? (byte)(var.data_) : (byte)0;
              offset += sizeof(byte);
              break;
            case ShaderVarType.kVec2:
              *(Vector2*)(data + offset) = var.data_ != null ? (Vector2)(var.data_) : new Vector2();
              offset += sizeof(Vector2);
              break;
            case ShaderVarType.kVec3:
              *(Vector3*)(data + offset) = var.data_ != null ? (Vector3)(var.data_) : new Vector3();
              offset += sizeof(Vector3);
              break;
            case ShaderVarType.kVec4:
              *(Vector4*)(data + offset) = var.data_ != null ? (Vector4)(var.data_) : new Vector4();
              offset += sizeof(Vector4);
              break;
            /*case ShaderVarType.kMat3:
              *(Matrix3x3*)(data + offset) = var.data_ != null ? (Matrix3x3)(var.data_) : new Matrix3x3();
              offset += sizeof(Matrix3x3);
              break;*/
            case ShaderVarType.kMat4:
              *(Matrix4x4*)(data + offset) = (var.data_ != null ? (Matrix4x4)(var.data_) : new Matrix4x4());
              offset += sizeof(Matrix4x4);
              break;
            default:
              Debug.Assert(false, "unknown type");
              break;
          }
        }
      }
      return Data;
    }
  };
}
