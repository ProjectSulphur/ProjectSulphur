#pragma once

#include "engine/networking/message_in.h"

#include <foundation/containers/vector.h>

#include <cinttypes>

namespace sulphur 
{
  namespace engine 
  {
    class MessagePayload;

    /**
    * @class sulphur::engine::ConnectionManager
    * @brief A class that handles the connection to and communication with the editor
    * @author Maarten ten Velden
    */
    class ConnectionManager
    {
    public:
      /**
      * @brief Destructor which disconnects safely
      * @param[in] as_editor (bool) A flag to indicate that the application should emulate the editor,
      *                             in which case a connection is made to an engine instead of an editor.
      */
      ConnectionManager(bool as_editor = false);
      ~ConnectionManager();

      /**
      * @brief Check if there are messages left to be processed
      * @param[out] out_id (sulphur::engine::MessageID) The memory in which the unique identifier that specifies the incoming message's type (if any) will be copied
      * @param[out] out_payload (sulphur::engine::MessagePayload*) The memory in which the payload of the incoming message (if any) will be copied
      * @return (bool) True if there are unprocessed messages left
      */
      bool RetrieveNextMessage(MessageID* out_id, MessagePayload* out_payload);
      /**
      * @brief Send a message to the editor
      * @param[in] id (sulphur::engine::MessageID) The unique identifier that specifies the message's type
      * @param[in] payload (const sulphur::engine::MessagePayload&) The payload of the message
      */
      void SendMessage(MessageID id, const MessagePayload& payload);
      /**
      * @brief Dispatch all messages immediately
      */
      void FlushMessages();

      /**
      * @brief Setup the connection properties before connection
      * @remarks Outputs a warning if there is already a connection.
      * @param[in] port (uint32_t) The port on which the connection will be made
      * @return (bool) True if a connection was established, false if failed for some reason.
      */
      bool Initialize(uint32_t port = 8008u);
      /**
      * @brief Connect to a running editor
      * @remarks Outputs a warning if there is already a connection.
      * @return (bool) True if a connection was established, false if failed for some reason.
      */
      bool Connect();
      /**
      * @brief Disconnect from the editor currently connected to. 
      * @remarks Does nothing if there is no connection.
      */
      void Disconnect();
      /**
      * @brief Test whether or not the engine is connected to an editor
      * @return (bool) True if a connection is established
      */
      bool IsConnected() const;

    private:
      /**
      * @brief Retrieve all network-sent messages and store them locally
      */
      void RetrieveMessages();

      static constexpr float kTimeOutDelay = 5.0f; //!< This value indicates how long the manager will wait for a connection before cancelling.

      uint32_t current_port = UINT32_MAX;
      foundation::Vector<MessageBuffer> unprocessed_messages;

      const bool as_editor_; //!< A flag to indicate that the application should emulate the editor, in which case a connection is made to an engine instead of an editor.
    };
  }
}