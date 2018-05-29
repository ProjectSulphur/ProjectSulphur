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
     * @brief Id of the currently loaded world.
     */
    public int curr_world_id { get; private set; }

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
      
      native.AssetProcessor.SetOutputPath(directory_path);
      native.AssetProcessor.SetPackageOutputPath(".\\resources");
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

      // write the settings to a new project file
      XmlWriterSettings settings = new XmlWriterSettings();
      settings.Indent = true;
      settings.IndentChars = "\t";
      XmlWriter writer = XmlWriter.Create(path, settings);
      using (writer)
      {
        writer.WriteStartElement("ProjectSettings");
        writer.WriteAttributeString("Name", path.Substring(path.LastIndexOf("\\") + 1));
        writer.WriteElementString("Directory", directory_path);
        writer.WriteEndElement();
      }

      // create the folders required
      if (Directory.Exists(directory_path + "\\resources") == false)
      {
        Directory.CreateDirectory(directory_path + "\\resources");
      }

      if (Directory.Exists(directory_path + "\\assets") == false)
      {
        Directory.CreateDirectory(directory_path + "\\assets");
      }

      StreamWriter file_writer = new StreamWriter(directory_path + "\\assets\\main.lua");
      file_writer.WriteLine("function OnInitialize()");
      file_writer.WriteLine("end");
      file_writer.WriteLine();
      file_writer.WriteLine("function Update(dt)");
      file_writer.WriteLine("end");
      file_writer.WriteLine();
      file_writer.WriteLine("function FixedUpdate()");
      file_writer.WriteLine("end");
      file_writer.Flush();
      file_writer.Close();
      
      native.AssetProcessor.SetOutputPath(directory_path);
      native.AssetProcessor.SetPackageOutputPath(".\\resources");
      native.AssetProcessor.CreateDefaults();

      ulong id = 0;
      if (native.AssetProcessor.ImportScript(directory_path + "\\assets\\main.lua", ref id) == false)
      {
        return false;
      }
      
      //TODO: create a new world upon project creation
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
          case "Directory":
            directory_path = reader.ReadElementContentAsString();
            break;
        }

        if (reader.Name == "ProjectSettings" && reader.NodeType == XmlNodeType.EndElement)
        {
          break;
        }
      }
    }
  }
}
