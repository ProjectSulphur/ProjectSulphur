using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Controls;
using System.Windows.Input;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.MenuBar : UserControl, sulphur.editor.ISubscribeable
   * @brief User control containing the top menus of the editor.
   * @see sulphur.editor.ISubscribeable 
   * @author Stan Pepels
   */
  public partial class MenuBar : UserControl, ISubscribeable
  {
    #region ISubscribable
    /**
     * @brief Notifications that this class can send
     */
    public enum Notifications
    {
      kProjectLoaded //!< A project was successfully loaded.
    }

    /**
      *@brief sulphur.editor.ISubscribeable.GetSubscriptions function implementation.
      *@remark this system is not subscribed to any other system.  
      *@see sulphur.editor.Subscription 
      */
    public Subscription[] GetSubscriptions()
    {
      return null;
    }

    /**
       *@brief event fired when subscribers need to be notified.
       *@see sulphur.editor.OnNotification 
       */
    private event OnNotification notify_subscribers_;

    /**
       *@see sulphur.editor.ISubscribeable.notify_event implementation.
       */
    event OnNotification ISubscribeable.notify_event
    {
      add
      {
        notify_subscribers_ += value;
      }
      remove
      {
        notify_subscribers_ -= value;
      }
    }
    #endregion

    /**
     * @brief initializes the WPF components.
     */
    public MenuBar()
    {
      InitializeComponent();
    }

    private ICommand open_file_cmd_; //!< Command value used for the open file menu item.
    private ICommand save_cmd_; //!< Command value used for the save menu item.
    private ICommand create_cmd_; //!< Command value used for the create menu item.

    /**
     * @brief Property used to get or set the command to be used when the open file menu item has been selected.
     */
    public ICommand open_file_cmd
    {
      get
      {
        return open_file_cmd_ ??
          (open_file_cmd_ = new utils.CommandHandler((object param) => OpenFile(param), true));
      }
      set
      {
        open_file_cmd_ = value;
      }
    }

    /**
     * @brief Property used to get or set the command to be used when the save menu item has been selected.
     */
    public ICommand save_cmd
    {
      get
      {
        return save_cmd_ ??
          (save_cmd_ = new utils.CommandHandler((object param) => Save(param), true));
      }
    }

    /**
     * @brief Property used to get or set the command to be used when the create menu item has been selected.
     */
    public ICommand create_cmd
    {
      get
      {
        return create_cmd_ ??
          (create_cmd_ = new utils.CommandHandler((object param) => CreateProject(param), true));
      }
    }

    /**
     * @brief Commands that loads a new project.
     * @param[in | opt] param (object) Optional parameter that can be passed with the command.
     */
    private void OpenFile(object param)
    {
      OpenFileDialog dialog = new OpenFileDialog();
      dialog.Filter = "Sulphur engine projects (*.spr)| *.spr";
      dialog.FileOk += (object sender, CancelEventArgs e) =>
      {
        if (Project.Load(dialog.FileName) == true)
        {
          NotificationEventArgs args = new NotificationEventArgs(null, (uint)Notifications.kProjectLoaded, id<MenuBar>.type_id_);
          notify_subscribers_?.Invoke(this, args);
          native.messages.LoadProjectMessage msg = new native.messages.LoadProjectMessage(Project.directory_path);
          byte[] data = Utils.StructToBytes(msg);
          native.Networking.SNetSendData((int)native.NetworkMessages.kLoadProject, data);
        }
      };
      dialog.ShowDialog();
    }

    /**
     * @brief Commands that creates a new project.
     * @param[in | opt] param (object) Optional parameter that can be passed with the command.
     */
    private void CreateProject(object param)
    {
      SaveFileDialog dialog = new SaveFileDialog();
      dialog.Filter = "Sulphur engine projects (*.spr)| *.spr";
      dialog.FileOk += (object sender, CancelEventArgs e) =>
      {
        if (Project.CreateNew(dialog.FileName) == true)
        {
          NotificationEventArgs args = new NotificationEventArgs(null, (uint)Notifications.kProjectLoaded, id<MenuBar>.type_id_);
          notify_subscribers_?.Invoke(this, args);

          native.messages.LoadProjectMessage msg = new native.messages.LoadProjectMessage(Project.directory_path);
          native.Networking.SNetSendData((int)native.NetworkMessages.kLoadProject, Utils.StructToBytes(msg));
        }
      };
      dialog.ShowDialog();
    }

    /**
     * @brief Commands that saves a project.
     * @param[in | opt] param (object) Optional parameter that can be passed with the command.
     */

    private void Save(object param)
    {
      WorldHierarchy hierarchy = (WorldHierarchy)param;
      hierarchy.hierarchy.SaveToDisk();

      // todo: save project changes
    }
  }
}
