#include "d3d12_root_signature.h"

#include "d3d12_device_wrapper.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12RootSignature::~D3D12RootSignature()
    {
      SafeRelease(root_signature_);
    }

    //------------------------------------------------------------------------------------------------------
    ID3D12RootSignature* D3D12RootSignature::CreateRootSignatureFromShader(
      const engine::ShaderHandle& shader,
      D3D12Device& device)
    {
      size_t num_textures = shader->GetTextureInfo().size();

      D3D12_STATIC_SAMPLER_DESC linear_sampler_desc = {};
      linear_sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      linear_sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      linear_sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      linear_sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
      linear_sampler_desc.MaxAnisotropy = 0;
      linear_sampler_desc.MipLODBias = 0;
      linear_sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
      linear_sampler_desc.MinLOD = 0;
      linear_sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
      linear_sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
      linear_sampler_desc.RegisterSpace = 0;
      linear_sampler_desc.ShaderRegister = 0;
      linear_sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

      //D3D12_STATIC_SAMPLER_DESC point_sampler_desc = {};
      //point_sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      //point_sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      //point_sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      //point_sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
      //point_sampler_desc.MaxAnisotropy = 0;
      //point_sampler_desc.MipLODBias = 0;
      //point_sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
      //point_sampler_desc.MinLOD = 0;
      //point_sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
      //point_sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
      //point_sampler_desc.RegisterSpace = 0;
      //point_sampler_desc.ShaderRegister = 1;
      //point_sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

      D3D12_STATIC_SAMPLER_DESC sampler_descs[] =
      {
        linear_sampler_desc,
        //point_sampler_desc
      };

      D3D12_DESCRIPTOR_RANGE1 desc_ranges[] =
      {
        {
          D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
          (uint32_t)num_textures,
          0,
          0,
          D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
          0
        }
      };

      CD3DX12_ROOT_PARAMETER1 root_parameters[2];
      root_parameters[0].InitAsConstantBufferView(
        0,
        0,
        D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
        D3D12_SHADER_VISIBILITY_ALL
      );

      root_parameters[1].InitAsDescriptorTable(
        _countof(desc_ranges),
        desc_ranges,
        D3D12_SHADER_VISIBILITY_PIXEL
      );

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
      
      desc.Init_1_1(
        _countof(root_parameters),
        root_parameters,
        _countof(sampler_descs),
        sampler_descs,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
      );

      ID3D12RootSignature* rs;
      if (device.CreateVersionedRootSignature(rs, desc) == false)
      {
        PS_LOG(Error, "Failed to create versioned root signature from shader reflection.\n");
      }

      return rs;
    }


  }
}