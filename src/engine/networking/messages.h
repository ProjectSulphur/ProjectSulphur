#pragma once
#include <tools/networking/export.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @brief An enumerator containing the identifiers of all the messages that can be sent/received
    */
    enum struct MessageID : uint32_t
    {
      kPOD = 0, //!< Unformatted plain-old-data
      kString, //!< A text string
      kNumMessages, //!< The amount of unique messages that can be received
    };

    /**
    * @struct sulphur::engine::Payload
    * @brief Base class for custom payload objects.
    * @details Used for compile-time type verification and defines special member function behaviour.
    * @author Maarten ten Velden
    */
    struct Payload
    {
      Payload& operator=(const Payload&) = delete;
    };

    /**
    * @class sulphur::engine::MessagePayload
    * @brief A class representing the data component of a message.
    * @see sulphur::engine::editor::Message::SendLocal
    * @see sulphur::engine::editor::Message::SendToEditor
    * @author Maarten ten Velden
    */
    class MessagePayload
    {
    public:
      /**
      * @brief Default constructor
      * @remarks Sets the amount of bytes used to the max payload capacity (networking::editor::kMaxPayloadSize)
      */
      MessagePayload();
      /**
      * @brief Custom copy constructor. Only copies over the bytes that are actually used
      * @param[in] payload (const sulphur::engine::editor::MessagePayload&) The object from which to copy the data over
      * @see sulphur::engine::editor::MessagePayload
      */
      MessagePayload(const MessagePayload& payload);
      /**
      * @brief Custom constructor. Copies over all bytes from a message.
      * @param[in] payload (const TPayload&) The object from which to copy the payload data
      * @tparam TPayload (typename) The class that identifies how to interpret the payload data
      */
      template<typename TPayload, typename = typename std::enable_if<eastl::is_base_of<Payload, TPayload>::value>::type>
      MessagePayload(const TPayload& payload);

      /**
      * @brief Getter for the unformatted data component of the message.
      * @return (uint8_t*) A pointer to the first byte of this array.
      */
      uint8_t* data();
      /**
      * @brief Constant getter for the unformatted data component of the message.
      * @return (const uint8_t*) A const-pointer to the first byte of this array.
      */
      const uint8_t* data() const;
      /**
      * @brief Getter for the number of bytes in the payload that contain actual information.
      * @return (uint32_t) The number that is the amount of bytes used.
      */
      uint32_t used_bytes() const;

      /**
      * @brief Conversion function for interpretting the internal data as defined by a class.
      * @tparam TPayload (typename) The class that identifies how to interpret the payload data
      * @return (const TPayload&) A read-only reference to the internal data as the requested type.
      */
      template<typename TPayload, typename = typename std::enable_if<eastl::is_base_of<Payload, TPayload>::value>::type>
      const TPayload& AsFormat() const;

      /**
      * @internal
      * @brief Check if a custom payload type is of acceptable size (at compile-time).
      * @tparam TPayload (typename) The class that identifies how to interpret the payload data
      */
      template<typename TPayload, typename = typename std::enable_if<eastl::is_base_of<Payload, TPayload>::value>::type>
      static constexpr void ValidateSize()
      {
        static_assert(sizeof(TPayload) < networking::editor::kMaxPayloadSize, "TPayload carries a payload bigger than the maximum allowed size");
      }

    private:
      uint8_t data_[networking::editor::kMaxPayloadSize]; //!< The unformatted data component of the message
      uint32_t used_bytes_; //!< The number of bytes in the payload that contain actual information

    };

    /**
    * @struct sulphur::engine::MessageBuffer
    * @brief A small structure that can be used to represent a message whose type is unknown at compile time
    * @author Maarten ten Velden
    */
    struct MessageBuffer
    {
      MessageID id; //!< The unique identifier that specifies the message's type
      MessagePayload payload; //!< The data component of the message
    };

    /**
    * @class sulphur::engine::editor::Message <sulphur::engine::MessageType, TPayload>
    * @brief A structure that makes sending messages easier by providing an interface that wraps the individual message components.
    * @tparam actual_id (MessageID) The unique identifier that specifies the message type
    * @tparam TPayload (typename) The class that implements the data component of the message
    * @remarks For an example of how this might be used see sulphur::engine::editor::PODMessage
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

    //-------------------------------------------------------------------------
    template<typename TPayload, typename>
    inline MessagePayload::MessagePayload(const TPayload& payload) :
      used_bytes_(sizeof(TPayload))
    {
      ValidateSize<TPayload>();
      memcpy_s(data_, sizeof data_, &payload, used_bytes_);
    }

    //-------------------------------------------------------------------------
    template<typename TPayload, typename>
    inline const TPayload& MessagePayload::AsFormat() const
    {
      ValidateSize<TPayload>();
      return reinterpret_cast<const TPayload&>(data_);
    }

    //-------------------------------------------------------------------------
    inline uint8_t* MessagePayload::data()
    {
      return data_;
    }
    //-------------------------------------------------------------------------
    inline const uint8_t* MessagePayload::data() const
    {
      return data_;
    }

    //-------------------------------------------------------------------------
    inline uint32_t MessagePayload::used_bytes() const
    {
      return used_bytes_;
    }
  }
}