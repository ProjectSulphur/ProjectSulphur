using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using System.IO;

namespace lua_parser
{
  class Parser
  {
    public enum SupportedTypes
    {
      kFloat,
      kDouble,
      kInt,
      kUnsignedInt,
      kString,
      kVector2,
      kVector3,
      kVector4,
      kMatrix3x3,
      kMatrix4x4,
      kVoid,
      kUserdata
    }

    public enum RefType
    {
      kPointer,
      kReference,
      kNormal
    }

    static Dictionary<string, SupportedTypes> kNameToType = new Dictionary<string, SupportedTypes>
    {
      { "float", SupportedTypes.kFloat },
      { "double", SupportedTypes.kDouble },
      { "int", SupportedTypes.kInt },
      { "char", SupportedTypes.kInt },
      { "int32_t", SupportedTypes.kInt },
      { "int64_t", SupportedTypes.kInt },
      { "unsigned int", SupportedTypes.kUnsignedInt },
      { "unsigned char", SupportedTypes.kUnsignedInt },
      { "uint32_t", SupportedTypes.kUnsignedInt },
      { "uint64_t", SupportedTypes.kUnsignedInt },
      { "String", SupportedTypes.kString },
      { "vec2", SupportedTypes.kVector2 },
      { "vec3", SupportedTypes.kVector3 },
      { "mat3", SupportedTypes.kMatrix3x3 },
      { "mat4", SupportedTypes.kMatrix4x4 },
      { "void", SupportedTypes.kVoid }
    };

    public class Enum
    {
      public Enum(string n)
      {
        name = n;
        values = new Dictionary<string, int>();
      }

      public string name;
      public Dictionary<string, int> values;
    }

    public class Function
    {
      public class Arg
      {
        public Arg(string n)
        {
          name = n;
          type_name = "void";
          type = SupportedTypes.kUserdata;
        }

        public string name;
        public string type_name;
        public SupportedTypes type;
        public RefType ref_type;
      }

      public Function(string n)
      {
        name = n;
        return_type = SupportedTypes.kUserdata;
        args = new List<Arg>();
      }

      public string name;
      public SupportedTypes return_type;
      public List<Arg> args;
    }

    public class Class
    {
      public Class(string n)
      {
        name = n;
        funcs = new List<Function>();
        enums = new List<Enum>();
      }

      public string name;
      public string lua_name;

      public List<Function> funcs;
      public List<Enum> enums;
    }

    protected static void Error()
    {
      Console.WriteLine("Invalid JSON file");
    }

    protected static SupportedTypes StringToType(string name)
    {
      foreach (KeyValuePair<string, SupportedTypes> kv in kNameToType)
      {
        if (kv.Key == name)
        {
          return kv.Value;
        }
      }

      return SupportedTypes.kUserdata;
    }

    protected static void AddFunction(Class cl, Json.Object mem, string name)
    {
      Function f = new Function(name);

      Json.Array args = (Json.Array)mem.GetValue("arguments");

      f.return_type = StringToType(((Json.String)((Json.Object)mem.GetValue("returnType"))
                                                     .GetValue("name")).GetValue());

      Json.Object arg;
      Function.Arg farg;

      Json.Object type;
      string named_type;
      string ref_type;

      for (int i = 0; i < args.GetCount(); ++i)
      {
        arg = (Json.Object)args.GetValue(i);

        farg = new Function.Arg(((Json.String)arg.GetValue("name")).GetValue());
        farg.ref_type = RefType.kNormal;

        type = ((Json.Object)arg.GetValue("type"));

        if (type.GetValue("name") != null)
        {
          named_type = ((Json.String)type.GetValue("name")).GetValue();
        }
        else
        {
          ref_type = ((Json.String)type.GetValue("type")).GetValue();

          switch (ref_type)
          {
            case "pointer":
              farg.ref_type = RefType.kPointer;
              break;

            case "reference":
              farg.ref_type = RefType.kReference;
              break;

            default:
              farg.ref_type = RefType.kNormal;
              break;
          }

          named_type = ((Json.String)((Json.Object)type.GetValue("baseType")).GetValue("name")).GetValue();
        }

        farg.type = StringToType(named_type);
        farg.type_name = named_type;

        f.args.Add(farg);
      }

      cl.funcs.Add(f);
    }

    protected static void AddEnum(Class cl, Json.Object mem, string name)
    {
      Enum e = new Enum(name);

      Json.Array members = (Json.Array)mem.GetValue("members");

      Json.Object val;
      Json.Value v;

      string key;
      int idx;

      for (int i = 0; i < members.GetCount(); ++i)
      {
        idx = i;
        val = (Json.Object)members.GetValue(i);
        key = ((Json.String)val.GetValue("key")).GetValue();

        if ((v = val.GetValue("value")) != null)
        {
          idx = Int32.Parse(((Json.String)v).GetValue());
        }

        e.values.Add(key, idx);
      }

      cl.enums.Add(e);
    }

    protected static bool AddLuaName(Class cl, Json.Object mem)
    {
      if (((Json.String)mem.GetValue("name")).GetValue() == "LUA_NAME")
      {
        Json.Object meta = (Json.Object)mem.GetValue("meta");

        if (meta == null)
        {
          return false;
        }

        cl.lua_name = meta.GetKeys().First();
        return true;
      }

      return false;
    }

    public static bool Parse(string json, out List<Class> classes)
    {
      classes = new List<Class>();

      Console.Write(json);
      Json.Value root;
      Json.Convert(json, out root);

      Json.Array values = (Json.Array)root;

      Json.Value v;
      Json.String type, name;
      Json.Object cl;
      Json.Array members;
      Json.Object mem;

      Class new_class;

      string class_name;
      string mem_type, mem_name;

      bool found_lua_name = false;

      for (int i = 0; i < values.GetCount(); ++i)
      {
        if ((v = values.GetValue(i)).IsType<Json.Object>() == true)
        {
          cl = (Json.Object)v;
          type = (Json.String)cl.GetValue("type");

          if (type.GetValue() != "class")
          {
            continue;
          }

          name = (Json.String)cl.GetValue("name");
          class_name = name.GetValue();

          Console.WriteLine("Parsing definition for: " + class_name);
          new_class = new Class(class_name);
          found_lua_name = false;

          members = (Json.Array)cl.GetValue("members");

          for (int j = 0; j < members.GetCount(); ++j)
          {
            mem = (Json.Object)members.GetValue(j);

            mem_type = ((Json.String)mem.GetValue("type")).GetValue();
            mem_name = ((Json.String)mem.GetValue("name")).GetValue();

            switch (mem_type)
            {
              case "function":
                AddFunction(new_class, mem, mem_name);
                break;

              case "enum":
                AddEnum(new_class, mem, mem_name);
                break;

              case "macro":
                if (found_lua_name == false && AddLuaName(new_class, mem) == true)
                {
                  found_lua_name = true;
                }
                break;
            }
          }

          Console.WriteLine("-- Lua name: " + new_class.lua_name);
          Console.WriteLine("-- Number of functions: " + new_class.funcs.Count);
          Console.WriteLine("-- Number of enums: " + new_class.enums.Count);

          classes.Add(new_class);
        }
      }

      return true;
    }
  }
}
