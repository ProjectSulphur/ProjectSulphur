using System;
using System.Runtime.InteropServices;

namespace sulphur
{
  namespace editor
  {
    namespace native
    {
      namespace messages
      {
        [StructLayout(LayoutKind.Sequential)]
        struct StringMessage
        {
          public byte[] bytes;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct InstantiateObjectMessage
        {
          public UInt64 asset_id;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct MouseClickMessage
        {
          public UInt64 id;
          public int x_pos;
          public int y_pos;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct RayCastResultMessage
        {
          float x, y, z;
          float dx, dy, dz;
          float t;
          UInt64 obj_id;
        }
      }

      public enum NetworkMessages : uint
      {
        kPOD = 0, //!< Unformatted plain-old-data
        kString, //!< A text string
        kWindowHandle, //!< A window handle to render to
        kEntityCreated, //!< The creation of a new entity
        kEntityDestroyed, //!< The destruction of an existing entity
        kAssetInstantiated, //!< The instantiation of an asset in the world
        kStartedPlaying, //!< A request from the editor to start playing the game
        kStoppedPlaying, //!< A request from the editor to stop playing the game
        kComponentAdded, //!< The creation of a new component on an entity
        kComponentRemoved, //!< The destruction of an existing component on an entity
        kClickRaycast, //!< A request for a raycast hit result from the viewport of a camera component
        kRaycast, //!< A request for a raycast hit result from world-space coordinates
        kRaycastResult, //!< A hit result of a raycast
        kCacheChanged, //!< Notification that the asset-cache must be reloaded
      }

      class Networking
      {

        public const uint kPacketSize = 512;
        public const uint kMaxPayLoadSize = kPacketSize - sizeof(UInt32);
        public struct Packet
        {
          public byte[] data;
        }

        public enum ErrorMessage : uint
        {
          kOk,
          kEnetInitFailed,
          kHostCreateFailed,
          kResolveIpFailed,
          kConnectFailed,
          kPacketCreationFailed,
          kNoPeerConnected,
          kPacketSendFailed
        };

        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetInitEditor();

        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetEditorConnect(string ip, UInt32 port);

        [DllImport("sulphur-networking.dll")]
        public static extern bool_ SNetRetrievePacket(ref UInt32 id, [Out]byte[] buf, uint size = kMaxPayLoadSize);

        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetSendData(UInt32 id, byte[] buf, uint size = kMaxPayLoadSize);

        [DllImport("sulphur-networking.dll")]
        public static extern void SNetDestroy();

        [DllImport("sulphur-networking.dll")]
        public static extern bool_ SNetIsConnected();

        [DllImport("sulphur-networking.dll")]
        public static extern void SNetFlushPackets();
      }
    } // native
  } // editor
} // sulphur
