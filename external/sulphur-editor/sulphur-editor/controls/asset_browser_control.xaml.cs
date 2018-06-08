using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Input;
using System.Windows.Documents;
using sulphur.editor.data;
using sulphur.editor.controls.inspector;

namespace sulphur
{
  namespace editor
  {
    namespace controls
    {
      /**
       *@class sulphur.editor.controls.AssetBrowserControl : UserControl, sulphur.editor.ISubscribeable, INotifyPropertyChanged 
       *@brief the asset browser xaml code behind for handling control events and response to notifications 
       *@author Stan Pepels
       */
      public partial class AssetBrowserControl : UserControl, ISubscribeable
      {
        private InspectorControl inspector_ = null;
        private UniformBuffer ub = null; // Temp variable

        /**
         * @brief Notifications ids for messages send to other systems.
         */
        public enum Notifications
        {
          kFileDrop, //!< Indicates that a file has been dropped on this control.
          kAssetRemoved, //!< Indicated that an has been removed.
          kAssetInstantiated //!< Indicates that an object has been instatiated using a certain asset.
        }

        /**
         *@brief constructor 
         */
        public AssetBrowserControl()
        {
          InitializeComponent();
        }

        public void SetInspector(InspectorControl inspector)
        {
          inspector_ = inspector;
        }

        #region ISubscribable
        /**
          *@brief get the subscritions to systems we want recieve notifications from
          *@return (sulphur.editor.Subscription[]) arrray of subscriptions to systems  
          *@remark this system is subscribed to the asset database
          */
        public Subscription[] GetSubscriptions()
        {
          return null;
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
        }//<! event fired for notifying subscribed system that something important happend
        #endregion

        /**
         *@brief actual event handler used to fire the notification event from the sulphur.editor.ISubscribeable interface
         */
        private event OnNotification notify_subscribers_;

        /**
         *@brief handle the OnContentDrop event 
         *@param[in] sender (object) the original sender of the event
         *@param[in] e (DragEventArgs) event arguments passed when the event was fired
         *@remark Sends an FileDrop notification for each file in the drop operation.
         *@see Notifications 
         */
        private void OnContentDrop(object sender, DragEventArgs e)
        {
          DirectoryItem directory = folder_explorer.SelectedItem as DirectoryItem;
          if (e.Data.GetDataPresent(DataFormats.FileDrop) && directory != null)
          {
            string[] file_names = (string[])e.Data.GetData(DataFormats.FileDrop);
            if ((e.Effects & DragDropEffects.Copy) != 0)
            {
              foreach (string file in file_names)
              {
                string name = file.Substring(file.LastIndexOf("\\"));
                App.Current.MainWindow.Focus();

                File.Copy(file, directory.path + name, true);
                NotificationEventArgs args = new NotificationEventArgs(directory.path + name, (int)Notifications.kFileDrop, id<AssetBrowserControl>.type_id_);
                notify_subscribers_?.Invoke(this,args);
              }
            }
          }
        }


        /**
         *@brief handle the OnMouseDoubleClick event 
         *@param[in] sender (object) the original sender of the event
         *@param[in] e (MouseButtonEventArgs) event arguments passed when the event was fired
         *@remark Sends an Asset instantiated notification 
         *@see Notifications
         */
        private void OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
          NotificationEventArgs args = new NotificationEventArgs(content.SelectedItem, (int)Notifications.kAssetInstantiated, id<AssetBrowserControl>.type_id_);
          notify_subscribers_?.Invoke(this, args);
        }
        
        /**
        *@brief handle the OnContentKeyDown event 
        *@param[in] sender (object) the original sender of the event
        *@param[in] e (KeyEventArgs) event arguments passed when the event was fired
        *@remark Sends an Asset removed notification
        *@see Notifications 
        */
        private void OnContentKeyDown(object sender, KeyEventArgs e)
        {
          switch (e.Key)
          {
            case Key.Delete:
              NotificationEventArgs args = new NotificationEventArgs(content.SelectedItem, (int)Notifications.kAssetRemoved, id<AssetBrowserControl>.type_id_);
              notify_subscribers_?.Invoke(this, args);
              break;
          }
        }

        /**
        *@brief handle the PreviewRightMouseButtonDown event on the treeview (folder tree)
        *@param[in] sender (object) the original sender of the event
        *@param[in] e (MouseEventArgs) event arguments passed when the event was fired
        */
        private void TreeViewItem_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
          TreeViewItem item = Utils.GetTreeViewItemInParents(e.OriginalSource as DependencyObject);

          if (item != null)
          {
            item.Focus();
            e.Handled = true;
          }
        }

        /**
        *@brief handle the PreviewMouseRightButtonDown event on the Listbox (asset list)
        *@param[in] sender (object) the original sender of the event
        *@param[in] e (MouseEventArgs) event arguments passed when the event was fired
        */
        private void ListBoxItem_PreviewMouseRightButtonDown(object sender, RoutedEventArgs e)
        {
          ListBoxItem ListBoxItem = sender as ListBoxItem; // find the first treeviewItem

          if (ListBoxItem != null)
          {
            ListBoxItem.IsSelected = true;
            e.Handled = true;
          }
        }

        private void content_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
          AssetCacheData data = (AssetCacheData)e.AddedItems[0];

          switch (data.type)
          {
            case AssetType.kMaterial:
              ReadMaterialData(data.path);
              break;
          }
        }

        private ShaderData ReadShaderData(string relative_path)
        {
          BinaryReader reader = new BinaryReader(Project.directory_path + "\\" + relative_path);
          if (!reader.is_ok)
          {
            return null;
          }

          ShaderData shaderData = new ShaderData();
          shaderData.Read(reader);
          return shaderData;
        }

        private void StoreMaterialData(string relative_path, MaterialData material_data)
        {
          // BinaryWriter writer = new BinaryWriter(Project.directory_path + "\\" + relative_path);
          // if (!writer.is_ok)
          // {
          //   return;
          // }

          //material_data.Write(writer);
        }

        private void ReadMaterialData(string relative_path)
        {
          BinaryReader reader = new BinaryReader(Project.directory_path + "\\" + relative_path);
          if (!reader.is_ok)
          {
            return;
          }

          MaterialData material_data = new MaterialData();
          material_data.Read(reader);

          AssetDatabase asset_database = (AssetDatabase)App.Current.Resources["asset_database"];
          
          AssetCacheData pixel_shader_cache = asset_database.Find(material_data.pixel_shader_id, AssetType.kShader);
          ShaderData pixelShader = ReadShaderData(pixel_shader_cache.path);

          ub = new UniformBuffer(pixelShader, material_data.uniform_buffers[0].data);

          inspector_.ClearInspectors();

          foreach (var pair in ub.name_map_)
          {
            ShaderVariable var = ub.data_map_[pair.Value];
            InspectorTypes type = InspectorTypes.kNumber;

            switch (var.type_)
            {
              case ShaderVarType.kBool:
                type = InspectorTypes.kCheckbox;
                break;
              case ShaderVarType.kFloat:
              case ShaderVarType.kDouble:
                type = InspectorTypes.kSlider;
                break;
              case ShaderVarType.kUint:
              case ShaderVarType.kUint8:
              case ShaderVarType.kInt:
                type = InspectorTypes.kNumber;
                break;
              case ShaderVarType.kMat3:
                type = InspectorTypes.kMatrix3x3;
                break;
              case ShaderVarType.kMat4:
                type = InspectorTypes.kMatrix4x4;
                break;
              case ShaderVarType.kVec2:
                type = InspectorTypes.kVector2;
                break;
              case ShaderVarType.kVec3:
                type = InspectorTypes.kVector3;
                break;
              case ShaderVarType.kVec4:
                type = InspectorTypes.kVector4;
                break;
            }

            inspector_.AddInspector(pair.Key, type,
                  new VarRef(() => ub.data_map_[pair.Value].data_,
                    val => {
                      ub.data_map_[pair.Value].Set(val);
                      material_data.uniform_buffers[0].data = ub.ConstructDataBuffer();
                      StoreMaterialData(relative_path, material_data);
                    }));
          }
        }
      }
    }
  }
}
