using System.Collections.Generic;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.SubscriptionHandler 
     *@brief this class handles the application subscriptions. every class implmenting the sulphur.editor.ISubscribable 
     * interface must be registered with this class in order to link up the events and delegates properly. 
     *@author Stan Pepels 
     */
    class SubscriptionHandler
    {
      /**
       *@brief register a class of type T with this system
       *@param[in] obj (T) refrence object to register
       *@remark the object must implement the sulphur.editor.ISubscribable else a compile error is thrown  
       */
      public static void Register<T>(T obj) where T : class, ISubscribeable
      {
        ISubscribeable subscribable = obj;
        Subscription[] subs = subscribable.GetSubscriptions();
        SubscriberID id = new SubscriberID(id<T>.type_id_);
        if(systems_by_type_.ContainsKey(id) == false)
        {
          systems_by_type_.Add(id, new List<ISubscribeable>());
        }

        systems_by_type_[id].Add(subscribable);

        if (subs != null)
        {
          ProcessSubscriptions(subs);
        }
        SubscribeTo(ref obj);
      }

      /**
      *@brief unregister a class of type T with this system
      *@param[in] obj (T) refrence object to unregister
      *@remark the object must implement the sulphur.editor.ISubscribable else a compile error is thrown
      *@remark in order for e registered object to become eledagble for garbage collection it must be unregistered from this system   
      */
      public static void Unregister<T>(T obj) where T : class, ISubscribeable
      {
        Subscription[] subs = obj.GetSubscriptions();
        SubscriberID id = new SubscriberID(id<T>.type_id_);
        systems_by_type_[id].Remove(obj);

        foreach(Subscription sub in subs)
        {
          foreach(ISubscribeable entry in systems_by_type_[sub.target_id])
          {
            entry.notify_event -= sub.callback;
          }
        }

        Dictionary<SubscriberID, List<ISubscribeable>>.Enumerator it = systems_by_type_.GetEnumerator();
        while(it.MoveNext() == true)
        {
          Subscription[] subscriptions = it.Current.Value[0].GetSubscriptions();
          if(subscriptions == null)
          {
            continue;
          }

          bool is_subscribed_to_obj = false;
          uint index = 0;
          foreach (Subscription sub in subscriptions)
          {
            if (sub.target_id == id)
            {
              is_subscribed_to_obj = true;
              break;
            }
            ++index;
          }

          if (is_subscribed_to_obj == true)
          {
            foreach (ISubscribeable registered in it.Current.Value)
            {
              obj.notify_event -= registered.GetSubscriptions()[index].callback;
            }
          }
        }
      }

      /**
       *@brief process the subscription retrieved by calling sulphur.editor.ISubscribable.GetSubscriptions function and link up the call-backs with allready registered systems 
       *@param[in] subs (sulphur.editor.Subscription[]) subscriptions to be linked up 
       */
      private static void ProcessSubscriptions(Subscription[] subs)
      {
        for (int i = 0; i < subs.Length; ++i)
        {
          if(systems_by_type_.ContainsKey(subs[i].target_id) == false)
          {
            continue;
          }

          List<ISubscribeable> objects = systems_by_type_[subs[i].target_id];
          for (int j = 0; j < objects.Count; j++)
          {
            objects[j].notify_event += subs[i].callback;
          }
        }
      }

      /**
       *@brief check all previously registered systems if they are subscribed to an object and if so link up the events and callbacks.
       *@param[in] obj (T) object to subscribe all previously registered objects to.
       */
      private static void SubscribeTo<T>(ref T obj) where T : class, ISubscribeable
      {
        Dictionary<SubscriberID, List<ISubscribeable>>.Enumerator it = systems_by_type_.GetEnumerator();
        while (it.MoveNext() == true)
        {
          Subscription[] subs = it.Current.Value[0].GetSubscriptions();
          if (subs == null)
          {
            continue;
          }

          bool is_subscribed_to_obj = false;
          uint index = 0;
          foreach (Subscription sub in subs)
          {
            if (sub.target_id.type_id == id<T>.type_id_)
            {
              is_subscribed_to_obj = true;
              break;
            }
            ++index;
          }

          if (is_subscribed_to_obj == true)
          {
            foreach (ISubscribeable registered in it.Current.Value)
            {
              obj.notify_event += registered.GetSubscriptions()[index].callback;
            }
          }
        }
      }

      private static Dictionary<SubscriberID, List<ISubscribeable>> systems_by_type_ = 
        new Dictionary<SubscriberID, List<ISubscribeable>>(new SubscriberIDComparer()); //<! dictionary of registered systems sorted by there type_id
    }
  }
}
