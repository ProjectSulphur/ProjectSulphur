#include "material_manager.h"
#include "asset_system.h"
#include <foundation/io/binary_reader.h>
#include <foundation/pipeline-assets/material.h>
#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Material* MaterialManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);
      if (reader.is_ok())
      {
        foundation::MaterialData asset_material = reader.Read<foundation::MaterialData>();

        AssetSystem& asset_system = AssetSystem::Instance();

        ShaderProgramHandle vertex_shader_program;
        ShaderProgramHandle hull_shader_program;
        ShaderProgramHandle domain_shader_program;
        ShaderProgramHandle geometry_shader_program;
        ShaderProgramHandle pixel_shader_program;

        if(asset_material.vertex_shader_id != 0)
        {
          vertex_shader_program = asset_system.Load<ShaderProgram>(
            asset_material.vertex_shader_id);
        }

        /*if (asset_material.hull_shader_id != 0)
        {
          hull_shader_program = asset_system.Load<ShaderProgram>(
            asset_material.hull_shader_id);
        }

        if (asset_material.domain_shader_id != 0)
        {
          domain_shader_program = asset_system.Load<ShaderProgram>(
            asset_material.domain_shader_id);
        }*/

        if (asset_material.geometry_shader_id != 0)
        {
          geometry_shader_program = asset_system.Load<ShaderProgram>(
            asset_material.geometry_shader_id);
        }

        if (asset_material.pixel_shader_id != 0)
        {
          pixel_shader_program = asset_system.Load<ShaderProgram>(
            asset_material.pixel_shader_id);
        }

        Shader* shader = foundation::Memory::Construct<Shader>(vertex_shader_program,
          hull_shader_program,
          domain_shader_program,
          geometry_shader_program,
          pixel_shader_program);

        if(shader->vertex_shader()->IsValid() == false || 
          shader->pixel_shader()->IsValid() == false)
        {
          PS_LOG(Error, "Material must have valid vertex and pixel shader.")
          foundation::Memory::Destruct(shader);
          return nullptr;
        }

        ShaderHandle shader_handle = asset_system.AddAsset(shader,
          asset_file.GetString()); // TODO: Shader should be given a proper name

        Material* material = foundation::Memory::Construct<Material>();

        MaterialPass pass(shader_handle);

        for (int i = 0; i < asset_material.separate_images.size(); ++i)
        {
          if (asset_material.separate_images[i] != 0)
          {
            pass.SetTexture(i, asset_system.Load<Texture>(
              asset_material.separate_images[i]));
          }
        }

        material->AddMaterialPass(pass);

        return material;
      }

      return nullptr;
    }
  }
}
