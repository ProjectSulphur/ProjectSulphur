#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include "tools/builder/shared/spv_shader_compiler.h"
#include "tools/builder/platform-specific/win32/win32_hlsl_compiler.h"
#include "tools/builder/shared/application.h"

#define ADDITIONALCOMPILERLIST sulphur::builder::Win32HlslCompiler

#include <foundation/io/binary_writer.h>
#include <foundation/io/binary_reader.h>
#include <foundation/logging/logger.h>

#include <spirv_cross.hpp>

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::Create(const foundation::Path& shader_file, 
      const ShaderPipelineOptions& options, foundation::ShaderAsset& shader)
    {
      foundation::String extension = shader_file.GetFileExtension();
      if (extension != "vert" && extension != "pixe" && extension != "geom" &&
        extension != "comp" && extension != "doma" && extension != "hull")
      {
        PS_LOG(Error, 
          "Shaders should have one of the following extensions: vert, pixe, geom, comp, doma or hull");
        return false;
      }

      foundation::BinaryReader reader(shader_file, false);

      if (reader.is_ok() == false)
      {
        PS_LOG_BUILDER(Warning,
          "Failed to load shader. file: %s", shader_file.GetString().c_str());
        return false;
      }


      if (GetShaderStage(shader_file, shader.data) == false)
      {
        DeconstructCompilers();

        PS_LOG_BUILDER(Warning,
          "Failed to deduce shader stage from file extension.");

        return false;
      }

      GetShaderName(shader_file, shader);

      
      foundation::String source_data = GetShaderDefines();
      source_data += reader.GetDataAsString().c_str();
      
      CreateFromSource(source_data, shader_file, shader.name.GetString(), 
        shader.data.stage, options, shader);

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::PackageShader(const foundation::Path& asset_origin,
      foundation::ShaderAsset& shader)
    {
      if (shader.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Warning,
          "Shader name not initialized. The shader will not be packaged.");
        return false;
      }

      if (shader.data.hlsl_data.empty() == true &&
        shader.data.spirv_data.empty() == true &&
        shader.data.pssl_data.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "Shader holds no binary data. The shader will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if (RegisterAsset(asset_origin, shader.name, output_file, shader.id) == false)
      {
        PS_LOG_BUILDER(Warning,
          "Failed to register shader. It will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(shader.data);

      if (writer.Save() == false)
      {
        PS_LOG_BUILDER(Warning,
          "Failed to package shader.");
        return false;
      }

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::String ShaderPipeline::GetCacheName() const
    {
      return "shader_package";
    }

    //-----------------------------------------------------------------------------------------------
    foundation::String ShaderPipeline::GetPackageExtension() const
    {
      return "ssp";
    }
    void ShaderPipeline::DeconstructCompilers()
    {
      while (compilers_.empty() == false)
      {
        delete compilers_.back();
        compilers_.pop_back();
      }
    }

    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::PackageDefaultAssets()
    {
      ShaderPipelineOptions options = {};
      options.targets = static_cast<int>(ShaderCompilerBase::Target::kAll);

      if (AssetExists("ps_default_vertex_shader") == false)
      {
        // Create default vertex shader
        foundation::ShaderAsset asset = {};
        asset.name = "ps_default_vertex_shader";
        asset.data.stage = foundation::ShaderData::ShaderStage::kVertex;

        foundation::String source = GetShaderDefines() +
          "PS_CONSTANTBUFFER SceneCB : register(b0)                                       \n"  \
          "{                                                                              \n"  \
          "  row_major float4x4 model;                                                    \n"  \
          "  row_major float4x4 view;                                                     \n"  \
          "  row_major float4x4 projection;                                               \n"  \
          "}                                                                              \n"  \
          "                                                                               \n"  \
          "struct VS_OUTPUT                                                               \n"  \
          "{                                                                              \n"  \
          "  float4 position : PS_VSOUTPUT;                                               \n"
          "};                                                                             \n"  \
          "                                                                               \n"  \
          "struct VS_INPUT                                                                \n"  \
          "{                                                                              \n"  \
          "  float3 position : POSITION;                                                  \n"  \
          "  float3 normal : NORMAL;                                                      \n"  \
          "  float3 tangent : TANGENT;                                                    \n"  \
          "  float2 uv : TEXCOORD;                                                        \n"  \
          "  float4 color : COLOR;                                                        \n"  \
          "};                                                                             \n"  \
          "                                                                               \n"  \
          "VS_OUTPUT main(VS_INPUT input)                                                 \n"  \
          "{                                                                              \n"  \
          "  VS_OUTPUT output;                                                            \n"  \
          "                                                                               \n"  \
          "  output.position = float4(input.position.xyz, 1.0f);                          \n"  \
          "  float4 pos = mul(output.position, mul(model, view));                         \n"  \
          "  output.position = mul(pos, projection);                                      \n"  \
          "                                                                               \n"  \
          "  return output;                                                               \n"  \
          "}                                                                              \n";

        if (CreateFromSource(source, "", asset.name.GetString(), asset.data.stage,
          options, asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to create default asset.");
          return false;
        }

        if (PackageShader(ASSET_ORIGIN_USER, asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to package default asset.");
          return false;
        }
      }

      if (AssetExists("ps_default_pixel_shader") == false)
      {
        // Create default pixel shader
        foundation::ShaderAsset asset = {};
        asset.name = "ps_default_pixel_shader";
        asset.data.stage = foundation::ShaderData::ShaderStage::kPixel;

        foundation::String source = GetShaderDefines() +
          "PS_CONSTANTBUFFER SceneCB : register(b0)                                                   \n" \
          "{                                                                                          \n" \
          "  row_major float4x4 model;                                                                \n" \
          "  row_major float4x4 view;                                                                 \n" \
          "  row_major float4x4 projection;                                                           \n" \
          "}                                                                                          \n" \
          "                                                                                           \n" \
          "struct VS_OUTPUT                                                                           \n" \
          "{                                                                                          \n" \
          "  float4 position : PS_VSOUTPUT;                                                           \n" \
          "};                                                                                         \n" \
          "Texture2D ps_texture_albedo : register(t0);                                                \n" \
          "SamplerState g_sampler : register(s0);                                                     \n" \
          "                                                                                           \n" \
          "float4 main(VS_OUTPUT input) : PS_PSOUTPUT                                                 \n" \
          "{                                                                                          \n" \
          "  float4 sampled_color =  ps_texture_albedo.Sample(g_sampler, float2(0, 0)) * 0.000001;    \n" \
          "  return float4(1.0, 0.0, 1.0, 1.0) + sampled_color;                                       \n" \
          "}                                                                                          \n";

        if (CreateFromSource(source, "", asset.name.GetString(), asset.data.stage,
          options, asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to create default asset.");
          return false;
        }

        if (PackageShader(ASSET_ORIGIN_USER, asset) == false)
        {
          PS_LOG_BUILDER(Error,
            "Failed to package default asset.");
          return false;
        }
      }

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::GetShaderName(const foundation::Path& path, 
      foundation::ShaderAsset& shader)
    {
      if(path.is_file_path() == false)
      {
        return false;
      }

      shader.name = path.GetFileName();
      return true;
    }

    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::GetShaderStage(const foundation::Path& path, 
      foundation::ShaderData& shader)
    {
      foundation::String stage_str = path.GetFileExtension();

      if (stage_str == "vert")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kVertex;
      }
      else if (stage_str == "comp")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kCompute;
      }
      else if (stage_str == "doma")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kDomain;
      }
      else if (stage_str == "hull")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kHull;
      }
      else if (stage_str == "geom")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kGeometry;
      }
      else if (stage_str == "pixe")
      {
        shader.stage = foundation::ShaderData::ShaderStage::kPixel;
      }
      else
      {
        return false;
      }
      return true;
    }

    bool ShaderPipeline::ValidateSource(
      const foundation::String& shader_source,
      foundation::ShaderAsset& shader,
      const foundation::Path& path,
      const ShaderPipelineOptions& options,
      foundation::Vector<uint8_t>& out_compiled)
    {
      SpvShaderCompiler compiler(options);
      return compiler.CompileShader(shader_source, shader, path, out_compiled);
      
    }

    void ShaderPipeline::ProcessShaderResource(foundation::ShaderResource& resource, 
      const spirv_cross::SPIRType& spv_type, const spirv_cross::Compiler& compiler)
    {
      switch (spv_type.basetype)
      {
      case spirv_cross::SPIRType::BaseType::Struct:
      {
        for (uint32_t i = 0; i < spv_type.member_types.size(); ++i)
        {
          foundation::ShaderResource empty_resource{};
          resource.members.push_back(empty_resource);
          spirv_cross::SPIRType member = compiler.get_type(spv_type.member_types[i]);
          ProcessShaderResource(resource.members.back(), member, compiler);
          resource.members.back().name = compiler.get_member_name(spv_type.self, i).c_str();
          resource.members.back().size = compiler.get_declared_struct_member_size(spv_type, i);
          resource.members.back().offset = compiler.type_struct_member_offset(spv_type, i);
        }
        resource.size = compiler.get_declared_struct_size(spv_type);
      }
      break;
      case spirv_cross::SPIRType::BaseType::Boolean:
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kBool;
        break;
      case spirv_cross::SPIRType::BaseType::Float:
        if (spv_type.vecsize > 1 && spv_type.columns == 1)
        {
          switch (spv_type.vecsize)
          {
          case 2:
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kVec2;
            break;
          case 3:
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kVec3;
            break;
          case 4:
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kVec4;
            break;
          }
        }
        else if (spv_type.vecsize > 1 && spv_type.columns > 1)
        {
          if (spv_type.columns == 3 && spv_type.vecsize == 3)
          {
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kMat3x3;
          }
          else if (spv_type.columns == 3 && spv_type.vecsize == 4)
          {
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kMat4x3;
          }
          else if (spv_type.columns == 4 && spv_type.vecsize == 4)
          {
            resource.concrete_type = foundation::ShaderResource::ConcreteType::kMat4x4;
          }
        }
        else
        {
          resource.concrete_type = foundation::ShaderResource::ConcreteType::kFloat;
        }
        break;
      case spirv_cross::SPIRType::BaseType::Int:
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kInt;
        break;
      case spirv_cross::SPIRType::BaseType::UInt:
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kUInt;
        break;
      case spirv_cross::SPIRType::BaseType::Double:
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kDouble;
        break;
      case spirv_cross::SPIRType::BaseType::Char:
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kChar;
        break;
      case spirv_cross::SPIRType::BaseType::Sampler:
        resource.base_type = foundation::ShaderResource::Types::kSeparateSampler;
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kUnknown;
        break;
      case spirv_cross::SPIRType::BaseType::Image:
      {
        resource.base_type = foundation::ShaderResource::Types::kSeparateImage;
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kUnknown;
        switch (spv_type.image.dim)
        {
        case spv::Dim::Dim1D:
          resource.image.dimension = foundation::ShaderResource::Image::Dimensions::k1D;
          break;
        case spv::Dim::Dim2D:
          resource.image.dimension = foundation::ShaderResource::Image::Dimensions::k2D;
          break;
        case spv::Dim::Dim3D:
          resource.image.dimension = foundation::ShaderResource::Image::Dimensions::k3D;
          break;
        case spv::Dim::DimCube:
          resource.image.dimension = foundation::ShaderResource::Image::Dimensions::kCubed;

          break;
        }
        resource.image.arrayed = spv_type.image.arrayed;
      }
      break;
      default: //int64, uint64
        resource.concrete_type = foundation::ShaderResource::ConcreteType::kUnknown;
        break;
      }
      resource.array_size.resize(spv_type.array.size());
      memcpy_s(resource.array_size.data(), sizeof(uint32_t) * spv_type.array.size(),
        spv_type.array.data(), sizeof(uint32_t) * spv_type.array.size());

      resource.vec_size = spv_type.vecsize;
      resource.cols = spv_type.columns;
      resource.name = compiler.get_name(spv_type.self).c_str();
    }

    //-----------------------------------------------------------------------------------------------
    bool ShaderPipeline::CreateFromSource(const foundation::String& source, 
      const foundation::Path& shader_file,
      const foundation::String& name, 
      foundation::ShaderData::ShaderStage shader_stage,
      const ShaderPipelineOptions& options,
      foundation::ShaderAsset& shader)
    {
      shader.name = name;
      shader.data.stage = shader_stage;

      ConstructCompilers<ADDITIONALCOMPILERLIST>(options);

      foundation::Vector<uint8_t> compiled;
      if (ValidateSource(source.c_str(), shader, shader_file, options, compiled) == false)
      {
        DeconstructCompilers();

        PS_LOG_BUILDER(Warning,
          "Shader validation failed. file: %s.", shader_file.GetString().c_str());
        return false;
      }

      Reflect(compiled, shader.data);

      if ((static_cast<uint8_t>(ShaderCompilerBase::Target::kSpirv) & options.targets) != 0 ||
        (static_cast<uint8_t>(ShaderCompilerBase::Target::kAll) & options.targets) != 0)
      {
        shader.data.spirv_data = compiled;
      }

      for (ShaderCompilerBase* compiler : compilers_)
      {
        if ((options.targets & static_cast<uint8_t>(compiler->target())) != 0 ||
          (options.targets & static_cast<uint8_t>(ShaderCompilerBase::Target::kAll)) != 0)
        {
          if (compiler->CompileShader(source, shader, shader_file, compiled))
          {
            switch (compiler->target())
            {
            case ShaderCompilerBase::Target::kHlsl: shader.data.hlsl_data = compiled; break;
            case ShaderCompilerBase::Target::kPssl: shader.data.pssl_data = compiled; break;
            default:;
            }
          }
        }
      }

      DeconstructCompilers();

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    void ShaderPipeline::Reflect(const foundation::Vector<uint8_t>& compiled, foundation::ShaderData& out_shader)
    {
      spirv_cross::Compiler compiler(reinterpret_cast<const uint32_t*>(compiled.data()), compiled.size() / sizeof(uint32_t));
      spirv_cross::ShaderResources resources = compiler.get_shader_resources();

      auto copy_resources = [&compiler, this](
        std::vector<spirv_cross::Resource>& resources,
        foundation::ShaderResource::Types type) -> foundation::Vector<foundation::ShaderResource>
      {
        foundation::Vector<foundation::ShaderResource> result = foundation::Vector<foundation::ShaderResource>(resources.size());
        int i = 0;
        for (spirv_cross::Resource& res : resources)
        {
          foundation::ShaderResource resource = foundation::ShaderResource();
          resource.desc_set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
          resource.binding = compiler.get_decoration(res.id, spv::DecorationBinding);
          spirv_cross::SPIRType spv_type = compiler.get_type(res.type_id);
          ProcessShaderResource(resource, spv_type, compiler);
          resource.name = res.name.c_str();
          resource.base_type = type;
          result[i] = resource;
          ++i;
        }
        return result;
      };

      spirv_cross::SPIREntryPoint& entry = compiler.get_entry_point(compiler.get_entry_points()[0]);

      spirv_cross::SpecializationConstant wg_x, wg_y, wg_z;
      compiler.get_work_group_size_specialization_constants(wg_x, wg_y, wg_z);

      glm::uvec3 workgroup_size;
      workgroup_size.x = entry.workgroup_size.x;
      workgroup_size.y = entry.workgroup_size.y;
      workgroup_size.z = entry.workgroup_size.z;

      if (wg_x.id != 0)
      {
        workgroup_size.x = compiler.get_constant(wg_x.id).scalar();
      }

      if (wg_y.id != 0)
      {
        workgroup_size.y = compiler.get_constant(wg_y.id).scalar();
      }

      if (wg_z.id != 0)
      {
        workgroup_size.z = compiler.get_constant(wg_z.id).scalar();
      }
      
      out_shader.workgroup_size = workgroup_size;
      out_shader.atomic_counters = copy_resources(
        resources.atomic_counters,
        foundation::ShaderResource::Types::kAtomicCounter);
      out_shader.inputs = copy_resources(
        resources.stage_inputs,
        foundation::ShaderResource::Types::kInput);
      out_shader.outputs = copy_resources(
        resources.stage_outputs,
        foundation::ShaderResource::Types::kOutput);
      out_shader.sampled_images = copy_resources(
        resources.sampled_images,
        foundation::ShaderResource::Types::kSampledImage);
      out_shader.separate_images = copy_resources(
        resources.separate_images,
        foundation::ShaderResource::Types::kSeparateImage);
      out_shader.separate_samplers = copy_resources(
        resources.separate_samplers,
        foundation::ShaderResource::Types::kSeparateSampler);
      out_shader.storage_images = copy_resources(
        resources.storage_images,
        foundation::ShaderResource::Types::kStorageImage);
      out_shader.uniform_buffers = copy_resources(
        resources.uniform_buffers,
        foundation::ShaderResource::Types::kUniformBuffer);
    }
  }
}