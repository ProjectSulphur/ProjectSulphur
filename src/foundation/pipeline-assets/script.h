#pragma once
#include "foundation/utils/asset_definitions.h"

namespace sulphur 
{
  namespace foundation 
  {
    
    /**
     * @class sulphur::foundation::ScriptData : public sulphur::foundation::IBinarySerializable
     * @brief The compiled script binary.
     * @author Timo van Hees
     */
    class ScriptData : public IBinarySerializable
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

      Vector<char> binary; //!< The compiled script binary.
    };

    /**
    * @struct sulphur::foundation::ScriptAsset
    * @brief Script asset.
    * @author Timo van Hees
    */
    struct ScriptAsset
    {
      AssetName name;     //!< The name of the script.
      AssetID id;         //!< The ID of the script.
      ScriptData data;    //!< The compiled script binary.
    };
  }
}
