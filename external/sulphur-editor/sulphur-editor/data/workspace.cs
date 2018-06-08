using System.ComponentModel;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Collections.Generic;

namespace sulphur
{
  namespace editor
  {
    /**
     *@class sulphur.editor.DirectoryList : System.Collections.ObjectModel.ObservableCollection<sulphur.editor.DirectoryItem>
     *@brief class used for displaying the list of sulphur.editor.DirectoryItem objects in the asset brower.
     * derived from System.Collections.ObjectModel.ObservableCollection<T> so it updates automatically
     *@author Stan Pepels
     */
    public class DirectoryList : ObservableCollection<DirectoryItem>
    {
      /**
       *@brief default constructor 
       */
      public DirectoryList() : base() { }
    }

    /**
     *@class sulphur.editor.DirectoryItem : System.ComponentModel.INotifyPropertyChanged
     *@brief class representing a directory in the directory tree.
     *@author Stan Pepels   
     */
    public class DirectoryItem : INotifyPropertyChanged
    {
      /**
       *@brief event fired when a property changed. This is used to update the view in the assetbrowser.
       *@see https://msdn.microsoft.com/en-us/library/system.componentmodel.inotifypropertychanged(v=vs.110).aspx 
       */
      public event PropertyChangedEventHandler PropertyChanged;

      /**
       *@brief the parent of this directory in the directory tree.
       */
      public DirectoryItem parent { get; private set; }

      /**
       *@brief absolute path to this directory
       *@remark setting this property raises the PropertyChanged event 
       */
      public string path
      {
        get
        {
          return path_value_;
        }
        set
        {
          if (value != path_value_)
          {
            path_value_ = value;
            NotifyPropertyChanged();
          }
        }
      }

      /**
       * @brief path to this directory relative from the root item.
        *@remark setting this property raises the PropertyChanged event 
       */
      public string relative_path
      {
        get
        {
          return relative_path_value_;
        }
        set
        {
          if (value != relative_path_value_)
          {
            relative_path_value_ = value;
            NotifyPropertyChanged();
          }
        }
      }

      /**
       *@brief name of this directory
       *@remark setting this property raises the PropertyChanged event 
       */
      public string name
      {
        get
        {
          return name_value_;
        }
        set
        {
          if (value != name_value_)
          {
            name_value_ = value;
            NotifyPropertyChanged();
          }
        }
      }

      /**
       *@brief list of subdirectories in this directory.
       *@remark setting this property raises the PropertyChanged event 
       */
      public DirectoryList items
      {
        get
        {
          return items_value_;
        }
        set
        {
          if (value != items_value_)
          {
            items_value_ = value;
            NotifyPropertyChanged();
          }
        }
      }

      /**
       * @brief Prperty used to get or set the permission to edit this directory.
       */
      public bool editable
      {
        get
        {
          return editable_value_;
        }
        set
        {
          if (value != editable_value_)
          {
            editable_value_ = value;
            NotifyPropertyChanged();
          }
        }
      }


      /**
       *@brief constructor. This sets up the directory item by filling in the name, path, parent and sub_dirs fields.
       *@param[in] folder_path (string) absolute or relative path to the folder to represent 
       *@param[in] folder_name (string) name of the folder to represent 
       *@param[in] parent_dir (sulphur.editor.DirectoryItem) parent directory from the directory to represent 
       */
      public DirectoryItem(string folder_path, string folder_name, DirectoryItem parent_dir)
      {
        DirectoryInfo info = new DirectoryInfo(folder_path);
        if (info.Exists == false)
        {
          return;
        }
        path = info.FullName;
        name = info.Name;
        parent = parent_dir;
        editable = true;
        items = new DirectoryList();
        DirectoryInfo[] sub_dirs = info.GetDirectories();

        relative_path = parent_dir == null ? path.Remove(0, Project.directory_path.Length) : 
          parent_dir.relative_path + path.Remove(0, parent_dir.path.Length);

        if(relative_path.StartsWith("\\") == true)
        {
          relative_path = relative_path.Remove(0, 1);
        }

        foreach (DirectoryInfo dir in sub_dirs)
        {
          DirectoryItem item = new DirectoryItem(dir.FullName, dir.Name, this);
          items.Add(item);
        }
      }

      /**
       *@brief reload this directory item
       *@remark this function is recursive and will call casue a refresh in all sub directories as well. 
       */
      public void Refresh()
      {
        DirectoryInfo info = new DirectoryInfo(path);
        if (info.Exists == false)
        {
          return;
        }
        App.Current.Dispatcher.Invoke(delegate { items.Clear(); });


        DirectoryInfo[] sub_dirs = info.GetDirectories();
        foreach (DirectoryInfo dir in sub_dirs)
        {
          DirectoryItem item = new DirectoryItem(dir.FullName, dir.Name, this);
          App.Current.Dispatcher.Invoke(delegate { items.Add(item); });
        }

      }

      private string path_value_; //<! actual path value used by the path property property

      private string name_value_; //<! actual name value used by the name property

      private bool editable_value_ = true; //<! Set to true if it is allowed to change this directory, set to false otherwise.

      private DirectoryList items_value_; //<! actual list of sub directories used by the items property

      private string relative_path_value_; //<! actual relative path value used by the relative_path property
      /**
       *@brief implementation of the System.ComponentModel.INotifyPropertyChanged interface 
       *@param[in] property_name (string) name of the propperty that changed.
       */
      private void NotifyPropertyChanged(string property_name = "")
      {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(property_name));
      }

      /**
       * @brief Handle the renamed event of the editbox that represents this directory.
       * @param[in] sender (object) The editbox representing this item.
       * @param[in] e (sulphur.editor.controls.EditBoxRenamedEventArgs) Arguments passed with the event.
       */
      public void HandleRenamed(object param)
      {
        EditBoxRenamedEventArgs e = param as EditBoxRenamedEventArgs;
        string new_path = path.Substring(0, path.LastIndexOf("\\"));
        new_path += "\\";
        new_path += e.new_value;
        Directory.Move(path, new_path);
      }

      private ICommand rename_folder_cmd_;       //<! command for a contextmenu. this command will delete an existing folder inside a given parent folder.

      /**
       *@brief delete_folder command property used for binding by a contextmenu.
       *@see sulphur.editor.Workspace.DeleteFolder
       */
      public ICommand rename_folder_cmd
      {
        get
        {
          return rename_folder_cmd_ ??
            (rename_folder_cmd_ = new utils.CommandHandler((object param) => HandleRenamed(param),
                                                           true));
        }
      }
    }

    /**
     *@class sulphur.editor.Workspace :  System.ComponentModel.INotifyPropertyChanged, sulphur.editor.ISubscribeable
     *@brief class representing the current workspace of the open project. 
     * This class is responsible for hot reloading assets and keeping track of the directory structure of a project.
     *@remark the base folder will always be named 'assets' if none can be found a new one will be created 
     *@author Stan Pepels 
     */
    public class Workspace : INotifyPropertyChanged, ISubscribeable
    {
      public enum Notifications //!< type notifications that can be send to subscribers
      {
        kFolderCreated, //!<a new folder got added to the workspace
        kFolderRemoved, //!<a folder got removed from the workspace
        kFolderRenames, //!<a folder in the workspace was renamed / moved
        kFileCreated,   //!<a file was copied / moved into one of the workspace folders via the windows explorer
        kFileRemoved,   //!<a file was removed from one of the workspace folders via the windows explorer
        kFileRenamed,   //!<a file got renamed or moved in one of the workspace folders via the windows explorer
        kFileChanged    //!<a file got overwritten in one of the workspace folders via the windows explorer
      }

      /**
       *@brief root directory of the workspace the root will always be named 'assets' 
       *@see sulpur.editor.DirectoryItem
       */
      public DirectoryItem root { get; set; }

      /**
       *@brief the directory structure relative to the root 'assets' folder.
       *@see sulphur.editor.DirectoryList
       */
      public DirectoryList items { get; set; }

      /**
       *@brief delete_folder command property used for binding by a contextmenu.
       *@see sulphur.editor.Workspace.DeleteFolder
       */
      public ICommand delete_folder_cmd
      {
        get
        {
          return delete_folder_cmd_ ??
            (delete_folder_cmd_ = new utils.CommandHandler((object param) => DeleteFolder(param),
                                                           true));
        }
      }

      /**
      *@brief new_folder command property used for binding by a contextmenu
      *@see sulphur.editor.Workspace.NewFolder
      */
      public ICommand new_folder_cmd
      {
        get
        {
          return new_folder_cmd_ ??
            (new_folder_cmd_ = new utils.CommandHandler((object param) => NewFolder(param), true));
        }
      }

      /**
       *@brief constructor. This creates a new assets folder in the working directory this folder will serve as the workspace.
       * This also sets up the folder watcher to watch for folder changes so directory structure is updated even when focus is not on the editor.
       * The same goes for the file watcher.
       */
      public Workspace()
      {
        file_queue_ = new List<FileQueueData>();
        Application.Current.MainWindow.GotKeyboardFocus += OnMainWindowFocus;
        Application.Current.MainWindow.LostKeyboardFocus += OnMainWindowFocusLost;
        DirectoryInfo info = new DirectoryInfo(Project.directory_path + "\\assets");
        items = new DirectoryList();
        root = new DirectoryItem(info.FullName, info.Name, null);
        root.editable = false;

        items.Add(root);

        folder_watcher_ = new FileSystemWatcher();
        folder_watcher_.Path = Project.directory_path + "\\assets";
        folder_watcher_.IncludeSubdirectories = true;
        folder_watcher_.EnableRaisingEvents = true;
        folder_watcher_.NotifyFilter = NotifyFilters.DirectoryName;
        folder_watcher_.Renamed += OnFolderWatcherRenamed;
        folder_watcher_.Created += OnFolderWatcherCreated;
        folder_watcher_.Deleted += OnFolderWatcherDeleted;

        file_watcher_ = new FileSystemWatcher();
        file_watcher_.Path = Project.directory_path + "\\assets";
        file_watcher_.IncludeSubdirectories = true;
        file_watcher_.EnableRaisingEvents = false;
        file_watcher_.Filter = "*.*";
        file_watcher_.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName;
        file_watcher_.Renamed += OnFileWatcherRenamed;
        file_watcher_.Changed += FileWatcherChanged;
        file_watcher_.Deleted += FileWatcherDeleted;
        file_watcher_.Created += FileWatcherCreated;
      }

      #region ISubscribable
      /**
       *@brief sulphur.editor.ISubscribeable.GetSubscriptions function implementation.
       *@remark this system is not subscribed to any other system.  
       *@see sulphur.editor.Subscription 
       */
      public Subscription[] GetSubscriptions()
      {
        Subscription[] subscriptions = new Subscription[1];
        subscriptions[0] = new Subscription(id<controls.MenuBar>.type_id_, HandleMenuNotification);
        return subscriptions;
      }

      private void HandleMenuNotification(object sender, NotificationEventArgs e)
      {
        switch ((controls.MenuBar.Notifications)e.notification_id)
        {
          case controls.MenuBar.Notifications.kProjectLoaded:
            {

              items.Clear();
              string assets_folder = Project.directory_path + "\\assets";
              folder_watcher_.Path = assets_folder;
              file_watcher_.Path = assets_folder;

              DirectoryInfo info = new DirectoryInfo(assets_folder);
              root = new DirectoryItem(info.FullName, info.Name, null);
              root.editable = false;
              items.Add(root);
            }
            break;
        }
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

      #region INotifyPropertyChanged
      /**
       *@brief System.ComponentModel.INotifyPropertyChanged event
       *@see https://msdn.microsoft.com/en-us/library/system.componentmodel.inotifypropertychanged(v=vs.110).aspx 
       */
      public event PropertyChangedEventHandler PropertyChanged;

      /**
        *@brief implementation of the INotifyPropertyChanged interface 
        *@param[in] property_name (string) name of the propperty that changed
        */
      private void NotifyPropertyChanged(string property_name = "")
      {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(property_name));
      }
      #endregion

      private FileSystemWatcher folder_watcher_; //<! watcher to raise events when something changes in the workspace directory structure from outside the editor.
      private FileSystemWatcher file_watcher_;   //<! watcher to raise events when something happens with files inside one of the folders in the workspace.
      private ICommand new_folder_cmd_;          //<! Command for a contextmenu. this command will create a new folder.
      private ICommand delete_folder_cmd_;       //<! command for a contextmenu. this command will delete an existing folder inside a given parent folder.

      /**
       *@struct sulphur.editor.Workspace.FileQueueData 
       *@brief entry in the file queue to be processed when the editor regains keyboard focus 
       */
      private struct FileQueueData
      {
        public string path; //<! relative or absolute file to a path located in a workspace folder
        public Notifications file_operation; //<! notification that needs be send to subscribers about what happened to this file.
      }

      private List<FileQueueData> file_queue_; //<! file queue for file operations that happend in the workspace while the editor did not have keyboard focus

      /**
       *@brief implementation of the sulphur.editor.Workspace.delete_folder_cmd_ command 
       *@param[in] parameter (object) parameter passed to the command. this parameter is used to delete a folder
       *@remark the parameter passed must be a DirectoryItem  
       *@see sulphur.editor.DirectoryItem
       */
      private void DeleteFolder(object parameter)
      {
        DirectoryItem item = (DirectoryItem)parameter;
        if (item.parent == null)
        {
          return;
        }
        Directory.Delete(item.path, true);
        notify_subscribers_?.Invoke(this, new NotificationEventArgs(item.path, (int)Notifications.kFolderRemoved, id<Workspace>.type_id_));
      }

      /**
       *@brief implementation of the nsulphur.editor.Workspace.new_folder_cmd_ command 
       *@param[in] parameter (object) parameter passed to the command. parameter will be used to create a new folder
       *@remark the parameter passed must be a DirectoryItem  
       *@see sulphur.editor.DirectoryItem
       */
      private void NewFolder(object parameter)
      {
        DirectoryItem parent = (DirectoryItem)parameter;
        string path = parent.path + "\\New Folder";
        string new_path = path;
        uint i = 1;
        while (Directory.Exists(new_path) == true)
        {
          new_path = path + i.ToString();
          ++i;
        }
        Directory.CreateDirectory(new_path);
      }

      /**
       *@brief check if a file can opened for read / write access and is not used by another process.
       *@param[in] path (string) full or relative path to a file.
       */
      private bool FileArrived(string path)
      {
        try
        {
          using (var file = File.Open(path, FileMode.Open, FileAccess.Read, FileShare.Read))
          {
            return true;
          }
        }
        catch (IOException)
        {
          return false;
        }
      }

      /**
       *@brief function that handles incomming notifications 
       *@param[in] sender (object) the original sender of the notification
       *@param[in] e (sulphut.editor.NotificationEventArgs) event aruments passed when the event was fired
       *@remark current empty implementation since this system is not subscribed to any other system 
       */
      private void OnNotification(object sender, NotificationEventArgs e)
      {
        return;
      }

      /**
       *@brief function that handles the deleted event for the folder watcher 
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (FileSystemEventArgs) argumants passed when the event was fired
       */
      private void OnFolderWatcherDeleted(object sender, FileSystemEventArgs e)
      {
        DirectoryInfo info = new DirectoryInfo(e.FullPath);
        DirectoryItem item = FindItem(info.FullName, root);
        if (item == null || item.parent == null)
        {
          return; // removing assets folder
        }

        DirectoryItem parent = item.parent;
        App.Current.Dispatcher.Invoke(delegate { parent.items.Remove(item); });
      }

      /**
       *@brief function that handles the created event for the folder watcher. This function will cause the directory tree to refresh so the visual display is updated.
       * this function is fired when a folder is created in the workspace.
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (FileSystemEventArgs) argumants passed when the event was fired
       */
      private void OnFolderWatcherCreated(object sender, FileSystemEventArgs e)
      {
        DirectoryInfo info = new DirectoryInfo(e.FullPath);
        DirectoryInfo parent_info = info.Parent;
        if (parent_info.FullName == root.path)
        {
          root.Refresh();
        }
        else
        {
          DirectoryItem parent = FindItem(parent_info.FullName, root);
          if (parent == null)
          {
            return;
          }
          parent.Refresh();
        }
      }

      /**
       *@brief function that handles the renamed event for the folder watcher. This function will cause the directory tree to refresh so the visual display is updated.
       * this function is fired when a folder is renamed in the workspace.
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (RenamedEventArgs) arguments passed when the event was fired
       */
      private void OnFolderWatcherRenamed(object sender, RenamedEventArgs e)
      {
        DirectoryInfo info = new DirectoryInfo(e.FullPath);
        string oldpath = root.path + "\\" + e.OldName;

        DirectoryItem item = FindItem(oldpath, root);
        if (item != null)
        {
          item.name = info.Name;
          item.path = info.FullName;
          item.Refresh();
        }
      }

      /**
       *@brief function that handles file changes that happened when the editor did not have keyboard focus.
       * this function is only fired whe a file is changed to the workspace when the editor has lost focus.
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (FileSystemEventArgs) argumants passed when the event was fired
       */
      private void FileWatcherChanged(object sender, FileSystemEventArgs e)
      {
        if (File.Exists(e.FullPath) == false)
        {
          return;
        }

        FileQueueData data = new FileQueueData();
        data.path = e.FullPath;
        data.file_operation = Notifications.kFileChanged;

        for (int i = 0; i < file_queue_.Count; ++i)
        {
          if (file_queue_[i].path == e.FullPath)
          {
            file_queue_[i] = data;
            return;
          }
        }
        file_queue_.Add(data);
      }

      /**
      *@brief function that handles files created in the workspace that were created when the editor did not have keyboard focus
      *  this function is only fired whe a file is created to the workspace when the editor has lost focus.
      *  multiple sulphur.editor.Workspace.FileWatcherChanged events can be fired after this event.
      *@param[in] sender (object) original object that fired the event
      *@param[in] e (FileSystemEventArgs) argumants passed when the event was fired
      */
      private void FileWatcherCreated(object sender, FileSystemEventArgs e)
      {
        FileQueueData data = new FileQueueData();
        data.path = e.FullPath;
        data.file_operation = Notifications.kFileCreated;

        for (int i = 0; i < file_queue_.Count; ++i)
        {
          if (file_queue_[i].path == e.FullPath)
          {
            file_queue_[i] = data;
            return;
          }
        }
        file_queue_.Add(data);
      }

      /**
       *@brief function that handles files created in the workspace that were created when the editor did not have keyboard focus
       *  this function is only fired whe a file is deleted to the workspace when the editor has lost focus.
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (FileSystemEventArgs) argumants passed when the event was fired
       */
      private void FileWatcherDeleted(object sender, FileSystemEventArgs e)
      {
        FileQueueData data = new FileQueueData();
        data.path = e.FullPath;
        data.file_operation = Notifications.kFileRemoved;

        for (int i = 0; i < file_queue_.Count; ++i)
        {
          if (file_queue_[i].path == e.FullPath)
          {
            file_queue_[i] = data;
            return;
          }
        }
        file_queue_.Add(data);
      }

      /**
       *@brief function that handles files renamed in the workspace that were renamed when the editor did not have keyboard focus
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (RenamedEventArgs) argumants passed when the event was fired
       *@remark this function is currently unimplemented. 
       */
      private void OnFileWatcherRenamed(object sender, RenamedEventArgs e)
      {
      }

      /**
       *@brief does an upwards search from a given directory to find a directory specified by its path
       *@param[in] path (string) path of the dorictory to find
       *@param[in] item (suphur.editor.DirectoryItem)to start the search from
       *@return (sulphur.editor.DirectoryItem) item found
       *@remark if the specified folder is not found null is returned 
       */
      private DirectoryItem FindItem(string path, DirectoryItem input)
      {
        foreach (DirectoryItem item in input.items)
        {
          if (item.path == path)
          {
            return item;
          }
          else
          {
            DirectoryItem result = FindItem(path, item);
            if (result != null)
            {
              return result;
            }
          }
        }
        return null;
      }

      /**
       *@brief handle the GotKeyboardFocus event by handling the file_queue. The file queue is build up when the editor lost focus and file operations are done in the workspace.
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (KeyboardFocusChangedEventArgs) argumants passed when the event was fired 
       *@remark this event can be fired by any control in the editor but the queue is only handled if the old focus was outside the editor 
       *@remark for every file in the file queue an event is fired to notify subscribers
       *@see sulphur.editor.Workspace.Notifications
       *@see sulphur.editor.Workspace.FileWatcherChanged 
       *@see sulphur.editor.Workspace.FileWatcherCreated 
       *@see sulphur.editor.Workspace.OnFileWatcherRenamed 
       */
      private void OnMainWindowFocus(object sender, KeyboardFocusChangedEventArgs e)
      {
        if (e.OldFocus != null)
        {
          return;
        }
        file_watcher_.EnableRaisingEvents = false;

        while (file_queue_.Count > 0)
        {
          for (int i = 0; i < file_queue_.Count; ++i)
          {
            string file = file_queue_[i].path;
            OnNotification handler = notify_subscribers_;
            FileInfo info = new FileInfo(file);
            NotificationEventArgs args =
              new NotificationEventArgs(info,
                                        (uint)file_queue_[i].file_operation,
                                        id<Workspace>.type_id_);

            if (file_queue_[i].file_operation == Notifications.kFileRemoved)
            {
              handler?.Invoke(this, args);
              file_queue_.RemoveAt(i);
            }
            else if (file_queue_[i].file_operation != Notifications.kFileRemoved && FileArrived(file) == true)
            {
              handler?.Invoke(this, args);
              file_queue_.RemoveAt(i);
            }
          }
        }
        e.Handled = true;
      }

      /**
       *@brief handle the KeyboardFocusLost event by toggling the folder watcher to start raising events
       *@param[in] sender (object) original object that fired the event
       *@param[in] e (KeyboardFocusChangedEventArgs) argumants passed when the event was fired 
       *@remark this event can be fired by any control in the editor but the folder watcher is only turned on if the new focus is outside the editor  
       *@remark this function enables the sulphur.editor.Workspace.file_watcher 
       */
      private void OnMainWindowFocusLost(object sender, KeyboardFocusChangedEventArgs e)
      {
        if (e.NewFocus == null)
        {
          file_watcher_.EnableRaisingEvents = true;
          e.Handled = true;
        }
      }
    }
  }
}
