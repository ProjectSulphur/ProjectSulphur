using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sulphur
{
  namespace editor
  {
    public interface IBinarySerializable
    {
      void Read(BinaryReader reader);
    }
  }
}
