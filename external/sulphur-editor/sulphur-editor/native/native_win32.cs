using System;
using System.Runtime.InteropServices;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      class Win32
      {
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct Window
        {
          public uint style;
          public IntPtr window_procedure;
          public int class_style;
          public int window_style;
          public IntPtr hinstance;
          public IntPtr hicon;
          public IntPtr hcursor;
          public IntPtr background;
          [MarshalAs(UnmanagedType.LPWStr)]
          public string menu_name;
          [MarshalAs(UnmanagedType.LPWStr)]
          public string class_name;
        }

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

        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Unicode)]
        public static extern bool DestroyWindow(IntPtr hwnd);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern UInt16 RegisterClassW([In] ref Window window_class);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr DefWindowProcW(IntPtr hwnd, uint msg, IntPtr wparam, IntPtr lparam);
      }
    } // native
  } // editor
} // sulphur
