#pragma once
#include "sized_string.h"
#include "foundation/containers/string.h"
#include "foundation/io/binary_reader.h"
#include "foundation/io/binary_writer.h"

namespace sulphur
{
  namespace foundation
  {
    using AssetID = uint64_t;           //!< ID type used to identify assets.
    using AssetName = SizedString<64>;  //!< String type used to identify assets.    

    /**
     * @struct sulphur::foundation::PackagePtr : sulphur::foundation::IBinarySerializable
     * @brief Stored in a package cache. Contains data about where the files 
     * related to the assets are stored.
     * @author Timo van Hees
     */
    struct PackagePtr : public IBinarySerializable
    {
      String asset_origin;  //!< The file path of the file the asset was created from.
      String filepath;      //!< The file path to the packaged file.

      /**
       * @see sulphur::foundation::IBinarySerializable::Write
       */
      void Write(BinaryWriter& binary_writer) const override
      {
        binary_writer.Write(asset_origin);
        binary_writer.Write(filepath);
      }
      /**
      * @see sulphur::foundation::IBinarySerializable::Read
      */
      void Read(BinaryReader& binary_reader) override
      {
        asset_origin = binary_reader.ReadString();
        filepath = binary_reader.ReadString();
      }
    };

    /**
     * @brief Generates an asset id from a name.
     * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
     * @return (sulphur::foundation::AssetID) The generated asset id.
     */
    inline AssetID GenerateId(const AssetName& name)
    {
      return eastl::string_hash<foundation::String>()(name.GetString());
    }
  }
}
