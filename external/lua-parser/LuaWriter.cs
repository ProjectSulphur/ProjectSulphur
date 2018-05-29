using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace lua_parser
{
  class LuaWriter
  {
    protected static string[] kIncludePaths =
    {
      "engine/scripting/script_state.h",
      "engine/scripting/script_utils.h",
      "engine/scripting/scriptable_values/scriptable_args.h",
      "<glm/glm.hpp>"
    };

    protected struct FuncDecl
    {
      public string static_name;
      public string func_name;
    }

    protected static char TypeToWrapperChar(Parser.SupportedTypes type)
    {
      switch (type)
      {
        case Parser.SupportedTypes.kBool:
          return 'B';

        case Parser.SupportedTypes.kInt:
        case Parser.SupportedTypes.kUnsignedInt:
        case Parser.SupportedTypes.kFloat:
        case Parser.SupportedTypes.kDouble:
          return 'N';

        case Parser.SupportedTypes.kString:
          return 'S';

        case Parser.SupportedTypes.kUserdata:
          return 'U';

        case Parser.SupportedTypes.kVector2:
        case Parser.SupportedTypes.kVector3:
        case Parser.SupportedTypes.kVector4:
        case Parser.SupportedTypes.kMatrix3x3:
        case Parser.SupportedTypes.kMatrix4x4:
        case Parser.SupportedTypes.kQuaternion:
        case Parser.SupportedTypes.kColor:
          return 'O';

        case Parser.SupportedTypes.kFunction:
          return 'F';

        default:
          return 'U';
      }
    }

    protected static void WriteCommentLine(StreamWriter stream)
    {
      stream.Write("\n//---------------------------------------------------------\n");
    }

    protected static FuncDecl WriteFunction(StreamWriter stream, Parser.Class cl, Parser.Function func)
    {
      WriteCommentLine(stream);

      FuncDecl decl;
      decl.static_name = "lua_" + cl.name + "_" + func.name;
      decl.func_name = func.name;

      stream.Write(
           "void " + decl.static_name + "(ScriptableArgs* args)\n" +
          "{\n");

      string check = func.is_static == false ? "U" : "";
      bool is_args = func.args.Count > 0 && func.args[0].type.type == Parser.SupportedTypes.kArgs;

      if (is_args == false)
      {
        foreach (Parser.Function.Arg arg in func.args)
        {
            check += TypeToWrapperChar(arg.type.type);
        }
      }

      if (is_args == false)
      {
        stream.Write("\targs->Check(\"" + check + "\");\n\n");
      }

      if (func.is_static == false)
      {
        stream.Write("\t" + cl.name + "* self = ScriptableObject::CheckUserData<" + cl.name + ">(" + cl.name + "::Name(), args->GetArg(0));\n");
      }

      Parser.Function.Arg a;
      string type_name;
      string get_arg;
      int aidx;

      int nargs = func.args.Count;

      for (int i = 0; i < nargs; ++i)
      {
        aidx = func.is_static == true ? i : i + 1;

        a = func.args[i];
        type_name = a.type.unqualified_name;
        get_arg = "args->GetArg(" + aidx + ")";

        if (a.type.type == Parser.SupportedTypes.kUserdata)
        {
          if (a.type.ref_type == Parser.RefType.kPointer)
          {
            stream.Write("\t" + type_name + "* arg_" + i + " = ScriptableObject::CheckUserData<" + type_name + ">(" + type_name + "::Name(), " + get_arg + ");\n");
            continue;
          }
        }
        else if (a.type.type == Parser.SupportedTypes.kArgs)
        {
            if (a.type.ref_type == Parser.RefType.kPointer)
            {
                stream.Write("\tScriptableArgs* arg_" + i + " = args;");
                continue;
            }
        }

        stream.Write("\t" + type_name + " arg_" + i + " = ScriptUtils::As<" + type_name + ">(" + get_arg + ");\n");
      }

      stream.Write("\n\t");

      if (func.return_type.type != Parser.SupportedTypes.kVoid)
      {
        stream.Write(func.return_type.name + " ret = ");
      }

      if (func.is_static == true)
      {
        stream.Write(cl.name + "::" + func.name + "(");
      }
      else
      {
        stream.Write("self->" + func.name + "(");
      }

      if (nargs == 0)
      {
        stream.Write(");\n");
      }
      
      for (int i = 0; i < nargs; ++i)
      {
        stream.Write("arg_" + i);

        if (i == nargs - 1)
        {
          stream.Write(");\n");
          continue;
        }

        stream.Write(", ");
      }

      if (func.return_type.type != Parser.SupportedTypes.kVoid)
      {
        if (func.return_type.type == Parser.SupportedTypes.kUserdata)
        {
          stream.Write("\targs->AddUserData(ret);\n");
        }
        else
        {
          stream.Write("\targs->AddReturn(ret);\n");
        }
      }

      stream.Write("}\n");

      return decl;
    }

    delegate void RegisterWriter(List<FuncDecl> decl);
    protected static void WriteFunctionRegisters(StreamWriter stream, Parser.Class cl, List<FuncDecl> decl_member, List<FuncDecl> decl_static)
    {
      RegisterWriter WriteAll = (List<FuncDecl> decl) =>
      {
        stream.Write(
          "\treturn\n" +
          "\t{");

        if (decl.Count == 0)
        {
          stream.Write("};");
          return;
        }

        stream.Write("\n");

        FuncDecl d;
        for (int i = 0; i < decl.Count; ++i)
        {
          d = decl[i];
          stream.Write("\t\t{ \"" + d.func_name + "\", " + d.static_name + " }");

          if (i == decl.Count - 1)
          {
            stream.Write(
              "\n" +
              "\t};");

            continue;
          }

          stream.Write(",\n");
        }
      };

      WriteCommentLine(stream);

      string register_decl = "sulphur::foundation::Vector<ScriptableFunctionRegister> " + cl.name + "::";
      stream.Write(
        register_decl + "FunctionRegister()\n" +
        "{\n");

      WriteAll(decl_static);
      stream.Write("\n}\n\n");

      WriteCommentLine(stream);

      stream.Write(
        register_decl + "MetaRegister()\n" +
        "{\n");

      WriteAll(decl_member);
      stream.Write("\n}\n\n");
    }

    protected static void WriteEnum(StreamWriter stream, Parser.Enum e)
    {
      stream.Write(
        "\t\t\t\"" + e.name + "\",\n" +
        "\t\t\t{\n");

      foreach (KeyValuePair<string, int> kv in e.values)
      {
        stream.Write("\t\t\t\t{ \"" + kv.Key + "\", " + kv.Value + " }");
        stream.Write((kv.Equals(e.values.Last()) == false ? "," : "") + "\n");
      }

      stream.Write("\t\t\t}\n");
    }

    protected static void WriteEnumRegisters(StreamWriter stream, Parser.Class cl)
    {
      WriteCommentLine(stream);
      stream.Write(
        "sulphur::foundation::Vector<ScriptableEnumRegister> " + cl.name + "::EnumRegister()\n" +
        "{\n" +
        "\treturn\n" +
        "\t{\n");

      Parser.Enum e;
      int n = cl.enums.Count;

      for (int i = 0; i < n; ++i)
      {
        stream.Write("\t\t{\n");

        e = cl.enums[i];
        WriteEnum(stream, e);

        stream.Write("\t\t}" + (i != n - 1 ? "," : "") + "\n");
      }

      stream.Write(
        "\t};\n" +
        "}\n");
    }

    protected static void WriteComponentRegister(StreamWriter stream, Parser.Class cl)
    {
      WriteCommentLine(stream);
      stream.Write(
        "sulphur::engine::ComponentIDBase* " + cl.name + "::ComponentRegister()\n" +
        "{\n" +
        "\treturn sulphur::foundation::Memory::Construct<ComponentID<" + cl.name + ">>();\n" +
        "}\n"
      );
    }

    protected static void WriteClass(StreamWriter stream, Parser.Class cl)
    {
      string[] namespaces = cl.namespaces.Split(new string[] { "::" }, StringSplitOptions.None);

      foreach (string ns in namespaces)
      {
        stream.Write("namespace " + ns + "\n{\n");
      }

      List<FuncDecl> decl_member = new List<FuncDecl>();
      List<FuncDecl> decl_static = new List<FuncDecl>();

      FuncDecl decl;
      foreach (Parser.Function func in cl.funcs)
      {
        decl = WriteFunction(stream, cl, func);

        if (func.is_static == true)
        {
          decl_static.Add(decl);
          continue;
        }

        decl_member.Add(decl);
      }

      WriteFunctionRegisters(stream, cl, decl_member, decl_static);
      WriteEnumRegisters(stream, cl);

      if (cl.is_component == true)
      {
        WriteComponentRegister(stream, cl);
      }

      foreach (string ns in namespaces)
      {
        stream.Write("\n}");
      }
    }

    protected static void WriteIncludes(StreamWriter stream, string wd)
    {
      stream.Write(
        "#include \"" + wd + "\"\n");

      string to_write;
      foreach (string inc in kIncludePaths)
      {
        to_write = inc;
        if (inc.Contains("<") == false)
        {
          to_write = "\"" + inc + "\"";
        }

        stream.Write("#include " + to_write + "\n");
      }

      stream.Write("\n");
    }

    protected static string ParseWorkingDir(string input_file)
    {
      string wd = System.Environment.CurrentDirectory + "/";
      if (wd.Length == 1 || input_file.Contains(':') == true)
      {
        wd = "";
      }

      wd = wd.Replace('\\', '/');
      input_file = input_file.Replace('\\', '/');

      wd += input_file;

      return wd;
    }

    public static bool Write(List<Parser.Class> classes, string input_file, string output_file)
    {
      string working_dir = ParseWorkingDir(input_file);

      using (FileStream fout = new FileStream(output_file, FileMode.OpenOrCreate, FileAccess.Write))
      using (StreamWriter stream = new StreamWriter(fout))
      {
        fout.SetLength(0);

        stream.Write(
          "/**\n" +
          "* ========================================\n" +
          "* Generated by Sulphur's lua-parser\n" +
          "* Do not modify this file manually\n" +
          "* ========================================\n" +
          "* Specifications: \n" +
          "* \n" +
          "* - Class data: \n");

        foreach (Parser.Class cl in classes)
        {
          stream.Write("*\t[Name: " + cl.name + ", Lua name: " + cl.lua_name + "]\n");
          stream.Write("*\t- Functions: " + cl.funcs.Count + "\n");
          stream.Write("*\t- Enums: " + cl.enums.Count + "\n");
        }

        stream.Write(
          "*/\n" +
          "\n");

        WriteIncludes(stream, working_dir);

        foreach (Parser.Class cl in classes)
        {
          WriteClass(stream, cl);
        }
      }

      return true;
    }
  }
}
