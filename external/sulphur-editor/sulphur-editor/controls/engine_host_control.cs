using System.Windows.Controls;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System;

namespace sulphur
{
  namespace editor
  {
    namespace controls
    {
      public class EngineHostControl : Border, ISubscribeable
      {
        /*public members*/
        public static void SetDefaultStyleProperties(){}

        public EngineHostControl()
        {
          SetDefaultStyleProperties();
          engine_ = new EngineHost((uint)ActualWidth, (uint)ActualHeight);
          Child = engine_;
        }

        #region ISubscribeable
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

        public Subscription[] GetSubscriptions()
        {
          Subscription[] subscritions = new Subscription[1];
          subscritions[0] = new Subscription(ID.type_id<MessageHandler>.value(),
                                             HandleNetworkedMessage);
          return subscritions;
        }
        #endregion

        /*private members*/
        private EngineHost engine_;

        private void HandleNetworkedMessage(object sender, NotificationEventArgs e)
        {
          if (e.type_id == ID.type_id<MessageHandler>.value())
          {
            switch ((MessageHandler.Notifications)e.notification_id)
            {
              case MessageHandler.Notifications.kConnected:
                byte[] data = Utils.StructToBytes(engine_.window.ToInt64());
                native.Networking.ErrorMessage msg = 
                  native.Networking.SNetSendData((UInt32)native.NetworkMessages.kWindowHandle,
                                                 data,
                                                 (uint)data.Length);
                break;
            }
          }
        }

        private event OnNotification notify_subscribers_;
      }

      public class EngineHost : HwndHost
      {
        /*public members*/
        public IntPtr window { get; private set; }

        public EngineHost(uint width, uint height)
        {
          width_ = width;
          height_ = height;
        }

        /*protected members*/
        protected override HandleRef BuildWindowCore(HandleRef handle)
        {
          window_proc_ = WndProc;

          native.Win32.Window window_class = new native.Win32.Window();
          window_class.style = 0x0002 | 0x0001;
          window_class.background = (IntPtr)6; // background color
          window_class.class_name = "HostedWindow";
          window_class.window_procedure = Marshal.GetFunctionPointerForDelegate(window_proc_);
          native.Win32.RegisterClassW(ref window_class);

          int style = 0;
          style |= 0x40000000; //WS_CHILD
          style |= 0x10000000; // WS_VISIBLE
          style |= 0x00800000; //WS_BORDER 

          window = IntPtr.Zero;
          window = native.Win32.CreateWindowEx(0,
                                                "HostedWindow",
                                                string.Empty,
                                                style,
                                                0,
                                                0,
                                                (int)width_,
                                                (int)height_,
                                                handle.Handle,
                                                IntPtr.Zero,
                                                IntPtr.Zero,
                                                IntPtr.Zero);
          engine_process_ = new Process();
          engine_process_.StartInfo.FileName = ".\\sulphur-engine.exe";
          engine_process_.StartInfo.UseShellExecute = false;
          engine_process_.Start();
          return new HandleRef(this, window);
        }

        protected override void DestroyWindowCore(HandleRef handle)
        {
          native.Win32.DestroyWindow(window);
          if (engine_process_ != null && engine_process_.HasExited == false)
          {
            engine_process_.Kill();
          }
        }

        /*private members*/
        private uint width_;

        private uint height_;

        private window_proc window_proc_;

        private Process engine_process_;

        private delegate IntPtr window_proc(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam);

        private IntPtr WndProc(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam)
        {
          switch (msg)
          {
            case WM_LBUTTONDOWN:
              native.messages.MouseClickMessage click = new native.messages.MouseClickMessage();
              click.x_pos = GetXLParam(lparam);
              click.y_pos = GetYLParam(lparam);
              click.id = 0;
              byte[] data = Utils.StructToBytes(click);
              native.Networking.SNetSendData((uint)native.NetworkMessages.kClickRaycast, data, (uint)data.Length);
              break;
          }

          return native.Win32.DefWindowProcW(hwnd, msg, wparam, lparam);// default msg handling
        }

        private int GetXLParam(IntPtr lparam)
        {
          return LOWORD(lparam.ToInt32());
        }

        private int GetYLParam(IntPtr lparam)
        {
          return HIWORD(lparam.ToInt32());
        }

        private int LOWORD(int i)
        {
          return (short)(i & 0xFFFF);
        }

        private int HIWORD(int i)
        {
          return (short)(i >> 16);
        }
        const uint WM_LBUTTONDOWN = 0x0201;
      }
    } // controls
  } // editor 
} // sulphur
