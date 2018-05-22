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
      { "c", "LUA_CLASS" },
      { "e", "LUA_ENUM" },
      { "f", "LUA_FUNC" },
      { "m", "LUA_NAME" }
    };

    static bool GetInputArgs(string[] args, out string output)
    {
      if (args.Length == 0)
      {
        Console.WriteLine("Invalid Lua parser usage: input");
        output = "";
        return false;
      }

      string a = args[0];

      foreach (KeyValuePair<string, string> arg in kArgs)
      {
        a += " -" + arg.Key + " " + arg.Value;
      }

      output = a;
      return true;
    }

    static int Main(string[] args)
    {
      string input;
      if (GetInputArgs(args, out input) == false)
      {
        return (int)ExitCodes.kInvalidFile;
      }

      string output;
      if (HeaderParser.Run(input, out output) != 0)
      {
        return (int)ExitCodes.kParsingFailed;
      }

      List<Parser.Class> classes;
      Parser.Parse(output, out classes);

      LuaWriter.Write(classes);

      Console.ReadKey();
      
      return (int)ExitCodes.kSuccess;
    }
  }
}
