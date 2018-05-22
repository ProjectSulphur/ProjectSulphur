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
    public class DirectoryList : ObservableCollection<DirectoryItem>
    {
      public DirectoryList() : base() { }
    }

    public class DirectoryItem : INotifyPropertyChanged
    {
      /*public members*/
      public event PropertyChangedEventHandler PropertyChanged;

      public DirectoryItem parent { get; private set; }

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

        items = new DirectoryList();
        DirectoryInfo[] sub_dirs = info.GetDirectories();

        foreach (DirectoryInfo dir in sub_dirs)
        {
          DirectoryItem item = new DirectoryItem(dir.FullName, dir.Name, this);
          items.Add(item);
        }
      }

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

      /*private members*/
      private string path_value_;

      private string name_value_;

      private DirectoryList items_value_;

      private void NotifyPropertyChanged(string propertyName = "")
      {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
      }
    }

    public class Workspace : INotifyPropertyChanged, ISubscribeable
    {
      /*public members*/
      public enum Notifications
      {
        kFolderCreated,
        kFolderRemoved,
        kFolderRenames,
        kFileCreated,
        kFileRemoved,
        kFileRenamed,
        kFileChanged
      }

      public DirectoryItem root { get; set; }
      public DirectoryList items { get; set; } //the directory structure relative to the assets folder

      public ICommand delete_folder_cmd
      {
        get
        {
          return delete_folder_cmd_ ?? 
            (delete_folder_cmd_ = new utils.CommandHandler((object param) => DeleteFolder(param),
                                                           true));
        }
      }

      public ICommand new_folder_cmd
      {
        get
        {
          return new_folder_cmd_ ?? 
            (new_folder_cmd_ = new utils.CommandHandler((object param) => NewFolder(param), true));
        }
      }

      public Workspace()
      {
        file_queue_ = new List<FileQueueData>();
        Application.Current.MainWindow.GotKeyboardFocus += OnMainWindowFocus;
        Application.Current.MainWindow.LostKeyboardFocus += OnMainWindowFocusLost;
        DirectoryInfo info = new DirectoryInfo(".\\assets");
        items = new DirectoryList();
        root = new DirectoryItem(info.FullName, info.Name, null);

        items.Add(root);

        folder_watcher_ = new FileSystemWatcher();
        folder_watcher_.Path = ".\\assets";
        folder_watcher_.IncludeSubdirectories = true;
        folder_watcher_.EnableRaisingEvents = true;
        folder_watcher_.NotifyFilter = NotifyFilters.DirectoryName;
        folder_watcher_.Renamed += OnFolderWatcherRenamed;
        folder_watcher_.Created += OnFolderWatcherCreated;
        folder_watcher_.Deleted += OnFolderWatcherDeleted;

        file_watcher_ = new FileSystemWatcher();
        file_watcher_.Path = ".\\assets";
        file_watcher_.IncludeSubdirectories = true;
        file_watcher_.EnableRaisingEvents = false;
        file_watcher_.Filter = "*.*";
        file_watcher_.NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite;
        file_watcher_.Renamed += OnFileWatcherRenamed;
        file_watcher_.Changed += FileWatcherChanged;
        file_watcher_.Deleted += FileWatcherDeleted;
      }

      public void OnRemoveFolder(object sender, RoutedEventArgs e)
      {
        return;
      }

      #region ISubscribable
      public Subscription[] GetSubscriptions()
      {
        return null;
      }

      private event OnNotification notify_subscribers_;

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
      public event PropertyChangedEventHandler PropertyChanged;

      private void NotifyPropertyChanged(string propertyName = "")
      {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
      }
      #endregion

      /*private members*/
      private FileSystemWatcher folder_watcher_; //watcher to raise events regarding firectory changes
      private FileSystemWatcher file_watcher_;   // watcher to raise events regarding file changes
      private ICommand new_folder_cmd_;
      private ICommand delete_folder_cmd_;

      private struct FileQueueData{
        public string path;
        public Notifications file_operation;
      }

      private List<FileQueueData> file_queue_;

      private void DeleteFolder(object parameter)
      {
        DirectoryItem item = (DirectoryItem)parameter;
        if (item.parent == null)
        {
          return; // trying to delete root is not allowed
        }

        Directory.Delete(item.path, true);
      }

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

      private void OnNotification(object sender, NotificationEventArgs e)
      {
        return;
      }

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
       handles overwrite and create events
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

      private void FileWatcherDeleted(object sender, FileSystemEventArgs e)
      {
        FileQueueData data = new FileQueueData();
        data.path = e.FullPath;
        data.file_operation = Notifications.kFileRemoved;

        for( int i = 0; i < file_queue_.Count; ++i)
        {
          if (file_queue_[i].path == e.FullPath)
          {
            file_queue_[i] = data;
            return;
          }
        }
        file_queue_.Add(data);
      }

      private void OnFileWatcherRenamed(object sender, RenamedEventArgs e)
      {
        // OnNotification handler = notify_subscribers_;
        // handler?.Invoke();
      }

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

      private void OnMainWindowFocus(object sender, KeyboardFocusChangedEventArgs e)
      {
        if(e.OldFocus != null)
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
            else if(file_queue_[i].file_operation != Notifications.kFileRemoved && FileArrived(file) == true)
            {
              handler?.Invoke(this, args);
              file_queue_.RemoveAt(i);
            }
          }
        }
        e.Handled = true;
      }

      private void OnMainWindowFocusLost(object sender, KeyboardFocusChangedEventArgs e)
      {
        if(e.NewFocus == null)
        {
          file_watcher_.EnableRaisingEvents = true;
          e.Handled = true;
        }
      }
    }
  }
}
