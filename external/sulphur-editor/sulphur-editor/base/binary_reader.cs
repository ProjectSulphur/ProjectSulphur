using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Numerics;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.BinaryReader
     *@brief class for reading binary files written using the sulphur::foundation::BinaryWriter 
     *@author Stan Pepels 
     */
    public class BinaryReader
    {
      
      /**
       *@brief bool indicating if the file is valid
       */
      public bool is_ok { get; private set; }

      /**
       *@brief constructor
       *@param[in] path (string) path to a binary file
       */
      public BinaryReader(string path)
      {
        if (File.Exists(path) == true)
        {
          FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read);
          data_ = new byte[stream.Length];
          stream.Read(data_, 0, data_.Length);
          stream.Close();
          read_pos_ = 0;
          is_ok = true;
        }
        else
        {
          is_ok = false;
        }
      }

      /**
       *@brief read a single byte
       *@return (byte) the byte read  
       */
      public byte ReadByte()
      {
        if (read_pos_ > data_.Length)
        {
          return 0;
        }
        return data_[read_pos_++];
      }

      /**
       *@brief read multiple bytes into an array
       *@param[in] buf (ref byte[]) byte buffer to fill
       *@remark length read is the lenght of the array passed 
       */
      public void ReadBytes(ref byte[] buf)
      {
        if (buf.Length == 0)
        {
          return;
        }

        if (buf.Length > data_.Length - read_pos_)
        {
          return;
        }

        Buffer.BlockCopy(data_, (int)read_pos_, buf, 0, buf.Length);
        read_pos_ += (uint)buf.Length;
      }

      /**
      *@brief read a 32 bit boolean value
      *@return (Boolean) value of the 32 bit boolean
      */
      public Boolean ReadBoolean()
      {
        Boolean result = BitConverter.ToBoolean(data_, (int)read_pos_);
        read_pos_ += sizeof(Boolean);
        return result;
      }

      /**
      *@brief read a 16 bit unsigned integer
      *@return (UInt16) value of the 16 bit unsigned integer
      */
      public UInt16 ReadU16()
      {
        UInt16 result = BitConverter.ToUInt16(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt16);
        return result;
      }

      /**
      *@brief read a 32 bit unsigned integer
      *@return (UInt32) value of the 32 bit unsigned integer
      */
      public UInt32 ReadU32()
      {
        UInt32 result = BitConverter.ToUInt32(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt32);
        return result;
      }

      /**
      *@brief read a 64 bit unsigned integer
      *@return (UInt64) value of the 64 bit unsigned integer
      */
      public UInt64 ReadU64()
      {
        UInt64 result = BitConverter.ToUInt64(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt64);
        return result;
      }

      /**
      *@brief read a 16 bit signed integer
      *@return (Int16) value of the 16 bit signed integer
      */
      public Int16 ReadI16()
      {
        Int16 result = BitConverter.ToInt16(data_, (int)read_pos_);
        read_pos_ += sizeof(Int16);
        return result;
      }

      /**
      *@brief read a 32 bit signed integer
      *@return (Int32) value of the 32 bit signed integer
      */
      public Int32 ReadI32()
      {
        Int32 result = BitConverter.ToInt32(data_, (int)read_pos_);
        read_pos_ += sizeof(Int32);
        return result;
      }

      /**
      *@brief read a 64 bit signed integer
      *@return (Int64) value of the 64 bit signed integer
      */
      public Int64 ReadI64()
      {
        Int64 result = BitConverter.ToInt64(data_, (int)read_pos_);
        read_pos_ += sizeof(Int64);
        return result;
      }

      /**
       *@brief read a string
       *@return (string) string read from the buffer
       */
      public string ReadString()
      {
        UInt32 size = ReadU32();
        byte[] buf = new byte[size];
        ReadBytes(ref buf);
        return System.Text.Encoding.Default.GetString(buf);
      }

      /**
       *@brief read a class that implements the sulphut.editor.IBinarySerializable interface
       *@param[in] obj (sulphut.editor.IBinarySerializable) object to be filled with data from the buffer
       */
      public void ReadSerializable(IBinarySerializable obj)
      {
        obj.Read(this);
      }

      /**
       *@brief set position to read from the buffer
       *@param[in] pos(uint) a position in the buffer  
       */
      public void Seek(uint pos)
      {
        if(read_pos_ >= data_.Length)
        {
          return;
        }
        read_pos_ = pos;
      }

      /**
      *@brief Read a list of type T where T implements the sulphur.editor.IBinarySerializable interface
      *@tparam T type to read
      *@param[out] out_list (List<T>) List of objects of type T read from the buffer
      *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Vector<T>& val) function
      */
      public void ReadList<T>(out List<T> out_list) where T : IBinarySerializable, new()
      {
        int size = (int)ReadU64();
        out_list = new List<T>(size);
        for (int i = 0; i < size; ++i)
        {
          out_list.Add(new T());
          ReadSerializable(out_list[i]);
        }
      }

      /**
       *@brief Read a list of type T where T is a refrence type and does not implement the sulphur.editor.IBinarySerializable interface
       *@tparam T type used to determain the byte array size
       *@param[out] out_list (List<bytep[]>) List of byte arrays with a sizeof type T read from the buffer
       *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Vector<T>& val) function
       */
      public void ReadList<T>(out List<byte[]> out_list)
      {
        UInt64 size = ReadU64();
        out_list = new List<byte[]>((int)size);
        byte[] buf = new byte[Marshal.SizeOf<T>()];
        for (int i = 0; i < out_list.Count; ++i)
        {
          ReadBytes(ref buf);
          out_list.Add(buf);
        }
      }

      /**
       *@brief read a dictionary with key value pairs of type T and type U. both types must implement the sulphur.editor.IBinarySerializable interface
       *@tparam T type used as the keys
       *@tparam U type used as the values 
       *@param[out] out_dict (Dictionary<T, U>) dictionry with key value pairs of type T,U read from the buffer
       *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Map<Key, Value>& val) function
       */
      public void ReadDictionary<T, U>(out Dictionary<T, U> out_dict) 
        where T : IBinarySerializable, new()
        where U : IBinarySerializable, new()
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<T, U>();
        for (int i = 0; i < (int)size; ++i)
        {
          T key = new T();
          U val = new U();
          ReadSerializable(key);
          ReadSerializable(val);
          out_dict.Add(key, val);
        }
      }

      /**
       *@brief read a dictionary with key value pairs of type T and type U. both types must implement the sulphur.editor.IBinarySerializable interface
       *@tparam T type used to determain the byte array size of the keys
       *@tparam U type used to determain the byte array size of the values
       *@param[out] out_dict (Dictionary<byte[], byte[]>) the dictionary read from the buffer 
       *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Map<Key, Value>& val) function
       */
      public void ReadDictionary<T, U>(out Dictionary<byte[], byte[]> out_dict)
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<byte[], byte[]>();
        byte[] key_buf = new byte[Marshal.SizeOf<T>()];
        byte[] value_buf = new byte[Marshal.SizeOf<U>()];
        for(int i = 0; i < (int)size; ++i)
        {
          ReadBytes(ref key_buf);
          ReadBytes(ref value_buf);
          out_dict.Add(key_buf, value_buf);
        }
      }

      /**
       *@brief read a dictionary with key value pairs of byte[] and type U.
       *@tparam T type used to determain the byte array size of the keys
       *@tparam U type that implements the sulphur.editor.IBinarySerializable interface
       *@param[out] out_dict (Dictionary<byte[], byte[]>) the dictionary read from the buffer 
       *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Map<Key, Value>& val) function
       */
      public void ReadDictionary<T, U>(out Dictionary<byte[], U> out_dict) where U : IBinarySerializable, new()                                           
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<byte[], U>();
        U obj = new U();

        byte[] buf = new byte[Marshal.SizeOf<T>()];
        for (int i = 0; i < (int)size; ++i)
        {
          ReadBytes(ref buf);
          ReadSerializable(obj);
          out_dict.Add(buf, obj);
        }
      }

      /**
      *@brief read a dictionary with key value pairs of type T and and byte[].
      *@tparam T type that implements the sulphur.editor.IBinarySerializable interface
      *@tparam U type used to determain the byte array size of the keys
      *@param[out] out_dict (Dictionary<T, byte[]>) the dictionary read from the buffer 
      *@remark this function is used to read data written with the sulphur::foundation::BinaryWriter::Write(const Map<Key, Value>& val) function
      */
      public void ReadDictionary<T, U>(out Dictionary<T, byte[]> out_dict) where T : IBinarySerializable, new()
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<T, byte[]>();
        T obj = new T();

        byte[] buf = new byte[Marshal.SizeOf<U>()];
        for (int i = 0; i < (int)size; ++i)
        {
          ReadBytes(ref buf);
          ReadSerializable(obj);
          out_dict.Add(obj, buf);
        }
      }

      private uint read_pos_; //<! current position in the buffer

      private byte[] data_; //<! data buffer to read from. with the data from the file given in the constructor.
    }
  }
}
