#pragma once
#include "engine/networking/messages.h"

#include <foundation/utils/asset_definitions.h>
#include <glm/glm.hpp>
#include <cinttypes>

namespace sulphur
{
  namespace engine
  {

    /**
    * Adding a custom message requires the following steps to be completed:
    * 1. Add an enumerator for the new message to sulphur::engine::editor::MessageID
    * 2. Declare a struct that inherits from sulphur::engine::editor::Payload in this file that
    *    implement the data component of the message you are adding.
    * 3. Declare an alias that specifies sulphur::engine::Message <sulphur::editor::MessageID, TPayload>
    *    passing it the identifier and the payload class
    */

    /**
    * @struct sulphur::engine::PODPayload : sulphur::engine::editor::Payload
    * @see sulphur::engine::MessageID::kPOD
    * @author Maarten ten Velden
    */
    struct PODPayload : Payload
    {
      byte data[networking::editor::kMaxPayloadSize];
    };
    using PODMessage = Message<MessageID::kPOD, PODPayload>;

    /**
    * @struct sulphur::engine::StringPayload : sulphur::engine::editor::Payload
    * @see sulphur::engine::MessageID::kString
    * @author Maarten ten Velden
    */
    struct StringPayload : Payload
    {
      char string[64]; //!< string with a max 64 characters
    };
    using StringMessage = Message<MessageID::kString, StringPayload>;
  }
}