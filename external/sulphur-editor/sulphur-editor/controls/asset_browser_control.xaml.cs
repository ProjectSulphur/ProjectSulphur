using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Media;

namespace sulphur
{
  namespace editor
  {
    namespace controls
    {
      public partial class AssetBrowserControl : UserControl, ISubscribeable, INotifyPropertyChanged
      {
        /*public members*/
        public List<AssetCacheData> asset_list
        {
          get
          {
            return asset_list_value;
          }
          set
          {
            if (value != asset_list_value)
            {
              asset_list_value = value;
              NotifyPropertyChanged();
            }
          }
        }
        public event PropertyChangedEventHandler PropertyChanged;

        public AssetBrowserControl()
        {
          InitializeComponent();
          folder_explorer.SelectedItemChanged += FolderExplorerSelectionChanged;
          database = Application.Current.Resources["asset_database"] as AssetDatabase;
        }

        #region ISubscribable
        public Subscription[] GetSubscriptions()
        {
          Subscription[] subscritions = new Subscription[1];
          subscritions[0] = new Subscription(ID.type_id<AssetDatabase>.value(), OnNotification);
          return subscritions;
        }

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

        /*private members*/   
        private event OnNotification notify_subscribers_;
        private DirectoryItem curr_directory;
        private List<AssetCacheData> asset_list_value;
        private AssetDatabase database { get; set; }

        private void NotifyPropertyChanged(string propertyName = "")
        {
          PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void FolderExplorerSelectionChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
          curr_directory = e.NewValue as DirectoryItem;
          if (curr_directory != null)
          {
            asset_list = database.GetAssetsInFolder(curr_directory.path);
          }
          else
          {
            asset_list = null;
          }
        }

        private void OnNotification(object sender, NotificationEventArgs e)
        {
          if (e.type_id == ID.type_id<AssetDatabase>.value())
          {
            if (curr_directory != null)
            {
              asset_list = database.GetAssetsInFolder(curr_directory.path);
            }
          }
        }

        private void OnContentDrop(object sender, DragEventArgs e)
        {
          if (e.Data.GetDataPresent(DataFormats.FileDrop) && curr_directory != null)
          {
            string[] file_names = (string[])e.Data.GetData(DataFormats.FileDrop);
            if ((e.Effects & DragDropEffects.Copy) != 0)
            {
              foreach (string file in file_names)
              {
                string name = file.Substring(file.LastIndexOf("\\"));
                File.Copy(file, curr_directory.path + name, true);
                database.Import(curr_directory.path + name);
              }
            }
          }
        }

        private void OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
          database.instantiate_cmd.Execute(content.SelectedItem);
        }

        private void OnContentKeyDown(object sender, KeyEventArgs e)
        {
          switch (e.Key)
          {
            case Key.Delete:
              AssetCacheData selected = (AssetCacheData)content.SelectedItem;
              database.DeleteAsset(selected);
              break;
          }
        }

        private void TreeViewItem_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
          TreeViewItem treeViewItem = GetTreeviewItem(e.OriginalSource as DependencyObject); // find the first treeviewItem

          if (treeViewItem != null)
          {
            treeViewItem.Focus();
            e.Handled = true;
          }
        }

        private void ListBoxItem_PreviewMouseRightButtonDown(object sender, RoutedEventArgs e)
        {
          ListBoxItem ListBoxItem = sender as ListBoxItem; // find the first treeviewItem

          if (ListBoxItem != null)
          {
            ListBoxItem.IsSelected = true;
            e.Handled = true;
          }
        }

        private TreeViewItem GetTreeviewItem(DependencyObject source)
        {
          while (source != null && !(source is TreeViewItem))
          {
            source = VisualTreeHelper.GetParent(source);
          }
          return source as TreeViewItem;
        }
      }
    }
  }
}
