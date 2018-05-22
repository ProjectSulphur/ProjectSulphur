using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace sulphur
{
  namespace editor
  {
    public class BinaryReader
    {
      private byte[] data_;
      private uint read_pos_;
      public bool is_ok { get; private set; }

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

      public byte ReadByte()
      {
        if (read_pos_ > data_.Length)
        {
          return 0;
        }
        return data_[read_pos_++];
      }

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

      public UInt16 ReadU16()
      {
        UInt16 result = BitConverter.ToUInt16(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt16);
        return result;
      }

      public UInt32 ReadU32()
      {
        UInt32 result = BitConverter.ToUInt32(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt32);
        return result;
      }

      public UInt64 ReadU64()
      {
        UInt64 result = BitConverter.ToUInt64(data_, (int)read_pos_);
        read_pos_ += sizeof(UInt64);
        return result;
      }

      public Int16 ReadI16()
      {
        Int16 result = BitConverter.ToInt16(data_, (int)read_pos_);
        read_pos_ += sizeof(Int16);
        return result;
      }

      public Int32 ReadI32()
      {
        Int32 result = BitConverter.ToInt32(data_, (int)read_pos_);
        read_pos_ += sizeof(Int32);
        return result;
      }

      public Int64 ReadI64()
      {
        Int64 result = BitConverter.ToInt64(data_, (int)read_pos_);
        read_pos_ += sizeof(Int64);
        return result;
      }

      public string ReadString()
      {
        UInt32 size = ReadU32();
        byte[] buf = new byte[size];
        ReadBytes(ref buf);
        return System.Text.Encoding.Default.GetString(buf);
      }

      public void ReadSerializable(IBinarySerializable obj)
      {
        obj.Read(this);
      }

      public void Seek(uint pos)
      {
        if(read_pos_ >= data_.Length)
        {
          return;
        }
        read_pos_ = pos;
      }

      public List<T> ReadList<T>() where T : struct
      {
        UInt64 size = ReadU64();
        List<T> result = new List<T>((int)size);
        byte[] buf = new byte[Marshal.SizeOf<T>()];
        for(int i = 0; i < result.Count; ++i)
        {
          ReadBytes(ref buf);
          result[i] = Utils.BytesToStruct<T>(buf);
        }
        return result;
      }

      public void ReadList<T>(out List<IBinarySerializable> out_list) where T : class, IBinarySerializable, new()
      {
        UInt64 size = ReadU64();
        out_list = new List<IBinarySerializable>((int)size);
        for (int i = 0; i < out_list.Count; ++i)
        {
          T val = new T();
          ReadSerializable(val);
          out_list[i] = val;
        }
      }

      public void ReadList<U>(out List<U> out_list) where U : IBinarySerializable
      {
        UInt64 size = ReadU64();
        out_list = new List<U>((int)size);
        for(int i = 0; i < out_list.Count; ++i)
        {
          ReadSerializable(out_list[i]);
        }
      }

      public void ReadDictionary<T, U>(out Dictionary<IBinarySerializable, IBinarySerializable> out_dict) where T : class, IBinarySerializable, new()
        where U : class, IBinarySerializable, new()
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<IBinarySerializable, IBinarySerializable>();
        for (int i = 0; i < (int)size; ++i)
        {
          T key = new T();
          U val = new U();
          ReadSerializable(key);
          ReadSerializable(val);
          out_dict.Add(key, val);
        }
      }

      public void ReadDictionary<T, U>(out Dictionary<byte[], byte[]> out_dict) where T : struct
                                                                               where U : struct
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

      public void ReadDictionary<T, U>(out Dictionary<byte[], IBinarySerializable> out_dict) where T : struct
                                                                                             where U : class, IBinarySerializable, new()                                           
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<byte[], IBinarySerializable>();
        U obj = new U();

        byte[] buf = new byte[Marshal.SizeOf<T>()];
        for (int i = 0; i < (int)size; ++i)
        {
          ReadBytes(ref buf);
          ReadSerializable(obj);
          out_dict.Add(buf, obj);
        }
      }

      public void ReadDictionary<T, U>(out Dictionary<IBinarySerializable, byte[]> out_dict) where T : struct
                                                                                        where U : class, IBinarySerializable, new()
      {
        UInt64 size = ReadU64();
        out_dict = new Dictionary<IBinarySerializable, byte[]>();
        U obj = new U();

        byte[] buf = new byte[Marshal.SizeOf<T>()];
        for (int i = 0; i < (int)size; ++i)
        {
          ReadBytes(ref buf);
          ReadSerializable(obj);
          out_dict.Add(obj, buf);
        }
      }
    }
  }
}
