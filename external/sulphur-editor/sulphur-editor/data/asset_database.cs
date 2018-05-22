using System.IO;
using System;
using System.Collections.Generic;
using System.Windows.Input;
using System.ComponentModel;

namespace sulphur
{
  namespace editor
  {
    public enum AssetType
    {
      kModel,
      kTexture,
      kShader,
      kMaterial,
      kMesh,
      kUnknown
    }

    public struct AssetCacheData
    {
      public UInt64 id { get; set; }
      public string name { get; set; }
      public string origin { get; set; }
      public string path { get; set; }
      public AssetType type { get; set; }
    }

    public class AssetDatabase : ISubscribeable
    {
      public Dictionary<string, List<AssetCacheData>> data { get; set; }

      public List<AssetCacheData> GetAssetsInFolder(string path)
      {
        if (data.ContainsKey(path) == false)
        {
          return null;
        }
        return data[path];
      }

      public AssetDatabase()
      {
        worker_ = new BackgroundWorker();
        worker_.DoWork += Worker_DoWork;
        worker_.RunWorkerCompleted += Worker__RunWorkerCompleted;
        asset_queue_ = new Queue<string>();
        data = new Dictionary<string, List<AssetCacheData>>();
        ReadCacheFiles();
      }

      private void Worker__RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
      {
        if(asset_queue_.Count != 0)
        {
          worker_.RunWorkerAsync();
        }
      }

      private void Worker_DoWork(object sender, DoWorkEventArgs e)
      {
        ProcessAsset(asset_queue_.Dequeue());
      }

      public void ProcessAsset(string path)
      {
        native.bool_ success = false;
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
          success = native.AssetProcessor.ImportModel(path, true, "vertex", "pixel");
        }

        else if (extension == ".png" ||
                 extension == ".jpeg" ||
                 extension == ".bmp" ||
                 extension == ".tga" ||
                 extension == ".jpg")
        {
          success = native.AssetProcessor.ImportTexture(path);
        }
        else if (extension == ".vert" ||
                 extension == ".comp" ||
                 extension == ".doma" ||
                 extension == ".hull" ||
                 extension == ".geom" ||
                 extension == ".pixe")
        {
          success = native.AssetProcessor.ImportShader(path);
        }

        if (success == false)
        {
          Console.WriteLine("unable to import asset {0}", path);
        }
        else
        {
          ReadCacheFiles();
          native.Networking.SNetSendData((uint)native.NetworkMessages.kCacheChanged, null, 0);
          Console.WriteLine("succesfully import asset {0}", path);
        }
      }

      public void DeleteAsset(object parameter)
      {
        AssetCacheData asset = (AssetCacheData)parameter;

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
          case AssetType.kMaterial:
            success = native.AssetProcessor.DeleteMaterial(asset.id);
            break;
          case AssetType.kMesh:
            success = native.AssetProcessor.DeleteMesh(asset.id);
            break;
          case AssetType.kModel:
            success = native.AssetProcessor.DeleteModel(asset.id);
            break;
          case AssetType.kShader:
            success = native.AssetProcessor.DeleteShader(asset.id);
            break;
          case AssetType.kTexture:
            success = native.AssetProcessor.DeleteTexture(asset.id);
            break;
        }

        if (success == true)
        {
          data[folderpath].Remove(asset);
          ReadCacheFiles();
          native.Networking.SNetSendData((uint)native.NetworkMessages.kCacheChanged, null, 0);
        }
      }

      public ICommand remove_asset_cmd
      {
        get
        {
          return remove_asset_cmd_ ?? 
            (remove_asset_cmd_ = new utils.CommandHandler((object param) => DeleteAsset(param),
                                                          true));
        }
      }

      public ICommand instantiate_cmd
      {
        get
        {
          return instantiate_cmd_ ?? 
            (instantiate_cmd_ = new utils.CommandHandler((object param) => Instantiate(param),
                                                         true));
        }
      }

      #region ISubscribeable
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

      public Subscription[] GetSubscriptions()
      {
        Subscription[] subscriptions = new Subscription[1];
        subscriptions[0] = new Subscription(ID.type_id<Workspace>.value(), OnNotification);
        return subscriptions;
      }
      #endregion

      public void Import(string path)
      {
        asset_queue_.Enqueue(path);
        if(worker_.IsBusy == false)
        {
          worker_.RunWorkerAsync();
        }
      }

      private BackgroundWorker worker_;

      private Queue<string> asset_queue_; 

      private event OnNotification notify_subscribers_;

      private void OnNotification(object sender, NotificationEventArgs e)
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
        }
      }

      private class PackagePtr : IBinarySerializable
      {
        public string origin { get; private set; }
        public string path { get; private set; }

        public void Read(BinaryReader reader)
        {
          origin = reader.ReadString();
          path = reader.ReadString();
        }
      }

      private string GetExtension(string path)
      {
        int index = path.LastIndexOf(".");
        return path.Substring(index);
      }

      private void ReadCacheFiles()
      {
        data.Clear();
        Dictionary<IBinarySerializable, IBinarySerializable> dict;

        BinaryReader reader = new BinaryReader("material_package.cache");
        if (reader.is_ok == true)
        {
          reader.ReadDictionary<utils.UInt64_s, PackagePtr>(out dict);
          ProcessCacheFileData(dict);
        }

        reader = new BinaryReader("mesh_package.cache");
        if (reader.is_ok == true)
        {
          reader.ReadDictionary<utils.UInt64_s, PackagePtr>(out dict);
          ProcessCacheFileData(dict);
        }

        reader = new BinaryReader("model_package.cache");
        if (reader.is_ok == true)
        {
          reader.ReadDictionary<utils.UInt64_s, PackagePtr>(out dict);
          ProcessCacheFileData(dict);
        }

        reader = new BinaryReader("shader_package.cache");
        if (reader.is_ok == true)
        {
          reader.ReadDictionary<utils.UInt64_s, PackagePtr>(out dict);
          ProcessCacheFileData(dict);
        }

        reader = new BinaryReader("texture_package.cache");
        if (reader.is_ok == true)
        {
          reader.ReadDictionary<utils.UInt64_s, PackagePtr>(out dict);
          ProcessCacheFileData(dict);
        }

        OnNotification handle = notify_subscribers_;
        NotificationEventArgs args = new NotificationEventArgs(null, 0, ID.type_id<AssetDatabase>.value());
        handle?.Invoke(this, args);
      }

      private void ProcessCacheFileData(Dictionary<IBinarySerializable, IBinarySerializable> cache_file_data)
      {

        foreach (KeyValuePair<IBinarySerializable, IBinarySerializable> entry in cache_file_data)
        {
          utils.UInt64_s id = (utils.UInt64_s)entry.Key;
          PackagePtr ptr = (PackagePtr)entry.Value;
          AssetCacheData cache_data = new AssetCacheData();
          DirectoryInfo info = new DirectoryInfo(ptr.origin);
          int slash = ptr.path.LastIndexOf("/") + 1;
          int dot = ptr.path.LastIndexOf(".");

          cache_data.id = id.value;
          cache_data.path = ptr.path;
          cache_data.origin = info.FullName;
          cache_data.name = ptr.path.Substring(slash,
                                                dot - slash);
          cache_data.type = GetAssetType(ptr.path);
          string original_folder = info.FullName.Substring(0, info.FullName.LastIndexOf("\\"));
          if (data.ContainsKey(original_folder) == false)
          {
            data.Add(original_folder, new List<AssetCacheData>());
          }
          data[original_folder].Add(cache_data);
        }

        cache_file_data.Clear();
      }

      private AssetType GetAssetType(string path)
      {
        string ext = GetExtension(path);

        if (ext == ".smo")
        {
          return AssetType.kModel;
        }

        else if (ext == ".ste")
        {
          return AssetType.kTexture;
        }

        else if (ext == ".ssp")
        {
          return AssetType.kShader;
        }
        else if (ext == ".sma")
        {
          return AssetType.kMaterial;
        }
        else if (ext == ".sme")
        {
          return AssetType.kMesh;
        }

        return AssetType.kUnknown;
      }

      private ICommand instantiate_cmd_;

      private ICommand remove_asset_cmd_;

      private void Instantiate(object parameter)
      {
        AssetCacheData asset = (AssetCacheData)parameter;
        native.messages.InstantiateObjectMessage msg_data = new native.messages.InstantiateObjectMessage();
        msg_data.asset_id = asset.id;
        byte[] data = Utils.StructToBytes(msg_data);
        native.Networking.SNetSendData((uint)native.NetworkMessages.kAssetInstantiated,
                                        data, (uint)data.Length);
        native.Networking.SNetFlushPackets();
      }
    }
  }
}
