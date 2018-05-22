#pragma once
#include "foundation/containers/string.h"

namespace sulphur
{
  namespace foundation
  {
    /**
     * @class sulphur::foundation::SizedString
     * @brief A string of a fixed size.
     * @tparam T The size of the string -1.
     * @author Timo van Hees
     */
    template<uint32_t T>
    class SizedString
    {
    public:
      /**
       * @brief Create an empty sized string.
       */
      SizedString();
      /**
       * @brief Create a sized string from a C-style string.
       * @param[in] str (const char*) The string to copy.
       */
      SizedString(const char* str);
      /**
       * @brief Create a sized string from a string.
       * @param[in] str (const sulphur::foundation::String&) The string to copy.
       */
      SizedString(const foundation::String& str);
      
      /**
       * @brief Assignment operator overload.
       * @param[in] rhs C-style string to copy.
       * @return (sulphur::foundation::SizedString <T>&) Sized string equal to @rhs.
       */
      SizedString<T>& operator=(const char* rhs);
      /**
       * @brief Equal operator overload.
       * @param[in] rhs (const sulphur::foundation::SizedString <T>&) String to compare with.
       * @return (bool) True if the strings are equal.
       */
      bool operator==(const SizedString<T>& rhs) const { return GetString() == rhs.GetString(); }
      /**
      * @brief Doesn't equal operator overload.
      * @param[in] rhs (const sulphur::foundation::SizedString <T>&) String to compare with.
      * @return (bool) True if the strings are not equal.
      */
      bool operator!=(const SizedString<T>& rhs) const { return GetString() != rhs.GetString(); }
      /**
      * @brief Less than operator overload.
      * @param[in] rhs (const sulphur::foundation::SizedString <T>&) String to compare with.
      * @return (bool) True if the string is shorter or the characters have a lower 
      * ASCII code than @rhs.
      */
      bool operator<(const SizedString<T>& rhs) const { return GetString() < rhs.GetString(); }

      /**
       * @brief Get the actual length of the sized string.
       * @return (uint32_t) The length of the sized string.
       */
      uint32_t get_length() const;

      /**
       * @brief Get a copy of the sized string as a string.
       * @return (sulphur::foundation::String) Copy of the string.
       */
      String GetString() const;
      /**
       * @brief Get a copy of the sized string as a C-style string.
       * @return (const char*) Copy of the string.
       */
      const char* GetCString() const;

    private:
      uint32_t length_; //!< The length of the string.
      char string_[T];  //!< The buffer containing the characters of the string.
    };

    template <uint32_t T>
    SizedString<T>::SizedString(): length_(0)
    {
      memset(string_, '\0', T);
    }

    template <uint32_t T>
    SizedString<T>::SizedString(const char* str)
    {
      const size_t length = strlen(str);
      strncpy_s<T>(string_, str, T - 1);
      length_ = static_cast<uint32_t>((T - 1 < length) ? T - 1 : length);
    }

    template <uint32_t T>
    SizedString<T>::SizedString(const foundation::String& str)
    {
      strncpy_s<T>(string_, str.c_str(), T - 1);
      length_ = static_cast<uint32_t>(T - 1 < str.size() ? T - 1 : str.size());
    }

    template <uint32_t T>
    SizedString<T>& SizedString<T>::operator=(const char* rhs)
    {
      const size_t length = strlen(rhs);
      strncpy_s<T>(string_, rhs, T - 1);
      length_ = static_cast<uint32_t>((T - 1 < length) ? T - 1 : length);
      return *this;
    }

    template <uint32_t T>
    uint32_t SizedString<T>::get_length() const
    {
      return length_;
    }

    template <uint32_t T>
    foundation::String SizedString<T>::GetString() const
    {
      return foundation::String(string_);
    }

    template <uint32_t T>
    const char* SizedString<T>::GetCString() const
    {
      return string_;
    }
  }
}
