using System;
using System.Threading;
using System.Diagnostics;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.MessageHandler : sulphur.editor.ISubscribeable 
     *@brief handles incomming messages from the sulphur-engine.
     *@author Stan Pepels  
     */
    class MessageHandler : ISubscribeable
    {
      /**
       *@struct sulphur.editor.MessageHandler.Message 
       *@brief struct for holding a message retrieved from the networking dll
       *@author Stan Pepels  
       */
      public struct Message
      {
        public native.NetworkMessages id; //!< message id indicated how to interpret the payload
        public object data; //!< actual data send with the message
      }

      /**
       *@brief initializes the system by creating a new thread 
       */
      public void Initialize()
      {
        message_thread_ = new Thread(Run);
        run_ = false;
      }

      /**
       *@brief starts the system polling the message queue and connection status 
       */
      public void Start()
      {
        run_ = true;
        message_thread_?.Start();
      }

      /**
       *@brief stops the system polling the message loop and connection status 
       */
      public void Terminate()
      {
        run_ = false;
        message_thread_?.Abort();
      }

      /**
       *@brief get the subscritions to systems we want recieve notifications from
       *@return (sulphur.editor.Subscription[]) arrray of subscriptions to systems  
       *@remark this system is not subscribed to any other system 
       */
      public Subscription[] GetSubscriptions()
      {
        return null;
      }

      /**
       *@brief handle connects and disconnects 
       */
      private void Run()
      {
        byte[] data = new byte[native.Networking.kPacketSize];
        native.Networking.ErrorMessage msg = 0;
        Message message = new Message();
        UInt32 id = 0;
        while (run_)
        {
          if (native.Networking.SNetIsConnected() == false)
          {
            msg = native.Networking.SNetEditorConnect("localhost", 8008);
            native.Networking.SNetRetrievePacket(ref id, data, (uint)data.Length);       
          }
          else
          {
            NotificationEventArgs args = new NotificationEventArgs(message,
                                                                    (uint)Notifications.kConnected,
                                                                    id<MessageHandler>.type_id_);
            OnNotification handler = notify_subscribers;
            handler?.Invoke(this, args);
            ProcessMessages();
          }
          Thread.Sleep(1000/30);
        }
      }

      /**
       *@brief poll the message queue and process the messages.
       *@remark when a message comes in a notification event is fired to notify other system of the incomming message
       */
      private void ProcessMessages()
      {
        native.Networking.Packet packet = new native.Networking.Packet();
        packet.data = new byte[native.Networking.kPacketSize];
        Stopwatch timer = new Stopwatch();
        OnNotification handler = notify_subscribers;

        Message message = new Message();

        while(native.Networking.SNetIsConnected() == true)
        {
          timer.Start();
          UInt32 id = 0;
          while(native.Networking.SNetRetrievePacket(ref id, packet.data) == true)
          {
            message.id = (native.NetworkMessages)id;
            if (message.id != 0)
            {
              switch (message.id)
              {
                case native.NetworkMessages.kEntityMoved:
                  message.data = Utils.BytesToStruct<native.messages.EntityMovedMessage>(packet.data);
                  break;
                case native.NetworkMessages.kEntityRotated:
                  message.data = Utils.BytesToStruct<native.messages.EntityRotateMessage>(packet.data);
                  break;
                case native.NetworkMessages.kEntityScaled:
                  message.data = Utils.BytesToStruct<native.messages.EntityScaleMessage>(packet.data);
                  break;
              }
          
              NotificationEventArgs args = 
                new NotificationEventArgs(message, 
                                          (uint)Notifications.kMessageRecieved,
                                          id<MessageHandler>.type_id_);
              handler?.Invoke(this, args);
              break;
            }
          }
        }
      }

      /**
       *@brief notify event
       */
      event OnNotification ISubscribeable.notify_event
      {
        add
        {
          notify_subscribers += value;
        }
        remove
        {
          notify_subscribers -= value;
        }      
      } 

      /**
       *@brief different notifications that can be send from this system 
       */
      public enum Notifications : uint
      {
        kConnectionLost, //<! connection with the engine was lost
        kConnected, //<! engine got connected to the editor
        kMessageRecieved //<! engine message recieved
      } 

      private bool run_; //<! indicates if the system is polling the message queue and managing connects and disconnects
      private Thread message_thread_; //<! seperate thread to run the message loop on.

      /**
       *@brief event for notifying subscribed systems 
       */
      public event OnNotification notify_subscribers;
    }
  }
}
