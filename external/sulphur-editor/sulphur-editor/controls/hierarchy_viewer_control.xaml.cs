using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Shapes;

namespace sulphur
{
  namespace editor
  {
    namespace controls
    {
      /**
       * @class sulphur.editor.controls.HierarchyViewerControl : System.Windows.Controls.UserControl, 
       *   sulphur.editor.ISubscribable
       * @brief The hierarchy viewer XAML code behind for handling events and responding to notifications.
       * @author Maarten ten Velden
       */
      public partial class HierarchyViewerControl : UserControl, ISubscribeable
      {
        private Point start_position; //!< The position at which the cursor pressed the left mouse button.
        private bool is_dragging; //!< True while the user is dragging a tree-view item.

        /**
         * @brief Notification types that can be send to subscribers
         */
        public enum Notifications
        {
          kExplorerDrop,
          kItemDrop
        };
        
        /**
         *@brief sulphur.editor.ISubscribeable.GetSubscriptions() function implementation.
         *@remark this system is not subscribed to any other system  
         */
        public Subscription[] GetSubscriptions()
        {
          return null;
        }

        /**
         *@brief event fired when subscribers need to be notified  
         */
        private event OnNotification notify_subscribers_;

        /**
         *sulphur.editor.ISubscribeable.notify_event implementation 
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
         * @brief Constructor
         */
        public HierarchyViewerControl()
        {
          InitializeComponent();
          
          hierarchy_explorer.PreviewMouseRightButtonDown += HierarchyExplorerPreviewMouseRightButtonDown;
          hierarchy_explorer.PreviewMouseLeftButtonDown += HierarchyExplorerPrevieuwMouseLeftButtonDown;
          hierarchy_explorer.MouseMove += HierarchyExplorerPrevieuwMouseMove;
          hierarchy_explorer.Drop += HierarchyExplorerDrop;
          hierarchy_explorer.SelectedItemChanged += HierarchyExplorerSelectedItemChanged;
        }

        /**
         * @brief Handles the ItemsChanegd event for the hierachy viewer.
         * @param[in] sender (object) The original sender of the event.
         * @param[in] e ( RoutedPropertyChangedEventArgs<object>) Arguments passed with the event.
         */
        private void HierarchyExplorerSelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
          WorldObject selected = e.NewValue as WorldObject;
          native.messages.EntitySelectedMessage msg = new native.messages.EntitySelectedMessage();

          if (selected == null)
          {
            msg.entity_index = UInt64.MaxValue;
          }
          else
          {
            msg.entity_index = (UInt64)selected.GetIndex();
          }

          byte[] data = Utils.StructToBytes(msg);
          native.Networking.SNetSendData(
            (uint)native.NetworkMessages.kEntitySelected,
            data,
            (uint)data.Length);
          native.Networking.SNetFlushPackets();
        }


        /**
        *@brief Handle the PreviewRightMouseButtonDown event on the treeview (hierarchy tree)
        *@param[in] sender (object) The original sender of the event
        *@param[in] e (System.Windows.Input.MouseEventArgs) Event arguments passed when the event was fired
        */
        private void HierarchyExplorerPreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
          // Find the first tree view item
          TreeViewItem item = Utils.GetTreeViewItemInParents(e.OriginalSource as DependencyObject);
          if (item != null)
          {
            item.Focus();
            e.Handled = true;
          }
        }

        /**
         * @brief Initiate drag-and drop of the currently selected item in the hierarchy explorer.
         */
        private void StartDrag()
        {
          is_dragging = true;

          if (hierarchy_explorer.SelectedItem != null)
          {
            DataObject data = new DataObject(
              typeof(WorldObject),
              hierarchy_explorer.SelectedItem);

            DragDropEffects effects = DragDrop.DoDragDrop(
              hierarchy_explorer,
              data,
              DragDropEffects.Move);
          }

          is_dragging = false;
        }

        /**
         * @brief Handle the PreviewMouseLeftButtonDown event on the tree view (hierarchy explorer)
         * @param[in] sender (object) the original sender of the event
         * @param[in] e (System.Windows.Input.MouseButtonEventArgs) event arguments passed when the event was fired
         */
        private void HierarchyExplorerPrevieuwMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
          start_position = e.GetPosition(null);
        }

        /**
         * @brief Handle the PreviewMouseMove event on the tree view (hierarchy explorer)
         * @param[in] sender (object) the original sender of the event
         * @param[in] e (System.Windows.Input.MouseEventArgs) event arguments passed when the event was fired
         */
        private void HierarchyExplorerPrevieuwMouseMove(object sender, MouseEventArgs e)
        {
          if (e.LeftButton == MouseButtonState.Pressed && is_dragging == false)
          {
            Point position = e.GetPosition(null);
            if (Math.Abs(position.X - start_position.X) > SystemParameters.MinimumHorizontalDragDistance ||
              Math.Abs(position.Y - start_position.Y) > SystemParameters.MinimumVerticalDragDistance)
            {
              StartDrag();
            }
          }
        }

        /**
        *@brief Handle the Drop event on the tree view (hierarchy explorer)
        *@param[in] sender (object) the original sender of the event
        *@param[in] e (System.Windows.DragEventArgs) event arguments passed when the event was fired
        */
        private void HierarchyExplorerDrop(object sender, DragEventArgs e)
        {
          NotificationEventArgs args = new NotificationEventArgs(
            e.Data.GetData(typeof(WorldObject)) as WorldObject,
            (int)Notifications.kExplorerDrop,
            id<HierarchyViewerControl>.type_id_ );
          OnNotification handler = notify_subscribers_;
          handler?.Invoke(this, args);
        }

        /**
        *@brief Handle the Drop event on the tree view item (hierarchy item)
        *@param[in] sender (object) the original sender of the event
        *@param[in] e (System.Windows.MouseButtonEventArgs) event arguments passed when the event was fired
        */
        private void HierarchyItemDrop(object sender, DragEventArgs e)
        {
          if (e.Data.GetDataPresent(typeof(WorldObject)) == true)
          {
            WorldObject target = (sender as FrameworkElement).DataContext as WorldObject;
            WorldObject source = e.Data.GetData(typeof(WorldObject)) as WorldObject;

            // Safety-check to see if the target parent is a child of the source
            bool conflict_found = false;
            WorldObject next = target.parent;
            while (next != null)
            {
              if (next == source)
              {
                conflict_found = true;
                break;
              }
              next = next.parent;
            }

            if (conflict_found == false)
            {
              WorldObject[] associates = new WorldObject[2];
              associates[0] = target;
              associates[1] = source;

              NotificationEventArgs args = new NotificationEventArgs(
                associates,
                (int)Notifications.kItemDrop,
                id<HierarchyViewerControl>.type_id_);
              OnNotification handler = notify_subscribers_;
              handler?.Invoke(this, args);
              
              e.Handled = true;
            }
          }
        }
      }
    }
  }
}