using System;
using System.Windows;
using System.Windows.Interop;

namespace sulphur
{
  namespace editor
  {

    public partial class MainWindow : Window, ISubscribeable
    {
      private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wparam, IntPtr lparam, ref bool handled)
      {
        OnNotification handler = notify_subscribers_;
        NotificationEventArgs args = new NotificationEventArgs(null, (uint)msg, id<MainWindow>.type_id_);

        switch (msg)
        {
          case 0x0232: //WM_EXITSIZEMOVED  
          notify_subscribers_?.Invoke(this, args);
          break;
        default:
          return IntPtr.Zero;
        }
        return IntPtr.Zero;
      }
      public MainWindow()
      {

      }

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
        *@brief event fired when subscribers need to be notified  
        */
      private event OnNotification notify_subscribers_;

      /**
         *@brief sulphur.editor.ISubscribeable.GetSubscriptions() function implementation.
         *@remark this system is subscribed to the sulphur.editor.MessageHandler
         */
      public Subscription[] GetSubscriptions()
      {
        return null;
      }


      public void Initialize()
      {
        InitializeComponent();
        SubscriptionHandler.Register(menu_bar);
        SubscriptionHandler.Register(dynamic_dock);
        SubscriptionHandler.Register(engine);
        SubscriptionHandler.Register(asset_browser);
        SubscriptionHandler.Register(hierarchy_viewer);
        asset_browser.SetInspector(inspector);
      }

      public void AddWndProc()
      {
        HwndSource source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
        source.AddHook(new HwndSourceHook(WndProc));
      }
    }
  }
}
