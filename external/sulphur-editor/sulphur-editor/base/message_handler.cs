using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Diagnostics;

namespace sulphur
{
  namespace editor
  {
    class MessageHandler : ISubscribeable
    {
      public struct Message
      {
        public native.NetworkMessages id;
        public object data;
      }

      public void Initialize()
      {
        message_thread_ = new Thread(Run);
        run_ = false;
      }

      public void Start()
      {
        run_ = true;
        message_thread_.Start();
      }

      public void Terminate()
      {
        run_ = false;
        message_thread_.Abort();
      }

      public Subscription[] GetSubscriptions()
      {
        return null;
      }

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
                                                                    ID.type_id<MessageHandler>.value());
            OnNotification handler = notify_subscribers;
            handler?.Invoke(this, args);
            ProcessMessages();
          }
          Thread.Sleep(1000/30);
        }
      }

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
                case native.NetworkMessages.kString:
                  message.data = Utils.BytesToStruct<native.messages.StringMessage>(packet.data);
                  break;
                case native.NetworkMessages.kRaycastResult:
                  native.messages.RayCastResultMessage msg =
                    Utils.BytesToStruct<native.messages.RayCastResultMessage>(packet.data);
                  break; 
              }
          
              NotificationEventArgs args = 
                new NotificationEventArgs(message, 
                                          (uint)Notifications.kMessageRecieved,
                                          ID.type_id<MessageHandler>.value());
              handler?.Invoke(this, args);
              break;
            }
          }
          timer.Stop();
          if(timer.ElapsedMilliseconds < 1000L / 60L)
          {
            Thread.Sleep((int)(1000L / 60L - timer.ElapsedMilliseconds));
          }
          timer.Reset();
        }
      }

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

      public enum Notifications : uint
      {
        kConnectionLost,
        kConnected,
        kMessageRecieved
      }


      private bool run_;
      private Thread message_thread_;
      public event OnNotification notify_subscribers;
    }
  }
}
