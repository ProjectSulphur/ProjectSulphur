using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;

namespace sulphur
{
  namespace editor
  {
    /**
     * @class sulphur.editor.WorldHierarchy : sulphur.editor.ISubscribable
     * @brief The hierarchy event interface which also owns the actual data.
     * @author Maarten ten Velden
     */
    public class WorldHierarchy : ISubscribeable
    {
      /**
       * @brief Notification types that can be send to subscribers
       */
      public enum Notifications
      {
        kEntityAdded,  //!< A new entity got added to the world hierarchy
        kEntityRemoved //!< A entity got removed from the world hierarchy
      }

      private ICommand unparent_entity_cmd_;   //!< Command for a context menu which sets an entities parent to the root node
      private ICommand new_root_entity_cmd_;   //!< Command for a context menu which creates a new entity without a parent
      private ICommand new_entity_cmd_;        //!< Command for a context menu which creates a new entity parented to the caller
      private ICommand delete_entity_cmd_;     //!< Command for a context menu which deletes an existing entity

      /**
       *@brief Event fired when subscribers need to be notified  
       */
      private event OnNotification notify_subscribers_;

      /**
       * @see sulphur.editor.ISubscribeable.notify_event
       */
      event OnNotification ISubscribeable.notify_event
      {
        add
        {
          notify_subscribers_ += value;
        }
        remove
        {
          notify_subscribers_ -= value;
        }
      }

      /**
       * @brief unparent_entity command property used for binding to a context menu
       * @remark Getting this property calls SetEntityParent with null parameter
       */
      public ICommand unparent_entity_cmd
      {
        get
        {
          return unparent_entity_cmd_ ??
            (unparent_entity_cmd_ = new utils.CommandHandler(
              (object param) => SetEntityParent(param as WorldObject, null),
              true));
        }
      }

      /**
       * @brief new_root_entity command property used for binding to a context menu
       * @remark Getting this property calls NewEntity with null parameter
       */
      public ICommand new_root_entity_cmd
      {
        get
        {
          return new_root_entity_cmd_ ??
            (new_root_entity_cmd_ = new utils.CommandHandler(
              (object param) => NewEntity("New Entity", null),
              true));
        }
      }

      /**
       * @brief new_entity command property used for binding to a context menu
       * @remark Getting this property calls NetEntity
       */
      public ICommand new_entity_cmd
      {
        get
        {
          return new_entity_cmd_ ??
            (new_entity_cmd_ = new utils.CommandHandler(
              (object param) => NewEntity("New Child", param as WorldObject),
              true));
        }
      }

      /**
       * @brief delete_entity command property used for binding to a context menu
       * @remark Getting this property calls DeleteEntity
       */
      public ICommand delete_entity_cmd
      {
        get
        {
          return delete_entity_cmd_ ??
            (delete_entity_cmd_ = new utils.CommandHandler(
              (object param) => DeleteEntity(param as WorldObject),
              true));
        }
      }

      /**
       * @brief Getter of the hierarchy that contains this control's data
       * @see sulphur.editor.WorldObject.Hierarchy
       */
      public WorldObject.Hierarchy hierarchy { get; }

      /**
       * @brief Constructor.
       */
      public WorldHierarchy()
      {
        hierarchy = new WorldObject.Hierarchy();
      }

      /**
       * @see sulphur.editor.ISubscribeable.GetSubscriptions
       * @remark This system is subscribed to the hierarchy viewer control.
       */
      public Subscription[] GetSubscriptions()
      {
        Subscription[] subscriptions = new Subscription[2];
        subscriptions[0] = new Subscription(id<controls.HierarchyViewerControl>.type_id_, OnNotification);
        subscriptions[1] = new Subscription(id<MessageHandler>.type_id_, HandleEngineMessage);
        return subscriptions;
      }

      /**
       * @brief Handle incomming messages from the attached engine(s).
       * @param[in] sender (object) The messageing system that recieved the message from the engine.
       * @param[in] e (NotificationEventArgs) Argument send with the event. The data of this evenent in this case contains the allready converted message.
       * @see sulphur.editor.base.MessageHandler
       */
      void HandleEngineMessage(object sender, NotificationEventArgs e)
      {
        MessageHandler.Message msg = (MessageHandler.Message)e.notification_data;
        if (e.notification_id == (uint)MessageHandler.Notifications.kMessageRecieved)
        {
          switch (msg.id)
          {
            case native.NetworkMessages.kEntityMoved:
              {
                native.messages.EntityMovedMessage data = (native.messages.EntityMovedMessage)msg.data;
                byte[] byte_data = Utils.StructToBytes(data);
                native.Networking.SNetSendData((uint)msg.id, byte_data, (uint)byte_data.Length);
                native.Networking.SNetFlushPackets();
                break;
              }
            case native.NetworkMessages.kEntityRotated:
              {
                native.messages.EntityRotateMessage data = (native.messages.EntityRotateMessage)msg.data;
                byte[] byte_data = Utils.StructToBytes(data);
                native.Networking.SNetSendData((uint)msg.id, byte_data, (uint)byte_data.Length);
                native.Networking.SNetFlushPackets();
                break;
              }
            case native.NetworkMessages.kEntityScaled:
              {
                native.messages.EntityScaleMessage data = (native.messages.EntityScaleMessage)msg.data;
                byte[] byte_data = Utils.StructToBytes(data);
                native.Networking.SNetSendData((uint)msg.id, byte_data, (uint)byte_data.Length);
                native.Networking.SNetFlushPackets();
              }
              // send modification to engine that data has been modified
              break;
          }
        }
      }
      /**
       *@brief handle incomming notifications from other systems
       *@param[in] sender (object) the original sender of the notification
       *@param[in] e (sulphut.editor.NotificationEventArgs) event aruments passed when the event was fired
       */
      private void OnNotification(object sender, NotificationEventArgs e)
      {
        switch ((controls.HierarchyViewerControl.Notifications)e.notification_id)
        {
          case controls.HierarchyViewerControl.Notifications.kExplorerDrop:
            unparent_entity_cmd.Execute(e.notification_data);
            break;
          case controls.HierarchyViewerControl.Notifications.kItemDrop:
            SetEntityParent(
              (e.notification_data as WorldObject[])[1],
              (e.notification_data as WorldObject[])[0]);
            break;
        }
      }

      /**
       * @brief Implementation of the set_entity_parent_cmd_ and unparent_entity_cmd_ commands
       * @param[in] target (sulphur.editor.) The parameter passed to the
       *   command. This parameter is the transform component of the entity for
       *   which to modify the parent.
       */
      public void SetEntityParent(WorldObject target, WorldObject parent)
      {
        UInt64 old_index = (UInt64)target.GetIndex();
        UInt64 new_parent_old_index = parent != null ? (UInt64)parent.GetIndex() : UInt64.MaxValue;

        hierarchy.SetParent(target, parent);

        // Notify the engine
        native.messages.EntityReparentedMessage msg = new native.messages.EntityReparentedMessage();
        msg.entity_old_index = old_index;
        msg.new_parent_old_index = new_parent_old_index;
        msg.new_sibling_index = (UInt64)target.GetSiblingIndex();

        byte[] data = Utils.StructToBytes(msg);
        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kEntityReparented,
          data,
          (uint)data.Length);
        native.Networking.SNetFlushPackets();
      }

      /**
       * @brief Implementation of the new_root_entity_cmd_ and new_entity_cmd_ commands
       * @param[in] name (string) The new entity's display name.
       * @param[in] parent (sulphur.editor.WorldObject) The parameter passed to the
       *   command. This parameter will become the parent for the newly created
       *   entity.
       * @remarks Passing null for the parent will parent the entity to the root.
       */
      public void NewEntity(string name, WorldObject parent)
      {
        WorldObject new_object = hierarchy.Create(name, parent);

        OnNotification handler = notify_subscribers_;
        NotificationEventArgs args = new NotificationEventArgs(
          new_object,
          (uint)Notifications.kEntityAdded,
          id<WorldHierarchy>.type_id_);
        handler?.Invoke(this, args);

        // Notify the engine
        native.messages.EntityCreatedMessage msg = new native.messages.EntityCreatedMessage();
        msg.entity_index = (UInt64)new_object.GetIndex();
        msg.sibling_index = (UInt64)new_object.GetSiblingIndex();
        msg.parent_index = parent != null ? (UInt64)parent.GetIndex() : UInt64.MaxValue;

        byte[] data = Utils.StructToBytes(msg);
        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kEntityCreated,
          data,
          (uint)data.Length);
        native.Networking.SNetFlushPackets();
      }

      /**
       * @brief Implementation of the delete_entity_cmd_ command
       * @param[in] target (sulphur.editor.WorldObject) The parameter passed to
       *   the command. This parameter is the entity to delete.
       */
      public void DeleteEntity(WorldObject target)
      {
        UInt64 index = (UInt64)target.GetIndex();

        hierarchy.Delete(target);

        OnNotification handler = notify_subscribers_;
        NotificationEventArgs args = new NotificationEventArgs(
          index,
          (uint)Notifications.kEntityRemoved,
          id<WorldHierarchy>.type_id_);
        handler?.Invoke(this, args);

        // Notify the engine
        native.messages.EntityDestroyedMessage msg = new native.messages.EntityDestroyedMessage();
        msg.entity_index = index;

        byte[] data = Utils.StructToBytes(msg);
        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kEntityDestroyed,
          data,
          (uint)data.Length);
        native.Networking.SNetFlushPackets();
      }
    }
  }
}
