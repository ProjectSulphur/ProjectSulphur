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
      kBool,
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
      kQuaternion,
      kColor,
      kVoid,
      kFunction,
      kUserdata,
      kArgs,
      kScriptHandle
    }

    public enum RefType
    {
      kPointer,
      kReference,
      kNormal
    }

    static Dictionary<string, SupportedTypes> kNameToType = new Dictionary<string, SupportedTypes>
    {
      { "bool", SupportedTypes.kBool },
      { "float", SupportedTypes.kFloat },
      { "double", SupportedTypes.kDouble },
      { "int", SupportedTypes.kInt },
      { "char", SupportedTypes.kInt },
      { "int32_t", SupportedTypes.kInt },
      { "int64_t", SupportedTypes.kInt },
      { "size_t", SupportedTypes.kInt },
      { "unsigned int", SupportedTypes.kUnsignedInt },
      { "unsigned char", SupportedTypes.kUnsignedInt },
      { "uint32_t", SupportedTypes.kUnsignedInt },
      { "uint64_t", SupportedTypes.kUnsignedInt },
      { "String", SupportedTypes.kString },
      { "vec2", SupportedTypes.kVector2 },
      { "vec3", SupportedTypes.kVector3 },
      { "mat3", SupportedTypes.kMatrix3x3 },
      { "mat4", SupportedTypes.kMatrix4x4 },
      { "quat", SupportedTypes.kQuaternion },
      { "Color", SupportedTypes.kColor },
      { "void", SupportedTypes.kVoid },
      { "ScriptableCallback", SupportedTypes.kFunction },
      { "ScriptHandle", SupportedTypes.kScriptHandle },
      { "ScriptableArgs", SupportedTypes.kArgs }
    };

    public class TypeDefinition
    {
      public TypeDefinition(Json.Object from)
      {
        if (from.GetValue("name") != null)
        {
          name = ((Json.String)from.GetValue("name")).GetValue();
          ref_type = RefType.kNormal;
        }
        else
        {
          string r = ((Json.String)from.GetValue("type")).GetValue();

          switch (r)
          {
            case "pointer":
              ref_type = RefType.kPointer;
              break;

            case "reference":
              ref_type = RefType.kReference;
              break;

            default:
              ref_type = RefType.kNormal;
              break;
          }

          name = ((Json.String)((Json.Object)from.GetValue("baseType")).GetValue("name")).GetValue();
        }

        unqualified_name = name;
        type = StringToType(name);

        switch (ref_type)
        {
          case RefType.kPointer:
            name += "*";
            break;

          case RefType.kReference:
            name += "&";
            break;
        }
      }

      public string name;
      public string unqualified_name;
      public SupportedTypes type;
      public RefType ref_type;
    }

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
        public Arg(string n, Json.Object typedef)
        {
          name = n;
          type = new TypeDefinition(typedef);
        }

        public string name;
        public TypeDefinition type;
      }

      public Function(string n, Json.Object ret_type)
      {
        name = n;
        return_type = new TypeDefinition(ret_type);
        args = new List<Arg>();
        is_static = false;
      }

      public string name;
      public TypeDefinition return_type;
      public List<Arg> args;
      public bool is_static;
    }

    public class Class
    {
      public Class(string n)
      {
        name = n;
        funcs = new List<Function>();
        enums = new List<Enum>();
        is_component = false;
      }

      public string name;
      public string lua_name;

      public List<Function> funcs;
      public List<Enum> enums;
      public string namespaces;
      public bool is_component;
    }

    protected static void Error()
    {
      Console.WriteLine("Invalid JSON file");
    }

    protected static SupportedTypes StringToType(string name)
    {
      string key;
      foreach (KeyValuePair<string, SupportedTypes> kv in kNameToType)
      {
        key = kv.Key;
        if (key == name || name.Contains(key))
        {
          return kv.Value;
        }
      }

      return SupportedTypes.kUserdata;
    }

    protected static void AddFunction(Class cl, Json.Object mem, string name)
    {
      Function f = new Function(name, ((Json.Object)mem.GetValue("returnType")));

      Json.Value meta;
      if ((meta = mem.GetValue("meta")) != null)
      {
        Dictionary<string, Json.Value>.KeyCollection keys = ((Json.Object)meta).GetKeys();
        if (keys.Contains("static") == true)
        {
          f.is_static = true;
        }
      }

      Json.Array args = (Json.Array)mem.GetValue("arguments");

      Json.Object arg;
      Function.Arg farg;

      for (int i = 0; i < args.GetCount(); ++i)
      {
        arg = (Json.Object)args.GetValue(i);

        farg = new Function.Arg(
          ((Json.String)arg.GetValue("name")).GetValue(),
          ((Json.Object)arg.GetValue("type")));

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

    protected static void CheckMacro(Class cl, Json.Object mem)
    {
      string name = ((Json.String)mem.GetValue("name")).GetValue();
      if (name == "SCRIPT_NAME")
      {
        AddLuaName(cl, mem);
      }
      else if (name == "SCRIPT_COMPONENT")
      {
        cl.is_component = true;
      }
    }

    protected static bool AddLuaName(Class cl, Json.Object mem)
    {
      Json.Object meta = (Json.Object)mem.GetValue("meta");

      if (meta == null)
      {
        return false;
      }

      cl.lua_name = meta.GetKeys().First();
      return true;
    }

    protected static void RecursiveParse(Json.Array root, List<Class> classes, string ns, string json)
    {
      bool verbose = false;
      Json.Value v;
      Json.String type, name;
      Json.Object cl;
      Json.Array members;
      Json.Object mem;

      Class new_class;

      string class_name;
      string mem_type, mem_name;

      for (int i = 0; i < root.GetCount(); ++i)
      {
        if ((v = root.GetValue(i)).IsType<Json.Object>() == true)
        {
          cl = (Json.Object)v;
          type = (Json.String)cl.GetValue("type");

          if (type.GetValue() == "include")
          {
            continue;
          }

          if (type.GetValue() == "namespace")
          {
            string new_ns = ns + (ns.Length == 0 ? "" : "::") + ((Json.String)cl.GetValue("name")).GetValue();
            RecursiveParse((Json.Array)cl.GetValue("members"), classes, new_ns, json);
          }

          if (type.GetValue() != "class")
          {
            continue;
          }

          if (cl.GetKeys().Contains("meta") == true)
          {
            if (((Json.Object)cl.GetValue("meta")).GetKeys().Contains("verbose") == true)
            {
              verbose = true;
            }
          }

          name = (Json.String)cl.GetValue("name");
          class_name = name.GetValue();

          Console.WriteLine("Parsing definition for: " + class_name);
          new_class = new Class(class_name);
          new_class.namespaces = ns;

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
                CheckMacro(new_class, mem);
                break;
            }
          }

          Console.WriteLine("-- Lua name: " + new_class.lua_name);
          Console.WriteLine("-- Number of functions: " + new_class.funcs.Count);
          Console.WriteLine("-- Number of enums: " + new_class.enums.Count);
          Console.WriteLine("-- Namespaces: " + new_class.namespaces);

          classes.Add(new_class);
        }
      }

      if (verbose == true)
      {
        Console.Write(json);
      }
    }

    public static bool Parse(string json, out List<Class> classes)
    {
      classes = new List<Class>();

      Json.Value root;
      Json.Convert(json, out root);
      
      RecursiveParse((Json.Array)root, classes, "", json);
     
      return true;
    }
    
    public static string ExpandFile(string input_file, string func_macro)
    {
      string lines = "";
      List<string> to_expand = new List<string>();

      using (StreamReader f = File.OpenText(input_file))
      {
        string ex_func = func_macro + "_EX()";
        string line = "";
        string func_def = "";
        int idx;

        while (f.EndOfStream == false)
        {
          line = f.ReadLine();

          if (line.Contains(ex_func) == true)
          {
            func_def = "";
            idx = line.IndexOf(ex_func) + ex_func.Length;

            char current = line[idx];
            while (current != ';')
            {
              func_def += current;
              ++idx;
              current = line[idx];
            }

            func_def += ';';

            to_expand.Add(func_def);
          }

          lines += line + "\n";
        }
      }

      string expanded = "";

      foreach (string ex in to_expand)
      {
        expanded += func_macro + "() " + ex + "\n";
      }

      lines = lines.Replace("SCRIPT_EXPAND();", expanded);

      using (StreamWriter f = new StreamWriter(File.OpenWrite("temp.h")))
      {
        f.Write(lines);
      }

      return "temp.h";
    }
  }
}
