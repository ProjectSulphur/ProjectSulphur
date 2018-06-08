using System.Xml;
using System.IO;

namespace sulphur.editor
{
  /**
   * @class sulphur.editor.Project
   * @brief Class responsible for doing operation such as loading projects and creating new projects.
   * @author Stan Pepels
   */
  class Project
  {
    /**
     * @brief Absolute directory path to the directory the *.spr file used to load the current project is located in. 
     */
    public static string directory_path { get; private set; } = "";

    /**
     * @brief Absolute file path to the *.spr file used to load the current project.
     */
    public static string path_ { get; private set; } = "";

    /**
     * @brief Name of the currently loaded project.
     */
    public static string name_ { get; private set; } = "";

    /**
     * @brief Project relative path to the current world being edited.
     */
    public static string current_world_ { get; set; } = "";

    /**
     * @brief Loads a new project at a given filepath
     * @param[in] path (string) Absolute or relative filepath to a *.spr file.
     * @return (bool) True if the project was loaded succesfully, false otherwise.
     * @remark This function reinitializes the asset pipelines.
     */
    public static bool Load(string path)
    {
      path_ = path;
      directory_path = path.Substring(0, path.LastIndexOf("\\"));

      XmlReaderSettings settings = new XmlReaderSettings();
      XmlReader reader = XmlReader.Create(path, settings);
      using (reader)
      {
        while (reader.Read())
        {
          if (reader.IsStartElement() && reader.Name == "ProjectSettings")
          {
            ReadSettings(reader);
          }
        }
      }
      native.AssetProcessor.SetProjectDirectory(directory_path);
      native.AssetProcessor.SetOutputPath(directory_path);
      native.AssetProcessor.SetPackageOutputPath("resources");
      native.AssetProcessor.CreateDefaults();
      return true;
    }

    /**
     * @brief Creates a new project at a given path.
     * @param[in] path (string) Location of the *.spr.
     * @return (bool) True if the project was created succesfully, false otherwise.
     * @remarks This will create an "assets" and "resources" folder in the project directory. 
     * @remarks Calling this function will reinitialize the asset pipelines.
     * @remarks Calling this function will overwrite any project allready located in the target directory.
     */
    public static bool CreateNew(string path)
    {
      path_ = path;
      directory_path = path.Substring(0, path.LastIndexOf("\\"));
      // create the folders required
      if (Directory.Exists(directory_path + "\\resources") == false)
      {
        Directory.CreateDirectory(directory_path + "\\resources");
      }

      if (Directory.Exists(directory_path + "\\assets") == false)
      {
        Directory.CreateDirectory(directory_path + "\\assets");
      }
      
      current_world_ = "assets\\main.swo";

      // write the settings to a new project file
      XmlWriterSettings settings = new XmlWriterSettings();
      settings.Indent = true;
      settings.IndentChars = "\t";
      XmlWriter xml_writer = XmlWriter.Create(path, settings);
      using (xml_writer)
      {
        xml_writer.WriteStartElement("ProjectSettings");
        xml_writer.WriteAttributeString("Name", path.Substring(path.LastIndexOf("\\") + 1));
        xml_writer.WriteElementString("World", current_world_);
        xml_writer.WriteEndElement();
        xml_writer.Close();
      }
      native.AssetProcessor.SetProjectDirectory(directory_path);
      native.AssetProcessor.SetOutputPath(directory_path);
      native.AssetProcessor.SetPackageOutputPath("resources");
      native.AssetProcessor.CreateDefaults();

      if(CreateEmptyWorld() == false)
      {
        Log.Write(Log.Verbosity.kError, "Unable to create or register world");
        return false;
      }

      if (CreateEmptyMainScript() == false)
      {
        Log.Write(Log.Verbosity.kError, "Unable to create or register empty main script");
        return false;
      }
      return true;
    }

    /**
     * @brief Reads the settings from a *.spr file.
     * @param[in] reader (XmlReader) Reader that reads the *.spr file with its read location set to the 'ProjectSettings' element.
     */
    private static void ReadSettings(XmlReader reader)
    {
      reader.MoveToFirstAttribute();
      name_ = reader[0].Remove(reader[0].IndexOf("."));
      reader.MoveToElement();

      while (reader.Read())
      {
        switch (reader.Name)
        {
          case "World":
            current_world_ = reader.ReadElementContentAsString();
            break;
        }

        if (reader.Name == "ProjectSettings" && reader.NodeType == XmlNodeType.EndElement)
        {
          break;
        }
      }
    }

    private static bool CreateEmptyMainScript()
    {
      StreamWriter file_writer = new StreamWriter(directory_path + "\\assets\\main.lua");
      file_writer.WriteLine("function OnInitialize()");
      file_writer.WriteLine("end");
      file_writer.WriteLine();
      file_writer.WriteLine("function Update(dt)");
      file_writer.WriteLine("end");
      file_writer.WriteLine();
      file_writer.WriteLine("function FixedUpdate()");
      file_writer.WriteLine("end");
      file_writer.Close();

      ulong id = 0;
      native.AssetProcessor.RegisterWorld(current_world_, ref id);
      return native.AssetProcessor.ImportScript("assets\\main.lua", ref id);
    }

    private static bool CreateEmptyWorld()
    {
      File.CreateText(directory_path + "\\assets\\main.swo").Close();
      File.CreateText(directory_path + "\\resources\\main.sbw").Close();
      ulong id = 0;
      return native.AssetProcessor.RegisterWorld(current_world_, ref id);
    }
  }
}
