#include "model.h"

namespace sulphur 
{
	namespace foundation 
	{
    //--------------------------------------------------------------------------------
	  void ModelData::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(mesh.id);
      binary_writer.Write(materials.size());

      for (int i = 0; i < materials.size(); ++i)
      {
        binary_writer.Write(materials[i].id);
      }
	  }

    //--------------------------------------------------------------------------------
	  void ModelData::Read(BinaryReader& binary_reader)
	  {
      mesh.id = binary_reader.ReadUnsigned64();
      size_t num_materials = binary_reader.ReadUnsigned64();
      materials.resize(num_materials);
      for(size_t i = 0; i < num_materials; ++i)
      {
        materials[i].id = binary_reader.ReadUnsigned64();
      }
	  }
	}
}
