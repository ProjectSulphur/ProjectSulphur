using System.Collections.Generic;

namespace sulphur
{
  namespace editor
  {
    class SubscriptionHandler
    {
      public static void Register<T>(T obj) where T : class, ISubscribeable
      {
        ISubscribeable subscribable = obj;
        Subscription[] subs = subscribable.GetSubscriptions();
        SubscriberID id = new SubscriberID(ID.type_id<T>.value());
        if(systems_by_type_.ContainsKey(id) == false)
        {
          systems_by_type_.Add(id, new List<ISubscribeable>());
        }

        systems_by_type_[id].Add(subscribable);

        if (subs != null)
        {
          ProcessSubscriptions(subs, ref subscribable);
        }
        SubscribeTo(id, ref subscribable);
      }

      public static void Unregister<T>(T obj) where T : class, ISubscribeable
      {
        Subscription[] subs = obj.GetSubscriptions();
        SubscriberID id = new SubscriberID(ID.type_id<T>.value());
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

      private static void ProcessSubscriptions(Subscription[] subs, ref ISubscribeable obj)
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

      private static void SubscribeTo(SubscriberID id, ref ISubscribeable obj)
      {
        Dictionary<SubscriberID, List<ISubscribeable>>.Enumerator it = systems_by_type_.GetEnumerator();
       while(it.MoveNext() == true)
        {
          Subscription[] subs = it.Current.Value[0].GetSubscriptions();
          if(subs == null)
          {
            continue;
          }

          bool is_subscribed_to_obj = false; 
          uint index = 0;
          foreach (Subscription sub in subs)
          {
            if (sub.target_id.type_id == id.type_id)
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

      private static Dictionary<SubscriberID, List<ISubscribeable>> systems_by_type_ = new Dictionary<SubscriberID, List<ISubscribeable>>(new SubscriberIDComparer());
    }
  }
}
