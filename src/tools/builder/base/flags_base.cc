#include "tools/builder/base/flags_base.h"

namespace sulphur 
{
  namespace builder 
  {

    //-----------------------------------------------------------------------------------------------
    Flag::Flag() :
      allow_multiple_(false)
    {
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::operator==(const Flag& other) const
    {
      return *GetKey() == *other.GetKey();
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::operator==(const Flag* other) const
    {
      return *GetKey() == *other->GetKey();
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::operator==(const char* other) const
    {
      return GetKey() == other;
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::operator==(const foundation::String& other) const
    {
      return GetKey() == other;
    }

    //-----------------------------------------------------------------------------------------------
    void Flag::set_allow_multiple(bool value)
    {
      allow_multiple_ = value;
    }

    //-----------------------------------------------------------------------------------------------
    void Flag::set_has_argument(bool value)
    {
      has_argument_ = value;
    }

    void Flag::set_optional(bool value)
    {
      optional_ = value;
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::has_argument() const 
    {
      return has_argument_;
    }

    bool Flag::optional() const
    {
      return optional_;
    }

    //-----------------------------------------------------------------------------------------------
    bool Flag::allow_multiple() const 
    {
      return allow_multiple_;
    }
  }
}