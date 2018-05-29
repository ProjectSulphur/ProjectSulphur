using Microsoft.Win32;
using System;
using System.Windows;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.ProjectWindow : Window
   * @brief Window that gets opened when the editor starts without a file. Used to open a project or create a new one.
   * @author Stan Pepels.
   */
  public partial class ProjectWindow : Window
  {
    public event EventHandler on_project_ok; //!< Event fired when a project was succesfully loaded or created.

    /**
     * @brief Intializes the WPF components of the control.
     */
    public ProjectWindow()
    {
      InitializeComponent();
    }

    /**
     * @brief Handles the click event of the new project menu item. This will open a file dialog and try to create a new project from the result of that dialog.
     * @param[in] sender (object) MenuItem that send the click event.
     * @param[in] e (RoutedEventArgs)  Arguments used to process the event.
     */
    private void NewProjectMenuClick(object sender, RoutedEventArgs e)
    {
      SaveFileDialog dialog = new SaveFileDialog();
      dialog.Filter = "Sulphur engine projects (*.spr)| *.spr";
      if(dialog.ShowDialog(this) == false)
      {
        return;
      } 

      if(Project.CreateNew(dialog.FileName) == false)
      {
        string msg = string.Format("Unable to create a new project. \n Path: {0}", dialog.FileName);
        MessageBoxResult result = MessageBox.Show(this, msg, "Project creation failed.", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.OK);
        if (result == MessageBoxResult.OK)
        {
          App.Current.Shutdown();
          Close();
        }
      }
      else
      {
        on_project_ok?.Invoke(this, new EventArgs());
      }
    }

    /**
     * @brief Handles the click event of the open project menu item. This will open a file dialog and try to open an existing project from the result of that dialog.
     * @param[in] sender (object) MenuItem that send the click event.
     * @param[in] e (RoutedEventArgs)  Arguments used to process the event.
     */
    private void OpenProjectMenuClick(object sender, RoutedEventArgs e)
    {
      OpenFileDialog dialog = new OpenFileDialog();
      dialog.Filter = "Sulphur engine projects (*.spr)| *.spr";
      if (dialog.ShowDialog(this) == false)
      {
        return;
      }

      if (Project.Load(dialog.FileName) == false)
      {
        string msg = string.Format("Unable to load existing project. \n Path: {0}", dialog.FileName);
        MessageBoxResult result = MessageBox.Show(this, msg, "Failed to load project.", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.OK);
        if(result == MessageBoxResult.OK)
        {
          App.Current.Shutdown();
          Close();
        }
      }
      else
      {
        on_project_ok?.Invoke(this, new EventArgs());
      }
    }
  }
}
