using System;
using System.Runtime.InteropServices;
using System.Text;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      /**
       *@class sulphur.editor.native.AssetProcessor
       *@brief class with static functions to leveradge the sulphur-builder
       *@author Stan Pepels  
       */
      public class AssetProcessor
      {
        /**
         *@see sulphur.builder.shared.Initialize 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern void Initialize();

        /**
         * @see sulphur.builder.shared.CreateDefaults 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern void CreateDefaults();

        /**
         *@see sulphur.builder.shared.Shutdown 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern void Shutdown();

        /**
         *@see sulphur.builder.shared.SetOutputPath 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ SetOutputPath(string path);

        /**
         *@see sulphur.builder.shared.SetPackageOutputPath 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ SetPackageOutputPath(string path);

        /**
         *@see sulphur.builder.shared.ImportModel 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportModel(string path, bool single_model, string vertex_shader, string pixel_shader);

        /**
         *@see sulphur.builder.shared.ImportMaterial 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportMaterial(string path, ref UInt64 ptr);

        /**
         *@see sulphur.builder.shared.ImportShader 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportShader(string path, ref UInt64 ptr);

        /**
         *@see sulphur.builder.shared.ImportTexture 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportTexture(string path, ref UInt64 ptr);

        /**
         *@see sulphur.builder.shared.ImportAudio 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportAudio(string path, ref UInt64 ptr);

        /**
         *@see sulphur.builder.shared.ImportScript 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportScript(string path, ref UInt64 ptr);
        
        /**
         *@see sulphur.builder.shared.DeleteModel 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteModel(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteMaterial 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteMaterial(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteShader 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteShader(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteTexture 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteTexture(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteAudio 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteAudio(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteAnimation 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteAnimation(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteScript 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteScript(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteSkeleton 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteSkeleton(UInt64 id);

        /**
         *@see sulphur.builder.shared.DeleteMesh 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteMesh(UInt64 id);

        /**
         *@see sulphur.builder.shared.UpdatePackagePtrs 
         */
        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ UpdatePackagePtrs(string[] new_origins, UInt64[] asset_ids, Int32[] asset_types, Int32 count);

        /**
         *@see sulphur.builder.shared.GetAssetPackageName 
         */
        [DllImport("sulphur-builder-shared.dll")]
        private static extern bool_ GetAssetPackageName(Int32 asset_type, sbyte[] out_data, Int32 buffer_size);

        /**
         * @brief Gets the packagename of the cache of a certain asset type.
         * @param[in] type (sulphur.editor.AssetType) Type of the asset to get the cache name of.
         * @return (string) Name of the cache.
         */
        public static string GetAssetPackageName(AssetType type)
        {
          sbyte[] buf = new sbyte[255];
          if(GetAssetPackageName((Int32)type, buf, buf.Length) == true)
          {
            unsafe
            {
              fixed (sbyte* ptr = buf)
              {
                string result = new string(ptr, 0, buf.Length, Encoding.ASCII);
                result = result.Remove(result.IndexOf('\0'));
                return result;
              }
            }
          }
          return null;
        }
      }
    } // native
  } // editor
} // sulphur