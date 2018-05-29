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
        /**
         *@struct sulphur.editor.native.messages.StringMessage
         *@brief message for sending / retrieving a string message from / to the engine
         *@author Stan Pepels  
         */
        [StructLayout(LayoutKind.Sequential)]
        struct StringMessage
        {
          public byte[] bytes; //!< byte array containg the string data in 1byte char values
        }

        /**
         *@struct sulphur.editor.native.messages.AssetInstantiatedPayload
         *@brief message for sending an instantiate message to the engine 
         *@see sulphur.editor.native.NetworkMessages.kAssetInstantiated
         *@author Stan Pepels
         */
        [StructLayout(LayoutKind.Sequential)]
        struct AssetInstantiatedPayload
        {
          public UInt64 asset_id; //!< id of the asset to be instantiated
        }

        /**
         *@struct sulphur.editor.native.messages.WindowHandleMessage
         *@brief message for providing a waiting engine with a window handle to render to
         *@see sulphur.editor.native.NetworkMessages.kWindowhandle
         *@author Maarten ten Velden
         */
        [StructLayout(LayoutKind.Sequential)]
        struct WindowHandleMessage
        {
          public Int64 handle; //!< The window handle to render to
        }

        /**
         *@struct sulphur.editor.native.messages.EntityCreatedMessage
         *@brief message for creating a new entity in the engine with an optional parent
         *@see sulphur.editor.native.NetworkMessages.kEntityCreated
         *@author Maarten ten Velden
         */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityCreatedMessage
        {
          public UInt64 entity_index; //!< The index in the hierarchy of the new entity
          public UInt64 sibling_index; //!< The sibling index of the new entity
          public UInt64 parent_index; //!< The index in the hierarchy of the new entity's parent (or UInt64.MaxValue to indicate no parent)
        }

        /**
         *@struct sulphur.editor.native.messages.EntityDestroyedMessage
         *@brief message for deleting an existing entity from the engine
         *@see sulphur.editor.native.NetworkMessages.kEntityDestroyed
         *@author Maarten ten Velden
         */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityDestroyedMessage
        {
          public UInt64 entity_index; //!< The index in the hierarchy of the entity to be destroyed
        }
        /**
         *@struct sulphur.editor.native.messages.EntityReparentedMessage
         *@brief message for modifying an existing entity's parent in the engine
         *@see sulphur.editor.native.NetworkMessages.kEntityReparented
         *@author Maarten ten Velden
         */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityReparentedMessage
        {
          public UInt64 entity_old_index; //!< The index before reparenting in the hierarchy of the entity
          public UInt64 new_parent_old_index; //!< The index before reparenting in the hierarchy of the entity's new parent (or UInt64.MaxValue to indicate no parent)
          public UInt64 new_sibling_index; //!< The index after reparenting in the hierarchy of the entity
        }

        /**
        *@struct sulphur.editor.native.messages.EntitySelectedMessage
        *@brief message used to indicate that an entity was selected in the editor.
        *@see sulphur.editor.native.NetworkMessages.kEntitySelected
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct EntitySelectedMessage
        {
          public UInt64 entity_index; //!< index of the entity in the world hierarchy.
        }

        /**
        *@struct sulphur.editor.native.messages.EntityMovedMessage
        *@brief message used to indicate that an entity was moved in the editor.
        *@see sulphur.editor.native.NetworkMessages.kEntityMoved
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityMovedMessage
        {
          public UInt64 entity_index; //!< index of the entity in the world hierarchy.
          public float x; //!< The amount of units to move the entity along the world x axis.
          public float y; //!< The amount of units to move the entity along the world y axis.
          public float z; //!< The amount of units to move the entity along the world z axis.
        }

        /**
        *@struct sulphur.editor.native.messages.SetTransformGizmoMessage
        *@brief message used to indicate a different transform gizmo was selected.
        *@see sulphur.editor.native.NetworkMessages.kSetTransformGizmo
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct SetTransformGizmoMessage
        {
          /**
           * @brief Enum of gizmo types.
           */
          public enum Type
          {
            kTranslate, //!< Gizmo used to move an entity in the scen view.
            kRotate, //!< Gizmo used to rotate an entity in the scene view.
            kScale //!< Gizmo used to scale an entity in the scene view.
          }

          public Int32 type; //!< The type of transform gizmo to use.
        }

        /**
        *@struct sulphur.editor.native.messages.EntityScaleMessage
        *@brief message used to indicate that an entity was scaled in the editor.
        *@see sulphur.editor.native.NetworkMessages.kEntityScaled
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityScaleMessage
        {
          UInt64 entity_index; //!< index of the entity in the world hierarchy.
          float x; //!< The scaling to be applied in the x direction.
          float y; //!< The scaling to be applied in the y direction.
          float z; //!< The scaling to be applied in the z direction.
        };

        /**
        *@struct sulphur.editor.native.messages.EntityRotateMessage
        *@brief message used to indicate that an entity was rotated in the editor.
        *@see sulphur.editor.native.NetworkMessages.kEntityRotated
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct EntityRotateMessage
        {
          UInt64 entity_index; //!< index of the entity in the world hierarchy.
          float w; //!< The w component of the quaternion with which to rotate the entity.
          float x; //!< The x component of the quaternion with which to rotate the entity.
          float y; //!< The y component of the quaternion with which to rotate the entity.
          float z; //!< The z component of the quaternion with which to rotate the entity.
        };

        /**
        *@struct sulphur.editor.native.messages.LoadProjectMessage
        *@brief message that a different project was loaded.
        *@see sulphur.editor.native.NetworkMessages.kLoadProject
        *@author Stan Pepels
        */
        [StructLayout(LayoutKind.Sequential)]
        struct LoadProjectMessage
        {
          /**
           * @brief Constructor used to put the string into a fixed size byte array.
           * @param[in] path (string) The value to contstruct the byte array from.
           */
          public LoadProjectMessage(string path)
          {
            directory_path = new byte[Networking.kMaxPayLoadSize];
            byte[] str_bytes = System.Text.Encoding.ASCII.GetBytes(path);

            Buffer.BlockCopy(str_bytes, 0, directory_path, 0, str_bytes.Length);
          }

          byte[] directory_path; //!< Byte array with data containning the converted path string.
        };
      }

      /**
       *@see sulphur::engine::MessageID 
       */
      public enum NetworkMessages : uint
      {
        kWindowHandle = 0,
        kEntityCreated, //!< The creation of a new entity
        kEntityDestroyed, //!< The destruction of an existing entity
        kEntityReparented, //!< The change of an existing entity's parent
        kAssetInstantiated, //!< The instantiation of an asset in the world
        kComponentAdded, //!< The creation of a new component on an entity
        kComponentRemoved, //!< The destruction of an existing component on an entity
        kCacheChanged, //!< Notification that the asset-cache must be reloaded

        kFastBackward, //!< A request from the editor to start rewinding
        kPreviousFrame,  //!< A request from the editor to rewind one frame
        kStartedPlaying, //!< A request from the editor to start playing the game
        kStoppedPlaying, //!< A request from the editor to stop playing the game
        kNextFrame,  //!< A request from the editor to forward one frame
        kFastForward,  //!< A request from the editor to fast forward

        kStartRewinding, //!< A request from the editor to start rewinding
        kStopRewinding, //!< A request from the editor to stop rewinding
        kObjectSelected, //!< An object got selected via a control in the editor.
        kSetTransformGizmo, //!< Sets the current type of transform gizmo to use.
        kEntitySelected, //!< A notification from the editor that acretain entity was selected.
        kEntityMoved, //!< A Notification from the editor that an entity was moved.
        kEntityRotated, //!< A Notification from the editor that an entity was rotated.
        kEntityScaled, //!< A Notification from the editor that an entity was scaled.
        kLoadProject, //!< A Notification from the editor that a different project was loaded.
        kNumMessages, //!< The amount of unique messages that can be received
      }

      /**
       *@class sulphur.editor.native.Networking
       *@brief class with static functions to leveradge the sulphur-networking
       *@author Stan Pepels   
       */
      class Networking
      {
        public const uint kPacketSize = 512; //!< max size of a package in bytes
        public const uint kMaxPayLoadSize = kPacketSize - sizeof(UInt32); //!< max payload size of a package

        /**
         *@struct sulphur.editor.native.Networking.Packet
         *@brief struct for retrieving a packet from the sukphur-networking.dll
         *@author Stan Pepels   
         */
        public struct Packet
        {
          public byte[] data; //!< data that came with the packet
        }

        /**
         *@see sulphur::networking::editor::ErrorMessage 
         */
        public enum ErrorMessage : uint
        {
          kOk, //!< ok
          kEnetInitFailed, //!< Enet failed to initialise
          kHostCreateFailed, //!< Creating a host failed
          kResolveIpFailed, //!< Resolving the IP address failed
          kConnectFailed, //!< Connecting to IP failed
          kPacketCreationFailed, //!< Failed to create a packet
          kNoPeerConnected, //!< No peer connected
          kPacketSendFailed //!< Failed to send a packet
        };

        /**
         *@see sulphur::networking::editor::SNetInitEditor 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetInitEditor();

        /**
         *@see sulphur::networking::editor::SNetEditorConnect 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetEditorConnect(string ip, UInt32 port);

        /**
         *@see sulphur::networking::editor::SNetRetrievePacket 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern bool_ SNetRetrievePacket(ref UInt32 id, [Out]byte[] buf, uint size = kMaxPayLoadSize);

        /**
         *@see sulphur::networking::editor::SNetSendData 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern ErrorMessage SNetSendData(UInt32 id, byte[] buf, uint size = kMaxPayLoadSize);

        /**
         *@see sulphur::networking::editor::SNetDestroy 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern void SNetDestroy();

        /**
         *@see sulphur::networking::editor::SNetIsConnected 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern bool_ SNetIsConnected();

        /**
         *@see sulphur::networking::editor::SNetFlushPackets 
         */
        [DllImport("sulphur-networking.dll")]
        public static extern void SNetFlushPackets();
      }
    } // native
  } // editor
} // sulphur
