#pragma once
#include "material.h"
#include "mesh.h"
#include "texture.h"

namespace sulphur 
{
	namespace foundation 
	{
    /**
    * @struct sulphur::foundation::ModelTextureCache
    * @brief Holds texture data for textures used by a material loaded from a scene.
    * @author Timo van Hees
    */
    struct ModelTextureCache
    {
      ModelTextureCache()
        : textures(1)
      {
        texture_lookup[""] = 0;
      }
      Vector<TextureAsset> textures;  //!< List of textures
      Map<String, int> texture_lookup;  //!< Lookup map to get the index of a texture by path.
    };

	  /**
	   * @class sulphur::foundation::ModelData : sulphur::foundation::IBinarySerializable
		 * @brief Model data to store in the package.
		 * @author Timo van Hees
		 */
    class ModelData : public IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable::Read
      */
      void Read(BinaryReader& binary_reader) override;

      MeshAsset mesh;                              //!< The mesh used by this model.
      Vector<MaterialAsset> materials; //!< The materials used by the sub-meshes of the mesh used by this model.
    };

    /**
    * @struct sulphur::foundation::ModelAsset
    * @brief Model loaded from a file.
    * @author Timo van Hees
    */
		struct ModelAsset
		{
      AssetName name; //!< The name of the model.
      AssetID id;     //!< The ID of the model.
      ModelData data; //!< The model data of the model.
      ModelTextureCache texture_cache; //!< The texture cache of the model.
		};
	}
}
