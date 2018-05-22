#include "model_info.h"
#include <foundation/logging/logger.h>

namespace sulphur 
{
  namespace foundation 
  {
    //-----------------------------------------------------------------------------------------------
    ModelInfo::ModelInfo(const String& file,
                         unsigned int number_of_models) :
      number_of_models_(number_of_models), 
      file_(file.c_str())
    {
      model_names_ = foundation::Vector<foundation::AssetName>(number_of_models_);
      models_to_load_ = foundation::Vector<bool>(number_of_models);
    }

    //-----------------------------------------------------------------------------------------------
    unsigned ModelInfo::number_of_models() const
    {
      return number_of_models_;
    }

    //-----------------------------------------------------------------------------------------------
    void ModelInfo::SetAssetName(unsigned index, const char* name)
    {
      if (index >= number_of_models_)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "index outside the range of models.");
        return;
      }

      model_names_[index] = name;
    }

    //-----------------------------------------------------------------------------------------------
    AssetName ModelInfo::GetAssetName(unsigned index) const
    {
      if (index >= number_of_models_)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "index outside the range of models.");
        return foundation::AssetName();
      }
      return model_names_[index];
    }

    //-----------------------------------------------------------------------------------------------
    void ModelInfo::SetModelsToLoad(unsigned index, bool load)
    {
      if (index >= number_of_models_)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "index outside the range of models.");
        return;
      }

      models_to_load_[index] = load;
    }

    //-----------------------------------------------------------------------------------------------
    bool ModelInfo::GetModelsToLoad(unsigned index) const
    {
      if (index >= number_of_models_)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "index outside the range of models.");
        return false;
      }

      return models_to_load_[index];
    }

    //-----------------------------------------------------------------------------------------------
    foundation::String ModelInfo::GetFile() const
    {
      return file_.GetString();
    }
  }
}
