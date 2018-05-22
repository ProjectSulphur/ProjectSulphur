using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace lua_parser
{
  class HeaderParser
  {
    public static int Run(string args, out string output)
    {
      ProcessStartInfo psi = new ProcessStartInfo();
      psi.WorkingDirectory = "";
      psi.FileName = "header-parser.exe";
      psi.Arguments = args;
      psi.UseShellExecute = false;
      psi.RedirectStandardOutput = true;

      Process p = Process.Start(psi);

      output = p.StandardOutput.ReadToEnd();
      p.WaitForExit();

      return p.ExitCode;
    }
  }
}
