using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      /**
       *@struct sulphur.editor.native.bool_
       *@brief struct used for c++ interoperation this represents a 1 byte large boolean 
       *@author Stan Pepels
       */
      public struct bool_
      {
        private byte value_; //<! value of the boolean stored as a byte

        /**
         *@brief implicit operation from C# native bool
         *@param[in] other (bool) value to represent
         */
        public static implicit operator bool_(bool other)
        {
          return new bool_(other);
        }

        /**
         *@brief override of get hash code 
         *@return (int) hash code created from the value_ variable 
         */
        public override int GetHashCode()
        {
          return value_.GetHashCode();
        }

        /**
         *@brief Equals function override 
         *@param[in] obj (object) object to compare to
         *@return (bool) reference object obj is equal to the value_ variable 
         */
        public override bool Equals(object obj)
        {
          return value_.Equals(obj);
        }

        /**
         *@brief == operator overload to compare to native C# bool 
         *@param[in] first (sulphur.editor.native bool_) left side of the == operator 
         *@param[in] second (bool) right side of the == operator  
         *@return (bool) if value_ represents the same as a C# bool
         */
        public static bool operator ==(bool_ first, bool other)
        {
          return (other == true && first.value_ > 0) || (other == false && first.value_ == 0);
        }

        /**
         *@brief == operator overload to compare to native C# bool 
         *@param[in] first (bool) left side of the == operator  
         *@param[in] second (sulphur.editor.native bool_) right side of the == operator 
         *@return (bool) if value_ represents the same as a C# bool
         */
        public static bool operator !=(bool_ first, bool other)
        {
          return (other == true && first.value_ == 0) || (other == false && first.value_ > 1);
        }

        /**
         *@brief implicit conversion to c# native bool 
         *@param[in] value (sulphur.editor.native.bool_) value to convert to bool 
         *@return (bool) bool value converted from value 
         */
        public static implicit operator bool(bool_ value)
        {
          return value.value_ == 0 ? false : true;
        }

        /**
         *@brief private constructor 
         *@param[in] value (bool) bool value to represent
         */
        private bool_(bool value)
        {
          if (value == true)
          {
            value_ = 1;
          }
          else
          {
            value_ = 0;
          }
        }
      };
    }
  }
}
