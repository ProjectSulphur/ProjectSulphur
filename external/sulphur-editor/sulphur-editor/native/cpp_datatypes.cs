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
      // struct representing a 1 byte large bool. .NET standard bool = 4 bytes while c++ bool = 1 byte
      // thus we need a 1 byte structure to represent a bool value
      public struct bool_
      {
        private byte value_;

        public static implicit operator bool_(bool other)
        {
          return new bool_(other);
        }

        public override int GetHashCode()
        {
          return value_.GetHashCode();
        }

        public override bool Equals(object obj)
        {
          return value_.Equals(obj);
        }

        public static bool operator ==(bool_ first, bool other)
        {
          return (other == true && first.value_ == 1) || (other == false && first.value_ == 0);
        }

        public static bool operator !=(bool_ first, bool other)
        {
          return (other == true && first.value_ == 0) || (other == false && first.value_ == 1);
        }

        public static implicit operator bool(bool_ value)
        {
          return (bool)value;
        }

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
