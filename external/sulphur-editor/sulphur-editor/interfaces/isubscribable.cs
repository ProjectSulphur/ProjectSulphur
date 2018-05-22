using System;
using System.Collections.Generic;
namespace sulphur
{
  namespace editor
  {
    public sealed class SubscriberIDComparer : IEqualityComparer<SubscriberID>
    {
      public bool Equals (SubscriberID first, SubscriberID second)
      {
        return first.type_id == second.type_id;
      }
      public int GetHashCode(SubscriberID obj)
      {
        return obj.type_id.GetHashCode();
      }
    }

    public class ID
    {
      // works because statics are only initialized onced even across multiple objects
      // also a new class is generated for every T used
      public class type_id<T> where T : class
      {
        static public uint value() { return id; }
        private static uint id = type_count_++;
      }
      private static uint type_count_ = 0;
    }

    // identification for a subscriber participation in communication between tools
    public class SubscriberID
    {
      public SubscriberID(uint type)
      {
        type_id = type;
      }
    
      public readonly uint type_id;
    }

    // class describing a subscription with a delegate fired with OnNotifyEvent
    // and a target id representing the system to subscribe to and an id representing the subscriber
    public class Subscription
    {
      public Subscription(uint target_type_id, OnNotification func)
      {
        target_id = new SubscriberID(target_type_id);
        callback = func;
      }

      public readonly OnNotification callback;
      public readonly SubscriberID target_id;
    }

    public class NotificationEventArgs : EventArgs
    {
      public NotificationEventArgs(object data, uint id, uint type)
      {
        notification_id = id;
        notification_data = data;
        type_id = type;
      }
      public readonly uint notification_id;
      public readonly object notification_data;
      public readonly uint type_id;
    }

    public delegate void OnNotification(object sender, NotificationEventArgs e);

    public interface ISubscribeable
    {
      // event fired to notify subscribed systems of an event
      event OnNotification notify_event;

      // return all subscriptions this object is allegeable to
      Subscription[] GetSubscriptions();

    }
  }
}
