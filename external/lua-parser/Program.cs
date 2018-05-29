using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace lua_parser
{
  class Program
  {
    enum ExitCodes
    {
      kSuccess,
      kInvalidFile,
      kParsingFailed
    }

    static Dictionary<string, string> kArgs = new Dictionary<string, string>
    {
      { "c", "SCRIPT_CLASS" },
      { "e", "SCRIPT_ENUM" },
      { "f", "SCRIPT_FUNC" }
    };

    static List<string> kMacros = new List<string>
    {
      "SCRIPT_NAME",
      "SCRIPT_COMPONENT"
    };
    
    static bool GetInputArgs(string[] args, out string output, out string input_file, out string output_file, out string temp_file)
    {
      if (args.Length < 4)
      {
        Console.WriteLine("Invalid Lua parser usage: -i path/to/input.h -o path/to/output.h");
        output = input_file = output_file = temp_file = "";
        return false;
      }

      input_file = "";
      output_file = "";

      for (int i = 0; i < args.Length; ++i)
      {
        if (args[i] == "-i")
        {
          input_file = args[i + 1];
        }
        else if (args[i] == "-o")
        {
          output_file = args[i + 1];
        }

        ++i;
      }

      temp_file = Parser.ExpandFile(input_file, kArgs["f"]);

      string a = temp_file;

      foreach (KeyValuePair<string, string> arg in kArgs)
      {
        a += " -" + arg.Key + " " + arg.Value;
      }

      foreach (string arg in kMacros)
      {
        a += " -m " + arg;
      }

      output = a;
      return true;
    }

    static int Main(string[] args)
    {
      string input;
      string input_file;
      string output_file;
      string temp_file;

      if (GetInputArgs(args, out input, out input_file, out output_file, out temp_file) == false)
      {
        return (int)ExitCodes.kInvalidFile;
      }

      string output;

      Console.WriteLine("Executing lua-parser with: " + input);
      if (HeaderParser.Run(input, out output) != 0)
      {
        return (int)ExitCodes.kParsingFailed;
      }

      List<Parser.Class> classes;
      Parser.Parse(output, out classes);

      LuaWriter.Write(classes, input_file, output_file);

      System.IO.File.Delete(temp_file);
      
      return (int)ExitCodes.kSuccess;
    }
  }
}
