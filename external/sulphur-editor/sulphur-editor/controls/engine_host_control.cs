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
      /**
       *@class sulphur.editor.controls.EngineHostControl : Border, sulphur.editor.ISubscribeable
       *@brief control serving as the 'Scene view' this control reserves an area in the application for the engine to render to
       *@author Stan Pepels  
       */
      public class EngineHostControl : Border, ISubscribeable
      {
        /**
         *@brief constructor
         *@remark this is where the actual engine host control gets created  
         */
        public EngineHostControl()
        {
          engine_ = new EngineHost((uint)ActualWidth, (uint)ActualHeight);
          Child = engine_;
        }

        #region ISubscribeable
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
         *@brief sulphur.editor.ISubscribeable.GetSubscriptions() function implementation.
         *@remark this system is subscribed to the sulphur.editor.MessageHandler
         */
        public Subscription[] GetSubscriptions()
        {
          Subscription[] subscritions = new Subscription[1];
          subscritions[0] = new Subscription(id<MessageHandler>.type_id_,
                                             HandleNetworkedMessage);
          return subscritions;
        }
        #endregion

        private EngineHost engine_; //<! control spawning the engine and handling the reserved window

        /**
         *@brief handle incomming notifiactaion from the message handler 
         *@param[in] sender (object) the original sender of the notification
         *@param[in] e (sulphut.editor.NotificationEventArgs) event aruments passed when the event was fired
         */
        private void HandleNetworkedMessage(object sender, NotificationEventArgs e)
        {
          if (e.type_id == id<MessageHandler>.type_id_)
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

        /**
         *@brief event fired when subscribers need to be notified  
         */
        private event OnNotification notify_subscribers_;
      }

      /**
       *@class sulphur.editor.controls.EngineHost : HwndHost 
       *@brief class handling the external engine. This class is also reponsible for creating a window for the engine to render on
       *@author Stan Pepels 
       */
      public class EngineHost : HwndHost
      {
        /**
         *@brief handle to the engine window this is equivalent to an HWND 
         */
        public IntPtr window { get; private set; }

        /**
         *@brief constructor.
         *@param[in] width (uint) width of the control on initialization
         *@param[in] height (uint) height of the control on initialization   
         */
        public EngineHost(uint width, uint height)
        {
          width_ = width;
          height_ = height;
        }

        /**
         *@brief created the window and spawns the engine process
         *@param[in] handle (HandleRef) handle to the control the new window must be parented to.
         *@return (HandleRef) newly created window
         *@remark the returned handle must always be aprented to another handle else it will be invalid and an exception will be thrown by WPF 
         */
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
          engine_process_.StartInfo.FileName = App.app_directory + "\\sulphur-engine.exe";
          engine_process_.StartInfo.UseShellExecute = false;
          engine_process_.StartInfo.RedirectStandardOutput = true;
          engine_process_.StartInfo.Arguments = "\"" + Project.directory_path + "\"";
          engine_process_.OutputDataReceived += EngineOutputWriter.engine_output_func;
          engine_process_.Start();
          engine_process_.BeginOutputReadLine();
          return new HandleRef(this, window);
        }

        /**
         *@brief cleanup the engine window
         *@param[in] handle (HandleRef) window handle of the control serving as the host
         */
        protected override void DestroyWindowCore(HandleRef handle)
        {
          native.Win32.DestroyWindow(window);
          if (engine_process_ != null && engine_process_.HasExited == false)
          {
            engine_process_.Close();
          }
        }

        private uint width_; //<! current width of the window in pixels

        private uint height_; //<! current height of the control in pixels

        private window_proc window_proc_; //<! window preocedure to handle incomming window messages

        private Process engine_process_;//<! the sulphur-engine hosted by the editor

        /**
         *@brief delegate to be passed to the window procedure upon creation
         *@param[in] hwnd (IntPtr) handle to the window to handle messages from
         *@param[in] msg (uint) message to process
         *@param[in] wparam (IntPtr) wparam send with the message
         *@param[in] lparam (IntPtr) lparam send with the message
         *@remkar this is an c# implementation of the WIn32 message procedre for a more detailed description see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx 
         */
        private delegate IntPtr window_proc(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam);

        /**
         *@brief window procedure implementation 
         *@param[in] hwnd (IntPtr) handle to the window to handle messages from
         *@param[in] msg (uint) message to process
         *@param[in] wparam (IntPtr) wparam send with the message
         *@param[in] lparam (IntPtr) lparam send with the message
         *@remkar this is an c# implementation of the WIn32 message procedre for a more detailed description see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx 
         */
        private IntPtr WndProc(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam)
        {
          return native.Win32.DefWindowProcW(hwnd, msg, wparam, lparam); // default msg handling
        }

        /**
         *@brief get the X Lparam from an lparam passed with a message in the window procedure
         *@param[in] lparam (IntPtr) lparam send with a window message 
         *@return (int) X Lparam extracted form the passed lparam  
         */
        private int GetXLParam(IntPtr lparam)
        {
          return LOWORD(lparam.ToInt32());
        }

        /**
         *@brief get the Y Lparam from an lparam passed with a message in the window procedure
         *@param[in] lparam (IntPtr) lparam send with a window message 
         *@return (int) Y Lparam extracted form the passed lparam  
         */
        private int GetYLParam(IntPtr lparam)
        {
          return HIWORD(lparam.ToInt32());
        }

        /**
         *@brief get the lower bits of an int
         *@param[in] i (int) value to get the lower bits from
         *@return (short) short containing the lower bits of i   
         */
        private short LOWORD(int i)
        {
          return (short)(i & 0xFFFF);
        }

        /**
         *@brief get the lower higher of an int
         *@param[in] i (int) value to get the lower bits from
         *@return (short) int containing the higher bits of i   
         */
        private short HIWORD(int i)
        {
          return (short)(i >> 16);
        }

        const uint WM_LBUTTONDOWN = 0x0201;//<! constant representing the the WM_LBUTTONDOWN message.
      }
    }
  }
}
