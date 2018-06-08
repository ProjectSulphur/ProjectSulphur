using System;
using System.Collections.Generic;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;
using System.Globalization;

namespace sulphur
{
  namespace editor
  {
    namespace utils
    {
      /**
       *@class sulphur.editor.utils.CommandHandler : ICommand 
       *@brief general class for handling commands
       *@author Stan Pepels 
       */
      public class CommandHandler : ICommand
      {
        /**
         *@brief constructor 
         *@param[in] action (Action<object>) function to execute with the command
         *@param[in] can_execute (bool) bool indicating if the command can be executed 
         */
        public CommandHandler(Action<object> action, bool can_execute)
        {
          action_ = action;
          can_execute_ = can_execute;
        }

        /**
         *@brief check if the command can be executed 
         *@param[in] parameter (object) parameters that can be used to prematurely check if the command can be executed.
         *@return (bool) true if the command can be executed, false otherwise.
         */
        public bool CanExecute(object parameter)
        {
          return can_execute_;
        }

        /**
         *@brief event fired when the command state changes 
         */
        public event EventHandler CanExecuteChanged;

        /**
         *@brief executes the command 
         *@param[in] parameter (object) parameters to execute the command with 
         */
        public void Execute(object parameter)
        {
          action_(parameter);
        }

        private Action<object> action_; //<! delegate holding the operation to execute the command with

        private bool can_execute_; //<! bool indicating if the command can be executed
      }

      /**
       *@class sulphur.editor.utils.BindingProxy : Freezable 
       *@brief proxy class to for relaying bindings in contextmenu
       *@author Stan Pepels 
       */
      public class BindingProxy : Freezable
      {
        #region Freezable

        /**
         *@brief create new instance of this class 
         */
        protected override Freezable CreateInstanceCore()
        {
          return new BindingProxy();
        }

        #endregion

        /**
         *@brief data to be relayed
         */
        public object data
        {
          get { return (object)GetValue(DataProperty); }
          set { SetValue(DataProperty, value); }
        }

        /**
         *@brief property used for the actual data binding
         */
        public static readonly DependencyProperty DataProperty =
            DependencyProperty.Register("data",
                                        typeof(object),
                                        typeof(BindingProxy),
                                        new UIPropertyMetadata(null));
      }

      /**
       *@class sulphur.editor.utils.EnumToString : IValueConverter 
       *@brief class for converting enum values to string values in databinding
       *@author Stan Pepels  
       */
      public class EnumToString : IValueConverter
      {
        /**
         *@brief convert enum value to string  
         *@param[in] value (object) value to convert 
         *@param[in] target_type (Type) type to convert to
         *@param[in] parameter (object) additional parameter used for the cenversion
         *@param[in] culture (System.Globalization.CultureInfo) The culture to use in the converter
         *@return (object) result of the conversion
         */
        public object Convert(object value,
                              Type target_type,
                              object parameter,
                              CultureInfo culture)
        {
          string result;
          result = Enum.GetName(value.GetType(), value);
          return result.Remove(0, 1);
        }

        /**
         *@brief convert string value to enum
         *@param[in] value (object) value to convert 
         *@param[in] target_type (Type) type to convert to
         *@param[in] parameter (object) additional parameter used for the cenversion
         *@param[in] culture (System.Globalization.CultureInfo) The culture to use in the converter
         *@return (object) enum value returned
         *@remark not implemented yet 
         */
        public object ConvertBack(object value, Type target_type, object parameter, System.Globalization.CultureInfo culture)
        {
          throw new NotImplementedException();
        }
      }

      /**
       * @class sulphur.editor.utils.FolderMappingConverter : IMultiValueConverter
       * @brief Value converter that takes a sulphur.editor.DirectoryItem and Dictionary<string, sulphur.editor.Assetlist> as input values 
       * and returns the sulphur.editor.AssetList at the directory path.
       * @see sulphur.editor.DirectoryItem
       * @see sulphur.editor.AssetList
       * @see https://msdn.microsoft.com/en-us/library/system.windows.data.imultivalueconverter(v=vs.110).aspx
       * @author Stan Pepels
       */
      public class FolderMappingConverter : IMultiValueConverter
      {
        /**
         * @see https://msdn.microsoft.com/en-us/library/system.windows.data.imultivalueconverter.convert(v=vs.110).aspx
         */
        public object Convert(object[] value, Type target_type, object parameter, CultureInfo culture)
        {
          DirectoryItem directory = value[0] as DirectoryItem;
          Dictionary<string, AssetList> mapping = value[1] as Dictionary<string, AssetList>;
          return directory != null && mapping.ContainsKey(directory.path.ToLower()) ? mapping[directory.path.ToLower()] : null;
        }

        /**
         * @see https://msdn.microsoft.com/en-us/library/system.windows.data.imultivalueconverter.convertback(v=vs.110).aspx
         */
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
          throw new NotImplementedException();
        }
      }

      /**
       *@class sulphur.editor.utils.SerializableDataType<T> : sulphur.editor.IBinarySerializable
       *@brief base class for making C# native data types compatible with the ReadDictionary<T> and ReadList<T> functions from the sulphur.editor.BinaryReader 
       *@author Stan Pepels 
       */
      public class SerializableDataType<T> : IBinarySerializable
      {
        /**
         *@brief default constructor
         */
        public SerializableDataType() { }

        /**
         *@brief constructor
         *@param[in] val (T) param to initialze to  
         */
        protected SerializableDataType(T val)
        {
          value = val;
        }


        protected T value { get; set; } //<! value to serialize

        /**
         *@brief see sulphur.editor.IBinarySerializable 
         */
        public virtual void Read(BinaryReader reader)
        {
          throw new NotImplementedException();
        }

        /**
         *@brief GetHashCode override
         *@return (int) hash code of the stored value  
         */
        public override int GetHashCode()
        {
          return value.GetHashCode();
        }

        /**
         *@brief Equals override
         *@param[in] obj (object) object to compare to  
         */
        public override bool Equals(object obj)
        {
          if (obj == null || GetType() != obj.GetType())
          {
            return false;
          }

          return value.Equals((obj as SerializableDataType<T>).value);
        }

        /**
         *@brief == operator overload for comparing a type T to the stored value 
         *@param[in] left (T) value on the left of the == operator
         *@param[in] right (SerializableDataType<T>) value on the right of the == operator
         *@return (bool) result of comparing left to right 
         */
        public static bool operator ==(T left, SerializableDataType<T> right)
        {
          return right.value.Equals(left);
        }

        /**
         *@brief == operator overload for comparing a type T to the stored value 
         *@param[in] left (SerializableDataType<T>) value on the left of the == operator
         *@param[in] right (T) value on the right of the == operator
         *@return (bool) result of comparing left to right 
         */
        public static bool operator ==(SerializableDataType<T> left, T right)
        {
          return left.value.Equals(right);
        }

        /**
         *@brief != operator overload for comparing a type T to the stored value 
         *@param[in] left (T) value on the left of the != operator
         *@param[in] right (SerializableDataType<T>) value on the right of the != operator
         *@return (bool) result of comparing left to right 
         */
        public static bool operator !=(T left, SerializableDataType<T> right)
        {
          return !right.value.Equals(left);
        }

        /**
         *@brief != operator overload for comparing a type T to the stored value 
         *@param[in] left (SerializableDataType<T>) value on the left of the != operator
         *@param[in] right (T) value on the right of the != operator
         *@return (bool) result of comparing left to right 
         */
        public static bool operator !=(SerializableDataType<T> left, T right)
        {
          return !left.value.Equals(right);
        }
      }

      /**
       *@class sulphur.editor.utils.UInt64_s : sulphur.editor.utils.SerializableDataType<UInt64> 
       *@brief class for serializing a UInt64
       *@remark this class behaves like a regular UInt64 but can be used with the read functions from the sulphur.editor.Binaryreader that use generics
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
       *@author Stan Pepels  
       */
      public class UInt64_s : SerializableDataType<UInt64>
      {
        /**
         *@brief default constructor 
         */
        public UInt64_s() { }

        /**
         *@brief constructor
         *@param[in] val (UInt64) value to initialize with 
         */
        public UInt64_s(UInt64 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (UInt64) value to convert from 
         */
        public static implicit operator UInt64_s(UInt64 other) { return new UInt64_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (UInt64_s) value to convert from 
        */
        public static implicit operator UInt64(UInt64_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        override public void Read(BinaryReader reader)
        {
          value = reader.ReadU64();
        }
      }

      /**
      *@class sulphur.editor.utils.UInt32_s : sulphur.editor.utils.SerializableDataType<UInt32> 
      *@brief class for serializing a UInt32
      *@see sulphur.editor.BinaryReader.ReadList<T> 
      *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class UInt32_s : SerializableDataType<UInt32>
      {
        /**
         *@brief default constructor 
         */
        public UInt32_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public UInt32_s(UInt32 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (UInt32) value to convert from 
         */
        public static implicit operator UInt32_s(UInt32 other) { return new UInt32_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (UInt32_s) value to convert from 
        */
        public static implicit operator UInt32(UInt32_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadU32();
        }
      }

      /**
      *@class sulphur.editor.utils.UVec3 : sulphur.editor.IBinarySerializable
      *@brief class for serializing a Vector3 of type sulphur.editor.utils.Uint32_s
      *@author Stan Pepels  
      */
      public class Vec3U : IBinarySerializable
      {
        public UInt32_s x { get; set; }
        public UInt32_s y { get; set; }
        public UInt32_s z { get; set; }

        /**
        *@brief implementation of the sulphur.editor.IBinarySerializable interface. this will read an entry from a binary file and fill this struct with its contents.
        *@param[in] reader (sulphur.editor.BinaryReader) reader containing the cache data.
        */
        public void Read(BinaryReader reader)
        {
          x = reader.ReadU32();
          y = reader.ReadU32();
          z = reader.ReadU32();
        }
      }

      /**
      *@class sulphur.editor.utils.UInt16_s : sulphur.editor.utils.SerializableDataType<UInt16> 
      *@brief class for serializing a Uint16
      *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class UInt16_s : SerializableDataType<UInt16>
      {
        /**
         *@brief default constructor 
         */
        public UInt16_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public UInt16_s(UInt16 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (UInt16) value to convert from 
         */
        public static implicit operator UInt16_s(UInt16 other) { return new UInt16_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (UInt16_s) value to convert from 
        */
        public static implicit operator UInt16(UInt16_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadU16();
        }
      }

      /**
      *@class sulphur.editor.utils.Int64_s : sulphur.editor.utils.SerializableDataType<Int64> 
      *@brief class for serializing a Int64
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class Int64_s : SerializableDataType<Int64>
      {
        /**
         *@brief default constructor 
         */
        public Int64_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public Int64_s(Int64 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (Int64) value to convert from 
         */
        public static implicit operator Int64_s(Int64 other) { return new Int64_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (Int64_s) value to convert from 
        */
        public static implicit operator Int64(Int64_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadI64();
        }
      }

      /**
      *@class sulphur.editor.utils.Int32_s : sulphur.editor.utils.SerializableDataType<Int32> 
      *@brief class for serializing a Int32
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class Int32_s : SerializableDataType<Int32>
      {
        /**
         *@brief default constructor 
         */
        public Int32_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public Int32_s(Int32 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (Int32) value to convert from 
         */
        public static implicit operator Int32_s(Int32 other) { return new Int32_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (Int32_s) value to convert from 
        */
        public static implicit operator Int32(Int32_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadI32();
        }
      }

      /**
      *@class sulphur.editor.utils.Int16_s : sulphur.editor.utils.SerializableDataType<Int16> 
      *@brief class for serializing a Int16
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class Int16_s : SerializableDataType<Int16>
      {
        /**
         *@brief default constructor 
         */
        public Int16_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public Int16_s(Int16 val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (Int16) value to convert from 
         */
        public static implicit operator Int16_s(Int16 other) { return new Int16_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (Int16_s) value to convert from 
        */
        public static implicit operator Int16(Int16_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadI16();
        }
      }

      /**
      *@class sulphur.editor.utils.Byte_s : sulphur.editor.utils.SerializableDataType<byte> 
      *@brief class for serializing a byte
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class Byte_s : SerializableDataType<byte>
      {
        /**
         *@brief default constructor 
         */
        public Byte_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public Byte_s(byte val) : base(val) { }

        /**
         *@brief implicit conversion from Byte_s 
         *@param[in] other (byte) value to convert from 
         */
        public static implicit operator Byte_s(byte other) { return new Byte_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.Byte_s 
        *@param[in] other (Byte_s) value to convert from 
        */
        public static implicit operator byte(Byte_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadByte();
        }
      }

      /**
      *@class sulphur.editor.utils.String_s : sulphur.editor.utils.SerializableDataType<string> 
      *@brief class for serializing a string
       *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
      *@author Stan Pepels  
      */
      public class String_s : SerializableDataType<string>
      {
        /**
         *@brief default constructor 
         */
        public String_s() { }

        /**
         *@see sulphur.editor.utils.SerializableDataType<T> 
         */
        public String_s(string val) : base(val) { }

        /**
         *@brief implicit conversion from UInt64 
         *@param[in] other (string) value to convert from 
         */
        public static implicit operator String_s(string other) { return new String_s(other); }

        /**
        *@brief implicit conversion from sulphur.editor.utils.UInt64_s 
        *@param[in] other (String_s) value to convert from 
        */
        public static implicit operator string(String_s other) { return other.value; }

        /**
         *@brief sulphur.editor.IBinarySerializable implementation 
         *@param[in] reader (sulphur.editor.BinaryReader) reader to read the value from
         */
        public override void Read(BinaryReader reader)
        {
          value = reader.ReadString();
        }
      }
    }

    /**
     *@class sulphur.editor.Utils 
     *@brief class containing static utility functions 
      *@see sulphur.editor.BinaryReader.ReadList<T> 
       *@see sulphur.editor.BinaryReader.ReadDictionary<T, U> 
     *@author Stan Pepels 
     */
    public class Utils
    {
      /**
       *@brief converts a byte[] containign data from an unmanaged c++ struct to a managed c# struct
       *@param[in] data (byte[]) byte array containg the unmanged c++ data 
       *@return (T) object converted from the input byte array 
       */
      public static T BytesToStruct<T>(byte[] data) where T : struct
      {
        unsafe
        {
          fixed (byte* ptr = data)
          {
            return Marshal.PtrToStructure<T>((IntPtr)ptr);
          }
        }
      }

      /**
       *@brief converts an object of type T to a byte[]
       *@param[in] obj (T) object to convert to a byte array 
       *@return (byte[]) byte array created from the input struct 
       */
      public static byte[] StructToBytes<T>(T obj) where T : struct
      {
        byte[] result = new byte[Marshal.SizeOf(obj)];
        unsafe
        {
          fixed (byte* ptr = result)
          {
            Marshal.StructureToPtr(obj, (IntPtr)ptr, false);
          }
        }
        return result;
      }

      /**
       *@brief Get an item in the tree view by doing a bottom-up search
       *@param[in] source (System.Windows.DependencyObject) The item to start the search from
       *@return (System.Windows.Controls.TreeViewItem) The item found (if any)
       *@remark When no item was found returns null
       */
      public static TreeViewItem GetTreeViewItemInParents(DependencyObject source)
      {
        while (source != null && (source is TreeViewItem) == false)
        {
          source = VisualTreeHelper.GetParent(source);
        }
        return source as TreeViewItem;
      }
    }
  }

}