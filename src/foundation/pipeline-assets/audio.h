#pragma once
#include "foundation/utils/asset_definitions.h"

namespace sulphur 
{
  namespace foundation 
  {    
    /**
     * @class sulphur::foundation::AudioBankData : public sulphur::foundation::IBinarySerializable
     * @brief Audio bank data.
     * @author Timo van Hees
     */
    class AudioBankData : public IBinarySerializable
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

      Vector<char> data; //!< The audio bank data.
    };

    /**
    * @struct sulphur::foundation::AudioBankAsset
    * @brief Audio bank asset.
    * @author Timo van Hees
    */
    struct AudioBankAsset
    {
      AssetName name;     //!< The name of the audio bank.
      AssetID id;         //!< The ID of the audio bank.
      AudioBankData data; //!< The audio bank data.
    };
  }
}
