using System.IO;
using System;
using System.Collections.Generic;
using System.Windows.Input;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace sulphur
{
  namespace editor
  {
    /**
     * @class sulphur.editor.AssetList : ObservableCollection<AssetCacheData>
     * @brief Assetlist that can be bound to by controls.
     * @author Stan Pepels
     */
    public class AssetList : ObservableCollection<AssetCacheData>
    {
      /**
       * @brief Find an asset by its id.
       * @param[in] id (UInt64) id of the asset to find.
       * @return (sulphur.editor.AssetCacheData) The data stored in the cache of the asset of the given id. If not found an invalid struct is returned.
       * @see sulphur.editor.AssetCacheData
       */
      public AssetCacheData Find(UInt64 id)
      {
        foreach (AssetCacheData entry in this)
        {
          if (entry.id == id)
          {
            return entry;
          }
        }
        return new AssetCacheData();
      }

      /**
        * @brief Find the index of an asset given an asset id.
        * @param[in] id (UInt64) id of the asset to get the index for.
        * @return (int) index of the asset data in the list.
        */
      public int FindIndex(UInt64 id)
      {
        foreach (AssetCacheData entry in this)
        {
          if (entry.id == id)
          {
            return IndexOf(entry);
          }
        }
        return -1;
      }
    };

    /**
     *@brief Enum representing the different type of assets supported by the editor
     */
    public enum AssetType : uint
    {
      kModel, //!< Model asset (*.obj, *.fbx, *.gltf)
      kTexture, //!< Texture asset (*.png, *.jpeg, *.bmp)
      kShader, //!< Shader asset (*.vert, *.comp, *.geom, *.hull, *.domap, *.pixe)
      kMaterial, //!< Material asset
      kMesh, //!< Mesh asset constructed from a model asset.
      kAudio, //!< Audio asset (*.?)
      kAnimation, //!< Animation asset constructed from an animation asset.
      kScript,//!< Script asset (*.lua).
      kSkeleton, //!< Skeleton asset constructed from an animation asset.
      kNumAssets, //! Number of supported asset types
      kUnknown //!< Unsuported asset type.
    }


    /**
     *@struct sulphur.editor.AssetCacheData
     *@brief structure representing an asset in the asset cache. this is the view model used by the asset browser  
     *@author Stan Pepels 
     */
    public struct AssetCacheData
    {
      /**
       *@brief id of the asset. The asset Id is created with a hash function from the name of the raw asset the packaged asset was created from.
       */
      public UInt64 id { get; set; }

      /**
       *@brief name of the asset as it is stored in a package. Every asset gets a name associated with it. This name is usually the same as the 
       * raw asset but in case of duplicate name a number is appended to it. 
       * for example : test.fbx --> test1.smo
       */
      public string name { get; set; }

      /**
       *@brief path to the raw asset the asset was created from. 
       * The raw asset path is always a path from the asset folder.
       */
      public string origin { get; set; }

      /**
       *@brief path to the package this asset is stored in.
       */
      public string path { get; set; }

      /**
       * @brief the type of asset.
       * @see sulphur.editor.AssetType
       */
      public AssetType type { get; set; }

      /**
       * @brief Value indicating if this data is valid.
       */
      public bool is_valid { get; set; }
    }

    /**
     *@class sulphur.editor.AssetDatabase : sulphur.editor.ISubscribeable 
     *@brief class that handles importing of assets and stores them on a per folder basis. 
     *@author Stan Pepels
     *@todo: split class into processor and database so we can remove references to it from the asset browser using data binding
     */
    public class AssetDatabase : ISubscribeable, INotifyPropertyChanged
    {
      /**
       * @brief data (Dictionary<string, List<AssetCacheData>>) dictionary containing imported asset based on the origin location.
       * @see sulphur.editor.AssetCacheData
       */
      public Dictionary<string, AssetList> data { get; set; }

      /**
       *@brief constructor. This sets up the background worker used for loading assets asynch.
       * This is also where the cache files are initialy read and the data structure is initialized
       */
      public AssetDatabase()
      {
        worker_ = new BackgroundWorker();
        worker_.DoWork += RunWorker;
        worker_.RunWorkerCompleted += OnWorkerRunCompleted;
        asset_queue_ = new Queue<string>();
        data = new Dictionary<string, AssetList>();
        BuildDatabase(Project.directory_path);
        data.Add(Path.GetFullPath(".\\assets"), new AssetList());
      }

      /**
       *@brief process a single asset. When an asset has been imported an message is send to the attached engine. 
       *@param[in] path (string) path to a raw asset to import into the current project  
       * @see sulphur.editor.native.networking.NetworkMessages
       * @remark This does not automatically update the database with the imported asset. But instead will add the imported asset id to the respective cache changelist.
       * @see UpdateDatabase
       * @see change_list
       */
      public void ProcessAsset(string path)
      {
        UInt64 id = 0;
        native.bool_ success = false;
        AssetType type = AssetType.kUnknown;
        if (File.Exists(path) == false)
        {
          return;
        }

        string extension = GetExtension(path);
        extension = extension.ToLower();

        if (extension == ".obj" ||
            extension == ".fbx" ||
            extension == ".gltf")
        {
          success = native.AssetProcessor.ImportModel(path, true, "", "");
          if (success)
          {
            type = AssetType.kModel;
          }
        }

        else if (extension == ".someAudioExtension") // @todo: correct the file type
        {
          success = native.AssetProcessor.ImportAudio(path, ref id);
          if (success)
          {
            type = AssetType.kAudio;
          }
        }

        else if (extension == ".lua")
        {
          success = native.AssetProcessor.ImportScript(path, ref id);
          if (success)
          {
            type = AssetType.kScript;
          }
        }

        else if (extension == ".png" ||
                 extension == ".jpeg" ||
                 extension == ".bmp" ||
                 extension == ".tga" ||
                 extension == ".jpg")
        {
          success = native.AssetProcessor.ImportTexture(path, ref id);
          if (success)
          {
            type = AssetType.kTexture;
          }
        }

        else if (extension == ".vert" ||
                 extension == ".comp" ||
                 extension == ".doma" ||
                 extension == ".hull" ||
                 extension == ".geom" ||
                 extension == ".pixe")
        {
          success = native.AssetProcessor.ImportShader(path, ref id);
          if (success)
          {
            type = AssetType.kShader;
          }
        }

        if (success == false)
        {
          Console.WriteLine("Unable to import asset {0}", path);
        }
        else
        {
          UpdateDatabase(id, Operation.kAdd, type);
          Console.WriteLine("Successfully import asset {0}", path);
        }
      }

      /**
       *@brief remove an asset from the asset cache. when the asset is removed succesfully a message is send to the attached engine.
       *@param[in] asset (sulphur.editor.AssetCacheData) asset to remove from the cache
       *@remark this will not delete the raw asset the packed asset originated from.
       *@see sulphur.editor.native.networking.NetworkMessages
       */
      public void DeleteAsset(AssetCacheData asset)
      {
        string folderpath = asset.origin.Substring(0, asset.origin.LastIndexOf("\\"));
        native.bool_ success = false;
        if (data.ContainsKey(folderpath) == false)
        {
          return;
        }

        foreach (AssetCacheData entry in data[folderpath])
        {
          if (entry.origin == asset.path)
          {
            asset = entry;
          }
        }

        switch (asset.type)
        {
          case AssetType.kAnimation:
            success = native.AssetProcessor.DeleteAnimation(asset.id);
            break;
          case AssetType.kAudio:
            success = native.AssetProcessor.DeleteAudio(asset.id);
            break;
          case AssetType.kMaterial:
            success = native.AssetProcessor.DeleteMaterial(asset.id);
            break;
          case AssetType.kMesh:
            success = native.AssetProcessor.DeleteMesh(asset.id);
            break;
          case AssetType.kModel:
            success = native.AssetProcessor.DeleteModel(asset.id);
            break;
          case AssetType.kScript:
            success = native.AssetProcessor.DeleteScript(asset.id);
            break;
          case AssetType.kShader:
            success = native.AssetProcessor.DeleteShader(asset.id);
            break;
          case AssetType.kSkeleton:
            success = native.AssetProcessor.DeleteSkeleton(asset.id);
            break;
          case AssetType.kTexture:
            success = native.AssetProcessor.DeleteTexture(asset.id);
            break;

        }

        if (success == true)
        {
          UpdateDatabase(asset.id, Operation.kRemove, asset.type);
        }
      }

      /**
       *@brief command to remove an asset from the current cache. this command is used for deleting assets via a context menu 
       *@see sulphur.editor.AssetDatabase.DeleteAsset 
       *@todo: move command to the browser control 
       */
      public ICommand remove_asset_cmd
      {
        get
        {
          return remove_asset_cmd_ ??
            (remove_asset_cmd_ = new utils.CommandHandler((object param) => DeleteAsset((AssetCacheData)param),
                                                          true));
        }
      }

      /**
       *@brief command to instantiate an asset. this command is used for instantiating assets via a context menu
       * @see sulphur.editor.AssteDatabase.Instantiate
       *@todo: move command to the browser control  
       */
      public ICommand instantiate_cmd
      {
        get
        {
          return instantiate_cmd_ ??
            (instantiate_cmd_ = new utils.CommandHandler((object param) => Instantiate((AssetCacheData)param),
                                                         true));
        }
      }

      /**
       *@brief import an asset using the background worker.
       *@param[in] path (string) path to a raw asset to import.
       *@see sulphur.editor.AssetType 
       */
      public void Import(string path)
      {
        asset_queue_.Enqueue(path);
        if (worker_.IsBusy == false)
        {
          worker_.RunWorkerAsync();
        }
      }

      #region INotifyPropertyChanged
      public event PropertyChangedEventHandler PropertyChanged; //!< Event fired when the asset list has changed.

      /**
       * @brief Invokes the PropertyChanged event.
       * @param[in] name (string) name of the property that changed.
       */
      protected void OnPropertyChanged(string name)
      {
        PropertyChangedEventHandler handler = PropertyChanged;
        handler?.Invoke(this, new PropertyChangedEventArgs(name));
      }
      #endregion

      #region ISubscribeable
      /**
      *@brief sulphur.editor.ISubscribeable.notify_event implementation 
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

      /**
       *@brief sulphur.editor.ISubscribeable.GetSubscriptions function implementation.
       *@remark this system is subscribed to the sulphur.editor.Workspace
       *@see sulphur.editor.Workspace.Notifications
       */
      public Subscription[] GetSubscriptions()
      {
        Subscription[] subscriptions = new Subscription[3];
        subscriptions[0] = new Subscription(id<Workspace>.type_id_, HandleWorkspaceNotification);
        subscriptions[1] = new Subscription(id<controls.MenuBar>.type_id_, HandleMenuNotification);
        subscriptions[2] = new Subscription(id<controls.AssetBrowserControl>.type_id_, HandleBrowserNotification);
        return subscriptions;
      }

      /**
       *@brief event fired when subscribers need to be notified  
       */
      private event OnNotification notify_subscribers_;
      #endregion

      /**
       *@class sulphur.editor.AssetDatabase.PackagePtr :  sulphur.editor.IBinarySerializable
       *@brief struct containing data from an entry read from a cache file.
       *@remark this is an exact copy of the PackagePtr struct in the engine
       *@see sulphur::foundation::PackagePtr
       *@author Stan Pepels  
       */
      private class PackagePtr : IBinarySerializable
      {
        /**
         *@brief sulphur::foundation::PackagePtr::origin
         */
        public string origin { get; private set; }

        /**
          *@brief sulphur::foundation::PackagePtr::path
         */
        public string path { get; private set; }

        /**
         *@brief implementation of the sulphur.editor.IBinarySerializable interface. this will read an entry from a binary file and fill this struct with its contents.
         *@param[in] reader (sulphur.editor.BinaryReader) reader containing the cache data.
         */
        public void Read(BinaryReader reader)
        {
          origin = reader.ReadString();
          path = reader.ReadString();
        }
      }

      /**
       * @brief List of operation that can be performed on the database.
       */
      private enum Operation
      {
        kAdd, //!< Specifies that an asset should be added.
        kRemove, //!< Specifies that an asset should be removed.
      }

      private BackgroundWorker worker_; //<! Background worker used to process assets asynch.
      private Queue<string> asset_queue_; //<! Asset queue processed by the background worker. Assets are added to the queue whe Import is called.
      private ICommand instantiate_cmd_; //<! Instantiates an object with the component that represents the currently selected asset in the assetlist.
      private ICommand remove_asset_cmd_; //<! Removes an asset from the database.

      /**
      *@brief handle the worker completed event by updating the asset database with all the newly processed assets.
      *@param[in] sender (object) original object that invoked the event
      *@param[in] e (RunWorkerCompletedEventArgs) arguments passed with the event
      *@remark If the queue is not empty after the asset has been imported the worker will process the next asset. The database is not updated untill the entire queue is empty.
      */
      private void OnWorkerRunCompleted(object sender, RunWorkerCompletedEventArgs e)
      {
        if (asset_queue_.Count != 0)
        {
          worker_.RunWorkerAsync();
        }
      }

      /**
       *@brief tells the worker to start processing the asset thats on the top of the queue 
       *@param[in] sender (object) original object that invoked the event
       *@param[in] e (DoWorkEventArgs) arguments passed when the event was invoked
       */
      private void RunWorker(object sender, DoWorkEventArgs e)
      {
        ProcessAsset(asset_queue_.Dequeue());
      }

      /**
       *@brief handle incoming notifications from the workspace 
       *@param[in] sender (object) original object that invoked the event
       *@param[in] e (NotificationEventArgs) arguments passed with the event
       * @see sulphur.editor.Workspace.Notifications
       */
      private void HandleWorkspaceNotification(object sender, NotificationEventArgs e)
      {
        FileInfo info = e.notification_data as FileInfo;
        switch ((Workspace.Notifications)e.notification_id)
        {
          case Workspace.Notifications.kFileCreated:
            Import(info.FullName);
            break;
          case Workspace.Notifications.kFileChanged:
            Import(info.FullName);
            break;
          case Workspace.Notifications.kFileRemoved:
            if (data.ContainsKey(info.Directory.FullName) == false)
            {
              break;
            }

            for (int i = 0; i < data[info.Directory.FullName].Count; ++i)
            {
              if (data[info.Directory.FullName][i].origin == info.FullName)
              {
                DeleteAsset(data[info.Directory.FullName][i]);
                break;
              }
            }
            break;
          case Workspace.Notifications.kFileRenamed:
            break;
          case Workspace.Notifications.kFolderRemoved:
            {
              string key = e.notification_data as string;
              key = key.ToLower();
              if (data.ContainsKey(key) == false)
              {
                break;
              }
              
              while(data[key].Count != 0)
              {
                DeleteAsset(data[key][0]);
              }
              data.Remove(key);
            }
            break;
        }
      }

      /**
       *@brief handle incoming notifications from the workspace 
       *@param[in] sender (object) original object that invoked the event
       *@param[in] e (NotificationEventArgs) arguments passed with the event
       * @see sulphur.editor.constrols.MenuBar.Notifications
       */
      private void HandleMenuNotification(object sender, NotificationEventArgs e)
      {
        switch ((controls.MenuBar.Notifications)e.notification_id)
        {
          case controls.MenuBar.Notifications.kProjectLoaded:
            BuildDatabase(Project.directory_path);
            break;
        }
      }

      /**
       *@brief handle incoming notifications from the workspace 
       *@param[in] sender (object) original object that invoked the event
       *@param[in] e (NotificationEventArgs) arguments passed with the event
       * @see sulphur.editor.controls.AssetBrowser.Notifications
       */
      private void HandleBrowserNotification(object sender, NotificationEventArgs e)
      {
        switch ((controls.AssetBrowserControl.Notifications)e.notification_id)
        {
          case controls.AssetBrowserControl.Notifications.kAssetInstantiated:
            instantiate_cmd.Execute(e.notification_data);
            break;
          case controls.AssetBrowserControl.Notifications.kFileDrop:
            Import((string)e.notification_data);
            break;
          case controls.AssetBrowserControl.Notifications.kAssetRemoved:
            DeleteAsset((AssetCacheData)e.notification_data);
            break;
        }
      }

      /**
       *@brief get the file extension from a file 
       *@param[in] path (string) path to a file
       *@return (string) extension retrieved from the path
       *@remark the returned extension always starts with a '.'   
       */
      private string GetExtension(string path)
      {
        int index = path.LastIndexOf(".");
        return path.Substring(index);
      }

      /**
       *@brief Builds the asset database by reading all available caches.
       *@param[in] path (string) Path to the folder the caches are exported to. 
       *@remark Sends an notification to the subscribers. Note that this notification has no data attached.
       */
      private void BuildDatabase(string path = ".")
      {
        data.Clear();
        for (int i = 0; i < (int)AssetType.kNumAssets; ++i)
        {
          ProcessCacheFileData(ReadCache((AssetType)i));
        }

        OnNotification handle = notify_subscribers_;
        NotificationEventArgs args = new NotificationEventArgs(null, 0, id<AssetDatabase>.type_id_);
        handle?.Invoke(this, args);

        native.Networking.SNetSendData((uint)native.NetworkMessages.kCacheChanged, null, 0);
      }

      /**
       * @brief Reads the cache of a certain asset type.
       * @param[in] type (sulphur.editor.AssetType) Type of asset to read the cache from.
       * @return (Dictionary<utils.UInt64_s, PackagePtr>) The data read from the cache.
       */
      private Dictionary<utils.UInt64_s, PackagePtr> ReadCache(AssetType type)
      {
        string cache_name = native.AssetProcessor.GetAssetPackageName(type);
        if (cache_name == null)
        {
          return null;
        }
        cache_name += ".cache";

        Dictionary<utils.UInt64_s, PackagePtr> result;
        BinaryReader reader = new BinaryReader(Project.directory_path + "\\" + cache_name);
        if (reader.is_ok == true)
        {
          reader.ReadDictionary(out result);
          return result;
        }
        return null;
      }

      /**
       * @brief Updates the database.
       * @param[in] asset_id (UInt64) The asset id used to update.
       * @param[in] operation (sulphur.editor.AssetDatabase.Operation) The operation used to update the database. 
       * @param[in] type (sulphur.editor.AssetType) Type of the asset.
       */
      private void UpdateDatabase(UInt64 asset_id, Operation operation, AssetType type)
      {
        switch (operation)
        {
          case Operation.kAdd:
            ProcessCacheFileData(asset_id, ReadCache(type)[asset_id]);
            break;
          case Operation.kRemove:
            foreach (KeyValuePair<string, AssetList> entry in data)
            {
              for (int i = 0; i < entry.Value.Count; ++i)
              {
                if (entry.Value[i].id == asset_id)
                {
                  entry.Value.RemoveAt(i);
                  break;
                }
              }
            }
            break;
        }
        native.Networking.SNetSendData((uint)native.NetworkMessages.kCacheChanged, null, 0);
      }

      /**
       * @brief Processes a single entry in an asset cache;
       * @param[in] id (UInt64) id of the asset.
       * @param[in] ptr (sulphur.editor.AssetDatabase.PackagePtr) Package pointer read from the cache.
       */
      private void ProcessCacheFileData(UInt64 id, PackagePtr ptr)
      {
        AssetCacheData cache_data = new AssetCacheData();
        DirectoryInfo info = new DirectoryInfo(ptr.origin);
        int slash = ptr.path.LastIndexOf("/") + 1;
        int dot = ptr.path.LastIndexOf(".");

        cache_data.id = id;
        cache_data.path = ptr.path;
        cache_data.origin = info.FullName;
        cache_data.name = ptr.path.Substring(slash,
                                              dot - slash);
        cache_data.type = GetAssetType(ptr.path);
        cache_data.is_valid = true;
        string original_folder = info.FullName.Substring(0, info.FullName.LastIndexOf("\\"));
        if (data.ContainsKey(original_folder) == false)
        {
          AssetList new_list = new AssetList();
          new_list.CollectionChanged += OnAssetListChanged;
          data.Add(original_folder, new_list);
        }

        if (data[original_folder].Find(id).is_valid == false)
        {
          App.Current.Dispatcher.Invoke(delegate
          {
            data[original_folder].Add(cache_data);
          });
        }
      }

      /**
       * @brief Notifies WPF that the asset list has changed.
       * @param[in] sender (object) The asset list that changed.
       * @param[in] e (NotifyCollectionChangedEventArgs) Arguments passed with the event.
       * @see sulphur.editor.AssetList
       */
      private void OnAssetListChanged(object sender, NotifyCollectionChangedEventArgs e)
      {
        OnPropertyChanged("data");
      }

      /**
       *@brief process all data read from a cache file and process it to construct the database
       *@param[in] cache_file_data (Dictionary<sulhur.editor.utils.UInt64_s, sulphur.editor.AssetDatabase.PackagePtr>) data to process 
       */
      private void ProcessCacheFileData(Dictionary<utils.UInt64_s, PackagePtr> cache_file_data)
      {
        if (cache_file_data == null)
        {
          return;
        }

        foreach (KeyValuePair<utils.UInt64_s, PackagePtr> entry in cache_file_data)
        {
          utils.UInt64_s id = entry.Key;
          PackagePtr ptr = entry.Value;
          ProcessCacheFileData(id, ptr);
        }
        cache_file_data.Clear();
      }

      /**
       *@brief get the asset type from a file path
       *@param[in] path (string) path to a raw asset 
       *@return (sulphur.editor.AssetType)  type of asset as derived from the file extension 
       */
      private AssetType GetAssetType(string path)
      {
        string ext = GetExtension(path);

        switch (ext)
        {
          case ".san": // @todo: is this the correct file type?
            return AssetType.kAnimation;
          case ".sau": // @todo: is this the correct file type?
            return AssetType.kAudio;
          case ".sma":
            return AssetType.kMaterial;
          case ".sme":
            return AssetType.kMesh;
          case ".smo":
            return AssetType.kModel;
          case ".ssc":
            return AssetType.kScript;
          case ".ssp":
            return AssetType.kShader;
          case ".ske": // @todo: is this the correct file type?
            return AssetType.kSkeleton;
          case ".ste":
            return AssetType.kTexture;
          default:
            return AssetType.kUnknown;
        }
      }

      /**
       *@brief instantiate an asset in the engine
       *@param[in] asset (sulphur.editor.AssetCacheData) asset to instantiate
       *@remark when this function is run an instantiate message will be send to the connected engine  
       */
      private void Instantiate(AssetCacheData asset)
      {
        native.messages.AssetInstantiatedPayload msg_data = new native.messages.AssetInstantiatedPayload();
        msg_data.asset_id = asset.id;
        byte[] data = Utils.StructToBytes(msg_data);
        native.Networking.SNetSendData((uint)native.NetworkMessages.kAssetInstantiated,
                                        data, (uint)data.Length);
        native.Networking.SNetFlushPackets();
      }
    }
  }
}
