using System;
using System.Runtime.InteropServices;
using System.Windows;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      /**
       *@class sulphur.editor.native.Win32
       *@brief class declaring functions for WIn32 interop
       *@author Stan Pepels   
       */
      class Win32
      {
        /**
         *@struct sulphur.editor.native.Win32.Window
         *@brief struct mimicing the WNDCLASSEX struct from win32
         *@remark see also: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx   
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct Window
        {
          public uint style; //!< style to be used for the window
          public IntPtr window_procedure; //!< window procedure for used for the window
          public int class_style; //!< window class style
          public int window_style; //!< window style
          public IntPtr hinstance; //!< pointer to the application
          public IntPtr hicon; //!< icon to be used for the window
          public IntPtr hcursor; //!< type of curser to use in the window
          public IntPtr background; //!< handle to the class background brush
          [MarshalAs(UnmanagedType.LPWStr)]
          public string menu_name; //!< the resource name of the class menu
          [MarshalAs(UnmanagedType.LPWStr)]
          public string class_name; //!< name for the class
        }

        /**
         *@see https://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx 
         */
        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
        public static extern IntPtr CreateWindowEx(int ex_style,
                                                   string class_name,
                                                   string window_name,
                                                   int style,
                                                   int x, int y,
                                                   int width, int height,
                                                   IntPtr parent_handle,
                                                   IntPtr menu,
                                                   IntPtr hinstance,
                                                   [MarshalAs(UnmanagedType.AsAny)] object pv_param);

        /**
         *@see https://msdn.microsoft.com/en-us/library/aa452942.aspx 
         */
        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Unicode)]
        public static extern bool DestroyWindow(IntPtr hwnd);

        /**
         *@see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633586(v=vs.85).aspx
         */
        [DllImport("user32.dll", SetLastError = true)]
        public static extern UInt16 RegisterClassW([In] ref Window window_class);

        /**
         *@see https://msdn.microsoft.com/en-us/library/windows/desktop/ms633572(v=vs.85).aspx
         */
        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr DefWindowProcW(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetCursorPos(ref Win32Point pt);

        [StructLayout(LayoutKind.Sequential)]
        internal struct Win32Point
        {
          public Int32 X;
          public Int32 Y;
        };

        public static Point GetMousePosition()
        {
          Win32Point w32Mouse = new Win32Point();
          GetCursorPos(ref w32Mouse);
          return new Point(w32Mouse.X, w32Mouse.Y);
        }
      }
    } // native
  } // editor
} // sulphur
