#pragma once
#include <tools/networking/export.h>

#include <cstring>

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
      kWindowHandle, //!< A window handle to render to
      kEntityCreated, //!< The creation of a new entity
      kEntityDestroyed, //!< The destruction of an existing entity
      kComponentAdded, //!< The creation of a new component on an entity
      kComponentRemoved, //!< The destruction of an existing component on an entity
      kClickRaycast, //!< A request for a raycast hit result from the viewport of a camera component
      kRaycast, //!< A request for a raycast hit result from world-space coordinates
      kRaycastResult //!< A hit result of a raycast
    };
    constexpr int kNumMessageTypes = static_cast<int>(MessageID::kRaycastResult) + 1; //!< The amount of unique message types defined.

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
    * @see sulphur::engine::Message::SendLocal
    * @see sulphur::engine::Message::SendToEditor
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
      * @param[in] payload (const sulphur::engine::MessagePayload&) The object from which to copy the data over
      * @see sulphur::engine::MessagePayload
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