#pragma once
#include "engine/core/world.h"
#include "engine/networking/messaging_system.h"
#include "engine/networking/message_in.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::Message <sulphur::engine::MessageType, TPayload>
    * @brief A structure that makes sending messages easier by providing an interface that wraps the individual message components.
    * @tparam actual_id (MessageID) The unique identifier that specifies the message type
    * @tparam TPayload (typename) The class that implements the data component of the message
    * @remarks For an example of how this might be used see sulphur::engine::PODMessage
    * @author Maarten ten Velden
    */
    template<MessageID actual_id, typename TPayload>
    class Message
    {
      static_assert(eastl::is_base_of<Payload, TPayload>::value, "TPayload must be derived from sulphur::engine::Payload");
      static_assert(sizeof(TPayload) < networking::editor::kMaxPayloadSize, "TPayload carries a payload bigger than the maximum allowed size");

    public:

      /**
      * @brief Accessor to edit the internal payload data
      */
      TPayload& payload();
      /**
      * @brief Constant getter to read the internal payload data
      */
      const TPayload& payload() const;
      /**
      * @brief Compile-time constant getter to get the message identifier.
      */
      constexpr MessageID id() const;

    private:
      TPayload payload_;
    };

    //-------------------------------------------------------------------------
    template<MessageID actual_id, typename TPayload>
    inline const TPayload& Message<actual_id, TPayload>::payload() const
    {
      return payload_;
    }

    //-------------------------------------------------------------------------
    template<MessageID actual_id, typename TPayload>
    inline TPayload& Message<actual_id, TPayload>::payload()
    {
      return payload_;
    }

    //-------------------------------------------------------------------------
    template<MessageID actual_id, typename TPayload>
    inline constexpr MessageID Message<actual_id, TPayload>::id() const
    {
      return actual_id;
    }
  }
}