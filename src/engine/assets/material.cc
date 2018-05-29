#include "engine/assets/material.h"
#include "engine/assets/shader.h"

#include "engine/assets/asset_system.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    MaterialPass::MaterialPass(const ShaderHandle& shader) :
      shader_(shader),
      uniform_buffers_(),
      textures_(shader->GetTextureInfo().size()),
      uavs_(shader->GetUAVInfo().size())
    {
      for (size_t i = 0; i < 5; ++i)
      {
        ShaderProgramHandle program = shader->GetShaderByType(static_cast<ShaderType>(i));
        
        if (program && program->IsValid())
        {
          uniform_buffers_[i] = program->uniform_buffer();
        }
      }

      // Fill with white textures
      for (TextureHandle& texture : textures_)
      {
        texture = AssetSystem::Instance().GetHandle<Texture>("__pixel_white");
      }

      // Fill with white textures
      for (TextureHandle& texture : uavs_)
      {
        texture = AssetSystem::Instance().GetHandle<Texture>("__pixel_white");
      }
    }

    //--------------------------------------------------------------------------------
    void MaterialPass::SetShader(const ShaderHandle& shader)
    {
      shader_ = shader;

      for (size_t i = 0; i < 5; ++i)
      {
        ShaderProgramHandle program = shader->GetShaderByType(static_cast<ShaderType>(i));

        if (program && program->IsValid())
        {
          uniform_buffers_[i] = program->uniform_buffer();
        }
      }

      textures_.clear();
      textures_.resize(shader->GetTextureInfo().size());

      uavs_.clear();
      uavs_.resize(shader->GetUAVInfo().size());
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& MaterialPass::GetTexture(size_t index) const
    {
      assert(textures_.size() >= index); // Index is bigger than the amount of textures the shader expects
      return textures_.at(index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& MaterialPass::GetTexture(const foundation::String& name) const
    {
      const foundation::Vector<TextureInfo> texture_info = shader_->GetTextureInfo();
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
    void MaterialPass::SetTexture(size_t index, const TextureHandle& texture)
    {
      assert(textures_.size() >= index); // Index is bigger than the amount of textures the shader expects
      textures_.at(index) = texture ? texture : AssetSystem::Instance().GetHandle<Texture>("__pixel_magenta");
    }

    //--------------------------------------------------------------------------------
    void MaterialPass::SetTexture(const foundation::String& name, const TextureHandle& texture)
    {
      const foundation::Vector<TextureInfo> texture_info = shader_->GetTextureInfo();
      for (int i = 0; i < texture_info.size(); ++i)
      {
        if (texture_info[i].name == name)
        {
          SetTexture(i, texture);
          return;
        }
      }
      assert(false && "This name does not match any of the shader variables");
    }

    //--------------------------------------------------------------------------------
    size_t MaterialPass::NumTextures() const
    {
      return textures_.size();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& MaterialPass::GetUAV(size_t index) const
    {
      assert(uavs_.size() >= index);
      return uavs_.at(index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& MaterialPass::GetUAV(const foundation::String& name) const
    {
      const foundation::Vector<TextureInfo> uav_info = shader_->GetUAVInfo();
      for (int i = 0; i < uav_info.size(); ++i)
      {
        if (uav_info[i].name == name)
        {
          return uavs_.at(i);
        }
      }
      assert(false && "This name does not match any of the shader variables");
      return uavs_[0];
    }

    //--------------------------------------------------------------------------------
    void MaterialPass::SetUAV(size_t index, const TextureHandle& uav)
    {
      assert(uavs_.size() >= index); // Index is bigger than the amount of uavs the shader expects
      uavs_.at(index) = uav;
    }

    //--------------------------------------------------------------------------------
    void MaterialPass::SetUAV(const foundation::String& name, const TextureHandle& uav)
    {
      const foundation::Vector<TextureInfo> uav_info = shader_->GetUAVInfo();
      for (int i = 0; i < uav_info.size(); ++i)
      {
        if (uav_info[i].name == name)
        {
          uavs_.at(i) = uav;
          return;
        }
      }
      assert(false && "This name does not match any of the shader variables");
    }

    //--------------------------------------------------------------------------------
    size_t MaterialPass::NumUAVs() const
    {
      return uavs_.size();
    }

    //--------------------------------------------------------------------------------
    UniformBuffer& MaterialPass::GetUniformBuffer(ShaderType shader_type)
    {
      return uniform_buffers_[static_cast<size_t>(shader_type)];
    }

    //--------------------------------------------------------------------------------
    const UniformBuffer& MaterialPass::GetUniformBuffer(ShaderType shader_type) const
    {
      return uniform_buffers_[static_cast<size_t>(shader_type)];
    }

    //--------------------------------------------------------------------------------
    Material::Material(const ShaderHandle& shader)
    {
      AddMaterialPass(MaterialPass(shader));
    }

    //--------------------------------------------------------------------------------
    Material::Material(std::initializer_list<ShaderHandle> shaders)
    {
      for (ShaderHandle shader : shaders)
      {
        AddMaterialPass(MaterialPass(shader));
      }
    }

    //--------------------------------------------------------------------------------
    void Material::AddMaterialPass(const MaterialPass& pass)
    {
      material_passes_.emplace_back(pass);
    }

    //--------------------------------------------------------------------------------
    MaterialPass& Material::GetMaterialPass(size_t index)
    {
      assert(material_passes_.size() >= index);
      return material_passes_.at(index);
    }

    //--------------------------------------------------------------------------------
    const MaterialPass& Material::GetMaterialPass(size_t index) const
    {
      assert(material_passes_.size() >= index);
      return material_passes_.at(index);
    }

    //--------------------------------------------------------------------------------
    void Material::SetMaterialPass(size_t index, const MaterialPass& pass)
    {
      assert(material_passes_.size() >= index);
      material_passes_.at(index) = pass;
    }

    //--------------------------------------------------------------------------------
    void Material::RemoveMaterialPass(size_t index)
    {
      assert(material_passes_.size() >= index);
      material_passes_.erase(material_passes_.begin() + index);
    }

    //--------------------------------------------------------------------------------
    size_t Material::NumMaterialPasses() const
    {
      return material_passes_.size();
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureHandle>& Material::GetTextures(size_t pass_index) const
    {
      assert(material_passes_.size() >= pass_index);
      return GetMaterialPass(pass_index).textures();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& Material::GetTexture(size_t texture_index, size_t pass_index) const
    {
      assert(material_passes_.size() >= pass_index);
      return GetMaterialPass(pass_index).GetTexture(texture_index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& Material::GetTexture(const foundation::String& name, 
      size_t pass_index) const
    {
      return GetMaterialPass(pass_index).GetTexture(name);
    }

    //--------------------------------------------------------------------------------
    void Material::SetTexture(size_t texture_index, 
      const TextureHandle& texture, size_t pass_index)
    {
      assert(material_passes_.size() >= pass_index);
      GetMaterialPass(pass_index).SetTexture(texture_index, texture);
    }
    
    //--------------------------------------------------------------------------------
    void Material::SetTexture(const foundation::String& name, 
      const TextureHandle& texture, size_t pass_index)
    {
      assert(material_passes_.size() >= pass_index);
      GetMaterialPass(pass_index).SetTexture(name, texture);
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureHandle>& Material::GetUAVs(size_t pass_index) const
    {
      assert(material_passes_.size() >= pass_index);
      return GetMaterialPass(pass_index).uavs();
    }

    //--------------------------------------------------------------------------------
    const TextureHandle& Material::GetUAV(size_t texture_index, size_t pass_index) const
    {
      assert(material_passes_.size() >= pass_index);
      return GetMaterialPass(pass_index).GetUAV(texture_index);
    }

    //--------------------------------------------------------------------------------
    const TextureHandle & Material::GetUAV(const foundation::String& name, size_t pass_index) const
    {
      return GetMaterialPass(pass_index).GetUAV(name);
    }

    //--------------------------------------------------------------------------------
    void Material::SetUAV(size_t texture_index, const TextureHandle& texture, size_t pass_index)
    {
      assert(material_passes_.size() >= pass_index);
      GetMaterialPass(pass_index).SetUAV(texture_index, texture);
    }

    //--------------------------------------------------------------------------------
    void Material::SetUAV(const foundation::String& name, 
      const TextureHandle& texture, size_t pass_index)
    {
      assert(material_passes_.size() >= pass_index);
      GetMaterialPass(pass_index).SetUAV(name, texture);
    }
  }
}