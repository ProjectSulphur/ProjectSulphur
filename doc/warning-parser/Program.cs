using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace warning_parser
{
  class Program
  {
    static string[] kToExclude =
    {
      "warning: the name",
      "warning: Detected potential recursive class relation",
      "TeX"
    };

    static string[] kSplitAt =
    {
      "src",
      "external"
    };

    static string StripPath(string input)
    {
      string output = "";
      string[] split = input.Split('/');

      if (split.Length == 0 || split.Length == 1)
      {
        return input;
      }

      bool found = false;
      int i = 0;
      for (; i < split.Length; ++i)
      {
        found = false;
        foreach (string s in kSplitAt)
        {
          if (split[i] == s)
          {
            found = true;
            break;
          }
        }

        if (found == true)
        {
          ++i;
          break;
        }
      }

      for (; i < split.Length; ++i)
      {
        output += split[i] + (i == split.Length - 1 ? "" : "/");
      }

      if (output.Contains(" warning: ") == false)
      {
        return output;
      }

      bool first = true;
      char ch;

      int j = 0;
      string stripped = "<b>";
      for (; j < output.Length; ++j)
      {
        ch = output[j];
        stripped += ch;

        if (ch == ':')
        {
          if (first == true)
          {
            first = false;
            continue;
          }

          ++j;
          break;
        }
      }

      stripped += "</b>";

      for (; j < output.Length; ++j)
      {
        stripped += output[j];
      }

      return stripped;
    }

    static void Main(string[] args)
    {
      if (args.Length < 1)
      {
        return;
      }

      string input_file = args[0];

      string[] lines = File.ReadAllLines(input_file);

      string output = "";
      string stripped = "";

      bool skip = false;
      foreach (string line in lines)
      {
        skip = false;
        foreach (string e in kToExclude)
        {
          if (line.Contains(e))
          {
            skip = true;
            break;
          }
        }

        if (skip == true || line.Length == 0)
        {
          continue;
        }

        stripped = StripPath(line);
        output += stripped + "<br>";
        Console.WriteLine(stripped);
      }

      string html =
        "<!DOCTYPE HTML>" +
        "<head>" +
        "\t<title>Doxygen warnings</title>" +
        "\t<style>" +
        "\t\t*" +
        "\t\t{" +
        "\t\t\tfont-family: Verdana;" +
        "\t\t}" +
        "\t</style>" +
        "</head>" +
        "<body>" +
        output +
        "</body>";

      File.WriteAllText(input_file.Split('.')[0] + "_parsed.html", html);
    }
  }
}
