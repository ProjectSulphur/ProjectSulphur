using System;
using System.Runtime.InteropServices;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      
      public class AssetProcessor
      {
        [DllImport("sulphur-builder-shared.dll")]
        public static extern void Initialize();

        [DllImport("sulphur-builder-shared.dll")]
        public static extern void Shutdown();

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ SetOutputPath(string path);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ SetPackageOutputPath(string path);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportModel(string path, bool single_model, string vertex_shader, string pixel_shader);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportMaterial(string path);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportShader(string path);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ ImportTexture(string path);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteModel(UInt64 id);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteMaterial(UInt64 id);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteShader(UInt64 id);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteTexture(UInt64 id);

        [DllImport("sulphur-builder-shared.dll")]
        public static extern bool_ DeleteMesh(UInt64 id);
      }
    } // native
  } // editor
} // sulphur