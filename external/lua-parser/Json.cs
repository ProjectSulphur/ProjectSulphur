using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using System.IO;

namespace lua_parser
{
  class Json
  {
    public class Value
    {
      public bool IsType<T>()
      {
        return GetType() == typeof(T);
      }
    }

    public class Object : Value
    {
      public void Add(string key, Value value)
      {
        values_.Add(key, value);
      }

      public Value GetValue(string key)
      {
        return values_.ContainsKey(key) == true ? values_[key] : null;
      }

      public Dictionary<string, Value>.KeyCollection GetKeys()
      {
        return values_.Keys;
      }

      private Dictionary<string, Value> values_ = new Dictionary<string, Value>();
    }

    public class Array : Value
    {
      public void Add(Value value)
      {
        values_.Add(value);
      }

      public Value GetValue(int index)
      {
        return values_[index];
      }

      public int GetCount()
      {
        return values_.Count;
      }

      private List<Value> values_ = new List<Value>();
    }

    public class String : Value
    {
      public String(string v)
      {
        value_ = v;
      }

      public string GetValue()
      {
        return value_;
      }

      private string value_;
    }

    public static void Convert(string src, out Value root)
    {
      using (JsonTextReader reader = new JsonTextReader(new StringReader(src)))
      {
        root = ParseValue(reader);
      }
    }

    public static Json.Value ParseValue(JsonTextReader reader)
    {
      reader.Read(); // Read first opening bracket

      Json.Value root = null;
      bool in_object = false;

      if (reader.TokenType == JsonToken.StartArray)
      {
        root = new Json.Array();
        in_object = false;
      }
      else
      {
        root = new Json.Object();
        in_object = true;
      }

      ParseValue(reader, root, in_object);

      return root;
    }

    public static void ParseValue(JsonTextReader reader, Json.Value root, bool in_object)
    {
      string last_key = "";
      Json.Value new_object = null;

      while (reader.Read() == true)
      {
        if (reader.TokenType == JsonToken.EndObject || reader.TokenType == JsonToken.EndArray)
        {
          break;
        }

        switch (reader.TokenType)
        {
          case JsonToken.StartObject:
            new_object = new Json.Object();
            ParseValue(reader, new_object, true);
            break;

          case JsonToken.StartArray:
            new_object = new Json.Array();
            ParseValue(reader, new_object, false);
            break;

          case JsonToken.PropertyName:
            last_key = (string)reader.Value;
            continue;

          case JsonToken.String:
          case JsonToken.Integer:
          case JsonToken.Null:
            object v = reader.Value;
            new_object = new Json.String(v == null ? "null" : v.ToString());
            break;
        }

        if (root == null)
        {
          root = new_object;
        }

        if (in_object == true)
        {
          ((Json.Object)root).Add(last_key, new_object);
          continue;
        }

        ((Json.Array)root).Add(new_object);
      }
    }
  }
}
