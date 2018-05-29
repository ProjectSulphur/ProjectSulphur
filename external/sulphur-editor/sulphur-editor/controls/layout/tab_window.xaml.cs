using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.TabWindow : Window, sulphur.editor.ISubscribeable
   * @brief Window that contains detached editor controls.
   * @see sulphur.editor.ISubscribeable.
   * @author Stan Pepels
   */
  public partial class TabWindow : Window, ISubscribeable
  {
    /**
     * @brief Notifications that this class can send
     */
    public enum Notifications
    {
      kDragStart, //!< The user started dragging the window.
      kDragEnd, //!< The user has stopped dragging the window.
      kDragMove, //!< The window has been moved.
      kDrop //!< The window has been dropped.
    }

    #region ISubscribeable
    /**
     * @brief sulphur.editor.ISubscribeable.GetSubscriptions function implementation.
     * @remark This system is not subscribed to any other system.  
     * @see sulphur.editor.Subscription.
     */
    public Subscription[] GetSubscriptions()
    {
      return null;
    }

    /**
      *@see sulphur.editor.ISubscribeable.notify_event implementation.
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
       *@brief event fired when subscribers need to be notified.
       *@see sulphur.editor.OnNotification 
       */
    private event OnNotification notify_subscribers_;
    #endregion

    /**
     * @brief Get the tabs currently in this window
     * @return (List<object>) All tab items in this window.
     */
    public List<object> GetTabs()
    {
      List<object> col = new List<object>();
      foreach (object o in ((DynamicDockTabControl)Content).Items)
      {
        col.Add(o);
      }
      return col;
    }

    /**
     * @brief Constructor whicch sets up the window procedure.
     * @param[in | opt] A tab item to add to the window on startup.
     * @see sulphur.editor.controls.DynamicDockTab
     */
    public TabWindow(DynamicDockTab item = null)
    {
      WindowStyle = WindowStyle.ToolWindow;
      InitializeComponent();
      SizeToContent = SizeToContent.WidthAndHeight;
      Show();
      Owner = Application.Current.MainWindow;
      HwndSource source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
      source.AddHook(new HwndSourceHook(WndProc));
      Content = new DynamicDockTabControl(false);
      (Content as DynamicDockTabControl).items_changed += TabWindow_items_changed;
      if (item != null)
      {
        (Content as DynamicDockTabControl).Items.Add(item);
      }
    }

    /**
     * @brief Removes all tabs from this window.
     */
    public void UnparentTabs()
    {
      ((DynamicDockTabControl)Content).Items.Clear();
    }

    private bool drag_start_send_ = false; //!< Indicates that a drag start message has been send.

    private Action dragged_callback; //!< Callback called when the window is being dragged across the screen.

    /**
     * @brief Event handler for the ItemsChanged event.
     * @param[in] sender (object) Object that send the event.
     * @param[in] e (Eventargs) Default event arguments.
     */
    private void TabWindow_items_changed(object sender, EventArgs e)
    {
      if ((Content as DynamicDockTabControl).Items.Count == 0)
      {
        Close();
      }
    }

    /**
     * @brief Window procedure for this window. Handles WM_ENTERSIZEMOVED, WM_EXITSIZEMOVED and WM_MOVE.
     * @param[in] hwnd (IntPtr) Window handle of the window the procedure is attached to.
     * @param[in] msg (int) message id.
     * @param[in] wparam (IntPtr) wparam send with the message.
     * @param[in] lparam (IntPtr) lparam send with the message 
     * @param[out] handled (ref bool) True if the message to be processed was handled. False otherwise
     * @return (IntPtr) Always IntPtr.Zero
     * @see https://msdn.microsoft.com/en-us/library/windows/desktop/ms632593(v=vs.85).aspx
     */
    private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wparam, IntPtr lparam, ref bool handled)
    {
      OnNotification handler = notify_subscribers_;
      switch (msg)
      {
        case 0x0231: //WM_ENTERSIZEMOVED
          {
            drag_start_send_ = false;
          }
          break;
        case 0x0232: //WM_EXITSIZEMOVED
          {
            if (drag_start_send_ == true)
            {
              NotificationEventArgs args = new NotificationEventArgs(null, (uint)Notifications.kDrop, id<TabItem>.type_id_);
              handler?.Invoke(this, args);

              args = new NotificationEventArgs(null, (uint)Notifications.kDragEnd, id<TabItem>.type_id_);
              handler?.Invoke(this, args);
              dragged_callback = null;
            }
          }
          break;
        case 0x0003: //WM_MOVE
          if (drag_start_send_ == false)
          {
            NotificationEventArgs args = new NotificationEventArgs(null, (uint)Notifications.kDragStart, id<TabItem>.type_id_);
            handler?.Invoke(this, args);
            drag_start_send_ = true;
            dragged_callback = Dragged;
          }
          dragged_callback?.Invoke();
          break;
      }
      return IntPtr.Zero;
    }

    /**
     * @brief Sends an notification that the window was dragged.
     */
    private void Dragged()
    {
      OnNotification handler = notify_subscribers_;
      NotificationEventArgs args = new NotificationEventArgs(null, (uint)Notifications.kDragMove, id<TabItem>.type_id_);
      handler?.Invoke(this, args);
    }
  }
}
