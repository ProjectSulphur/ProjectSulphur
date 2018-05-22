#pragma once
#include <foundation/containers/string.h>

namespace sulphur 
{
  namespace builder 
  {

    /**
    *@class sulphur::builder::Flag
    *@brief base class for flags used in commands to define certain options
    *@author Stan Pepels
    */
    class Flag
    {
    public:
      /**
      *@brief get the key value to which identifies this flag
      *@return (const char*) key to identify this command
      */
      virtual const char* GetKey() const = 0;

      /**
      *@brief set the flag to be allowed to be used multiple times in a command
      *@param[in] value (bool) new value
      */
      void set_allow_multiple(bool value);

      /**
      *@brief set the flag to take an argument when used
      *@param[in] value (bool) new value
      */
      void set_has_argument(bool value);

      /**
      *@brief set the flag to be optional in a command
      *@param[in] value (bool) new value
      */
      void set_optional(bool value);

      /**
      *@brief check if the flag takes arguments
      *@return (bool) boolean indicating whether this flag takes an argument
      */
      bool has_argument() const;

      /**
      *@brief check if the flag is optional
      *@return (bool) boolean indicating whether this flag is optional
      */
      bool optional() const;

      /**
      *@brief check if the flag can be used multiple times when executing a command
      *@return (bool) boolean indicating whether or not this flag can be passed multiple times to the same command
      */
      bool allow_multiple() const;

      /**
      *@brief check if Getkey() return value is the same other.GetKey() value
      *@param[in] other (const sulphur::builder::Flag&) another flag
      *@return (bool) boolean indicating wheter or not the passed flag equals this flag
      */
      bool operator == (const Flag& other) const;

      /**
      *@brief check if Getkey() return value is the same other->GetKey() value
      *@param[in] other (const sulphur::builder::Flag*) another flag pointer
      *@return (bool) boolean indicating wheter or not the passed flag equals this flag
      */
      bool operator == (const Flag* other) const;

      /**
      *@brief check if Getkey() return value is the same other
      *@param[in] other (const char*) value to compare to
      *@return (bool) boolean indicating wheter or not the GetKey functions return value equals other
      */
      bool operator == (const char* other) const;

      /**
      *@brief check if Getkey() return value is the same other
      *@param[in] other (const sulphur::foundation::String&) value to compare to
      *@return (bool) boolean indicating wheter or not the GetKey functions return value equals other.c_str()
      */
      bool operator == (const foundation::String& other) const;

    protected:

      /**
      *@brief default constructor
      */
      Flag();

    private:
      bool has_argument_; //<! value indicating if the flag takes an argument
      bool allow_multiple_; //<! value indication if this flag can be use multiple times in the same command 
      bool optional_;
    };
  }
}