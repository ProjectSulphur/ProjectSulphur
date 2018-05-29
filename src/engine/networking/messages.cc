#include "engine/networking/messages.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    MessagePayload::MessagePayload()
      : used_bytes_(sizeof data_)
    {
    }

    //-------------------------------------------------------------------------
    MessagePayload::MessagePayload(const MessagePayload& payload) :
      used_bytes_(payload.used_bytes_)
    {
      memcpy_s(data_, sizeof data_, payload.data_, used_bytes_);
    }
  }
}