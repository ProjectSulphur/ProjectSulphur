using System;
using System.Windows.Data;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;

namespace sulphur
{
  namespace editor
  {
    namespace utils
    {
      public class CommandHandler : ICommand
      {
        /*public memberes*/
        public CommandHandler(Action<object> action, bool can_execute)
        {
          action_ = action;
          can_execute_ = can_execute;
        }

        public bool CanExecute(object parameter)
        {
          return can_execute_;
        }

        public event EventHandler CanExecuteChanged;

        public void Execute(object parameter)
        {
          action_(parameter);
        }

        /*private members*/
        private Action<object> action_;

        private bool can_execute_;
      }

      public class BindingProxy : Freezable
      {
        #region Overrides of Freezable

        protected override Freezable CreateInstanceCore()
        {
          return new BindingProxy();
        }

        #endregion

        public object data
        {
          get { return (object)GetValue(DataProperty); }
          set { SetValue(DataProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Data.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty DataProperty =
            DependencyProperty.Register("data", typeof(object), typeof(BindingProxy), new UIPropertyMetadata(null));
      }

      public class EnumToString : IValueConverter
      {
        public object Convert(object value, Type target_type, object parameter, System.Globalization.CultureInfo culture)
        {
          string result;
          result = Enum.GetName(value.GetType(), value);
          return result.Remove(0, 1);
        }

        public object ConvertBack(object value, Type target_type, object parameter, System.Globalization.CultureInfo culture)
        {
          throw new NotImplementedException();
        }
      }

      // classes to be used with binary reader and writer read vector and list methods
      public class UInt64_s : IBinarySerializable
      {
        public UInt64 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadU64();
        }
      }

      public class Uint32_s : IBinarySerializable
      {
        public UInt32 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadU32();
        }
      }

      public class Uint16_s : IBinarySerializable
      {
        public UInt16 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadU16();
        }
      }

      public class Int64_s : IBinarySerializable
      {
        public Int64 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadI64();
        }
      }

      public class Int32_s : IBinarySerializable
      {
        public Int32 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadI32();
        }
      }

      public class Int16_s : IBinarySerializable
      {
        public Int16 value { get; set; }

        public void Read(BinaryReader reader)
        {
          value = reader.ReadI16();
        }
      }
    }

    public class Utils
    {
      public static T BytesToStruct<T>(byte[] data) where T: struct
      {
        unsafe
        {
          fixed(byte* ptr = data)
          {
            return Marshal.PtrToStructure<T>((IntPtr)ptr);
          }
        }
      }

      public static byte[] StructToBytes<T>(T obj) where T : struct
      {
        byte[] result = new byte[Marshal.SizeOf(obj)];
        unsafe
        {
          fixed(byte* ptr = result)
          {
            Marshal.StructureToPtr(obj, (IntPtr)ptr, false);
          }
        }
        return result;
      }
    }
  }

}