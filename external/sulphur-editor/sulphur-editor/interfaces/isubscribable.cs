using System;
using System.Collections.Generic;
namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.SubscriberIDComparer : IEqualityComparer<SubscriberID>
     *@brief compares to sulphur.editor.SubscriberID instances
     *@author Stan Pepels  
     */
    public sealed class SubscriberIDComparer : IEqualityComparer<SubscriberID>
    {
      /**
       *@brief compares the type ids from 2 sulphur.editor.SubscriberID instances 
       *@param[in] first (sulphur.editor.SubscriberID) first instance in the comparison
       *@param[in] second (sulphur.editor.SubscriberID) instance to compare the first instance to
       *@return (bool) result of comparing type id from both instances 
       */
      public bool Equals (SubscriberID first, SubscriberID second)
      {
        return first.type_id == second.type_id;
      }

      /**
       *@brief get a hash code from a sulphur.editor.SubscriberID instance
       *@param[in] obj (sulphur.editor.SubscriberID) instamce to get hash code from 
       */
      public int GetHashCode(SubscriberID obj)
      {
        return obj.type_id.GetHashCode();
      }
    }

    /**
     *@class sulphur.editor.type_count
     *@brief class holding the number of types used with the sulphur.editor.id<T> class  
     *@author Stan Pepels  
     */
    public class type_count
    {
      /**
       *@brief current count of types registered.
       */
      public static uint value_ { get; protected set; } = 0;
    }

    /**
     *@class sulphur.editor.id<T> : sulphur.editor.type_count 
     *@brief class used for getting the unique type id of a type T
     *@author Stan Pepels  
     */
    public class id<T> : type_count
    {
      /**
       *@brief type id for type T. whenever a class with type T is used the type count is incremented by 1.
       * @see sulphur.esitor.type_count.value
       * 
       */
      public static uint type_id_{ get; } = type_count.value_++;
    }
    /**
     *@class sulphur.editor.SubscriberID
     *@brief class representing a system that uses the subscriber system
     *@see sulphur.editor.SubscriptionHandler
     *@author Stan Pepels  
     */
    public class SubscriberID
    {
      /**
       *@brief constructor
       *@param[in] type (uint) type id of the subscriber   
       */
      public SubscriberID(uint type)
      {
        type_id = type;
      }
    
      public readonly uint type_id; //!< subscriber type id
    }

    /**
     *@class sulphur.editor.Subscription 
     *@brief class representing a subscription to a class. A subscription is defined by a target event and delegate.
     *@author Stan Pepels 
     */
    public class Subscription
    {
      /**
       *@brief constructor that initializes the readonly values from this class.
       *@param[in] target_type_id (uint) type id of the system you want to subscribe to
       *@param[in] func (sulphur.editor.OnNotification) function to call when the system subscribed to gives a notification 
       */
      public Subscription(uint target_type_id, OnNotification func)
      {
        target_id = new SubscriberID(target_type_id);
        callback = func;
      }

      public readonly OnNotification callback; //!< function used to handle a notifications from a system. This function is fired for every notificatoin fired by the target system types.
      public readonly SubscriberID target_id; //!< type id of the system to recieve notifications from
    }

    /**
     *@class sulphur.editor.NotificationEventArgs :  EventArgs
     *@brief class passed with the notify event holding information relevant to the event.
     *@author Stan Pepels 
     */
    public class NotificationEventArgs : EventArgs
    {
      /**
       *@brief constructor
       *@param[in] data (object) notification data used for processing the notification
       *@param[in] id (uint) type id of the system that send the notification. usefull if a single function is used to process messages from different system types.
       *@param[in] type (uint) type id of the system sending the notification    
       */
      public NotificationEventArgs(object data, uint id, uint type)
      {
        notification_id = id;
        notification_data = data;
        type_id = type;
      }
      public readonly uint notification_id; //!< notification identifier
      public readonly object notification_data; //!< additional send with the event
      public readonly uint type_id; //!< the type of system that send the notification
    }

    /**
     *@brief delegate for notification handlers 
     */
    public delegate void OnNotification(object sender, NotificationEventArgs e);

    /**
     *@interface sulphur.editor.ISubscribeable 
     *@brief interface that must be implemented in order to use the subscription system with a class
     *@author Stan Pepels  
     */
    public interface ISubscribeable
    {
      /**
       *@brief notification event which triggers handlers on systems subscribed to this system 
       */
      event OnNotification notify_event;

      /**
       *@brief get the subscriptions for all systems you want to recieve notifications from
       *@return (sulphur.editor.Subscription[]) array of subscriptions  
       */
      Subscription[] GetSubscriptions();
    }
  }
}
