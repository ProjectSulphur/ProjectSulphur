#include "engine/assets/post_process_material.h"
#include "engine/assets/compute_shader.h"

#include "engine/assets/asset_system.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    ComputePass::ComputePass(const ComputeShaderHandle& compute_shader) :
      compute_shader_(compute_shader),
      uniform_buffer_(compute_shader->GetUniformBuffer()),
      textures_(compute_shader->GetTextureInfo().size()),
      uavs_(compute_shader->GetUAVInfo().size())
    {
      // Fill with white textures
      for (TextureHandle& texture : textures_)
      {
        texture = AssetSystem::Instance().GetHandle<Texture>("__pixel_white");
      }
    }

    //--------------------------------------------------------------------------------
    void ComputePass::SetShader(const ComputeShaderHandle& compute_shader)
    {
      compute_shader_ = compute_shader;
      
      uniform_buffer_ = compute_shader->GetUniformBuffer();

      textures_.clear();
      textures_.resize(compute_shader->GetTextureInfo().size());
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& ComputePass::GetTexture(size_t index) const
    {
      assert(textures_.size() >= index);
      return textures_.at(index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& ComputePass::GetTexture(const foundation::String& name) const
    {
      const foundation::Vector<TextureInfo> texture_info = compute_shader_->GetTextureInfo();
      for (int i = 0; i < texture_info.size(); ++i)
      {
        if (texture_info[i].name == name)
        {
          return textures_.at(i);
        }
      }
      assert(false && "This name does not match any of the shader variables");
      return textures_[0];
    }

    //--------------------------------------------------------------------------------
    void ComputePass::SetTexture(size_t index, const TextureHandle& texture)
    {
      assert(textures_.size() >= index); // Index is bigger than the amount of textures the shader expects
      textures_.at(index) = texture;
    }

    //--------------------------------------------------------------------------------
    void ComputePass::SetTexture(const foundation::String& name, const TextureHandle& texture)
    {
      const foundation::Vector<TextureInfo> texture_info = compute_shader_->GetTextureInfo();
      for (int i = 0; i < texture_info.size(); ++i)
      {
        if (texture_info[i].name == name)
        {
          textures_.at(i) = texture;
          return;
        }
      }
      assert(false && "This name does not match any of the shader variables");
    }

    //--------------------------------------------------------------------------------
    size_t ComputePass::NumTextures() const
    {
      return textures_.size();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& ComputePass::GetUAV(size_t index) const
    {
      assert(uavs_.size() >= index);
      return uavs_.at(index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& ComputePass::GetUAV(const foundation::String& name) const
    {
      const foundation::Vector<TextureInfo> texture_info = compute_shader_->GetUAVInfo();
      for (int i = 0; i < texture_info.size(); ++i)
      {
        if (texture_info[i].name == name)
        {
          return uavs_.at(i);
        }
      }
      assert(false && "This name does not match any of the shader variables");
      return uavs_[0];
    }

    //--------------------------------------------------------------------------------
    void ComputePass::SetUAV(size_t index, const TextureHandle& texture)
    {
      assert(uavs_.size() >= index); // Index is bigger than the amount of textures the shader expects
      uavs_.at(index) = texture;
    }

    //--------------------------------------------------------------------------------
    void ComputePass::SetUAV(const foundation::String& name, const TextureHandle& texture)
    {
      const foundation::Vector<TextureInfo> texture_info = compute_shader_->GetUAVInfo();
      for (int i = 0; i < texture_info.size(); ++i)
      {
        if (texture_info[i].name == name)
        {
          uavs_.at(i) = texture;
          return;
        }
      }
      assert(false && "This name does not match any of the shader variables");
    }

    //--------------------------------------------------------------------------------
    size_t ComputePass::NumUAVs() const
    {
      return uavs_.size();
    }

    //--------------------------------------------------------------------------------
    const glm::uvec3& ComputePass::GetWorkGroupSize() const
    {
      return compute_shader_->GetWorkGroupSize();
    }

    //--------------------------------------------------------------------------------
    UniformBuffer& ComputePass::GetUniformBuffer()
    {
      return uniform_buffer_;
    }

    //--------------------------------------------------------------------------------
    const UniformBuffer& ComputePass::GetUniformBuffer() const
    {
      return uniform_buffer_;
    }

    //--------------------------------------------------------------------------------
    PostProcessMaterial::PostProcessMaterial(const ComputeShaderHandle& compute_shader)
    {
      AddComputePass(ComputePass(compute_shader));
    }

    //--------------------------------------------------------------------------------
    PostProcessMaterial::PostProcessMaterial(std::initializer_list<ComputeShaderHandle> compute_shaders)
    {
      for (ComputeShaderHandle compute_shader : compute_shaders)
      {
        AddComputePass(ComputePass(compute_shader));
      }
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::AddComputePass(const ComputePass& pass)
    {
      compute_passes_.emplace_back(pass);
    }

    //--------------------------------------------------------------------------------
    ComputePass& PostProcessMaterial::GetComputePass(size_t index)
    {
      assert(compute_passes_.size() >= index);
      return compute_passes_.at(index);
    }

    //--------------------------------------------------------------------------------
    const ComputePass& PostProcessMaterial::GetComputePass(size_t index) const
    {
      assert(compute_passes_.size() >= index);
      return compute_passes_.at(index);
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::SetComputePass(size_t index, const ComputePass& pass)
    {
      assert(compute_passes_.size() >= index);
      compute_passes_.at(index) = pass;
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::RemoveComputePass(size_t index)
    {
      assert(compute_passes_.size() >= index);
      compute_passes_.erase(compute_passes_.begin() + index);
    }

    //--------------------------------------------------------------------------------
    size_t PostProcessMaterial::NumComputePasses() const
    {
      return compute_passes_.size();
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureHandle>& PostProcessMaterial::GetTextures(size_t pass_index) const
    {
      assert(compute_passes_.size() >= pass_index);
      return GetComputePass(pass_index).textures();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& PostProcessMaterial::GetTexture(size_t texture_index, size_t pass_index) const
    {
      assert(compute_passes_.size() >= pass_index);
      return GetComputePass(pass_index).GetTexture(texture_index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle & PostProcessMaterial::GetTexture(const foundation::String& name, size_t pass_index) const
    {
      return GetComputePass(pass_index).GetTexture(name);
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::SetTexture(size_t texture_index, const TextureHandle& texture, size_t pass_index)
    {
      assert(compute_passes_.size() >= pass_index);
      GetComputePass(pass_index).SetTexture(texture_index, texture);
    }
    
    //--------------------------------------------------------------------------------
    void PostProcessMaterial::SetTexture(const foundation::String& name, const TextureHandle& texture, size_t pass_index)
    {
      assert(compute_passes_.size() >= pass_index);
      GetComputePass(pass_index).SetTexture(name, texture);
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureHandle>& PostProcessMaterial::GetUAVs(size_t pass_index) const
    {
      assert(compute_passes_.size() >= pass_index);
      return GetComputePass(pass_index).textures();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& PostProcessMaterial::GetUAV(size_t uav_index, size_t pass_index) const
    {
      assert(compute_passes_.size() >= pass_index);
      return GetComputePass(pass_index).GetUAV(uav_index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle & PostProcessMaterial::GetUAV(const foundation::String& name, size_t pass_index) const
    {
      return GetComputePass(pass_index).GetUAV(name);
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::SetUAV(size_t uav_index, const TextureHandle& texture, size_t pass_index)
    {
      assert(compute_passes_.size() >= pass_index);
      GetComputePass(pass_index).SetUAV(uav_index, texture);
    }

    //--------------------------------------------------------------------------------
    void PostProcessMaterial::SetUAV(const foundation::String& name, const TextureHandle& texture, size_t pass_index)
    {
      assert(compute_passes_.size() >= pass_index);
      GetComputePass(pass_index).SetUAV(name, texture);
    }
  }
}