using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.ComponentModel;
using System;
using System.Collections.Specialized;
using Newtonsoft.Json;
using System.IO;

namespace sulphur
{
  namespace editor
  {
    /**
     * @class sulphur.editor.WorldObject
     * @brief A world object roughly represents an entity with a transform component and display information.
     * @author Maarten ten Velden
     */
    [JsonObject(MemberSerialization.OptIn)]
    public class WorldObject : INotifyPropertyChanged
    {
      private bool is_dirty_; //!< Value indicating that this object has changed since it was last serialized.
      /**
       * @brief Property used to get and set the is_dirty value
       */
      public bool is_dirty
      {
        get { return is_dirty_; }
        set
        {
          if (is_dirty_ != value && value == true)
          {
            object_changed_event?.Invoke(this, new EventArgs());
          }
          is_dirty_ = value;
        }
      }

      /**
       * @brief Event fired when the is_dirty flag gets set to true.
       */
      public event EventHandler object_changed_event;

      /**
       * transform component of this world object
       */
      [JsonProperty]
      public TransformComponent transform_ { get; private set; }

      /**
       * @brief The event that is fired when a property changes
       *@see https://msdn.microsoft.com/en-us/library/system.componentmodel.inotifypropertychanged(v=vs.110).aspx 
       */
      public event PropertyChangedEventHandler PropertyChanged;

      private string name_; //!< The actual value of this entity's display name
      /* 
       * @brief Getter and setter for this entity's display name
       */
      public string name
      {
        get
        {
          return name_;
        }
        set
        {
          name_ = value;
          is_dirty = true;
          NotifyPropertyChanged("name");
        }
      }

      private WorldObject previous_; //!< The actual value of the entity that precedes this one in the hierarchy
      /**
       * @brief Getter and (protected) setter for the entity that precedes this one in the hierarchy
       * @remark Setting of this property is done only by the hierarchy that owns it
       * @see sulphur.editor.WorldObject.Hierarchy
       */
      public WorldObject previous
      {
        get
        {
          return previous_;
        }
        protected set
        {
          previous_ = value;
          NotifyPropertyChanged("previous");
        }
      }

      private WorldObject next_; //!< The actual value of the entity that succeeeds this one in the hierarchy
      /**
       * @brief Getter and (protected) setter for the entity that succeeds this one in the hierarchy
       * @remark Setting of this property is done only by the hierarchy that owns it
       * @see sulphur.editor.WorldObject.Hierarchy
       */
      public WorldObject next
      {
        get
        {
          return next_;
        }
        protected set
        {
          next_ = value;
          NotifyPropertyChanged("next");
        }
      }

      private WorldObject parent_; //!< The actual value of the entity that is this one's parent in the hierarchy
      /**
       * @brief Getter and (protected) setter for the entity that is this one's parent in the hierarchy
       * @remark Setting of this property is done only by the hierarchy that owns it
       * @see sulphur.editor.WorldObject.Hierarchy
       */
      public WorldObject parent
      {
        get
        {
          return parent_;
        }
        protected set
        {
          parent_ = value;
          if (value != null)
          {
            transform_.SetParent(value.transform_);
          }
          else
          {
            transform_.SetParent(null);
          }
          NotifyPropertyChanged("parent");
        }
      }

      protected ObservableCollection<WorldObject> children_;  //!< The actual value of the entities that are parented to this one in the hierarchy
      /**
       * @brief Getter for the entities that are parented to this one in the hierarchy
       * @remark Setting of this property is done by the hierarchy that owns it through the protected instance
       * @see sulphur.editor.WorldObject.children_
       * @see sulphur.editor.WorldObject.Hierarchy
       */
      public ReadOnlyObservableCollection<WorldObject> children
      {
        get
        {
          return new ReadOnlyObservableCollection<WorldObject>(children_);
        }
      }

      /**
       * @brief Constructor for an entity with a display name
       * @param[in] name (string) The entity's display name
       * @remarks Protected as the hierarchy that owns the object is responsible for it.
       * @see sulphur.editor.WorldObject.Hierarchy.Create
       */
      protected WorldObject(string name)
      {
        this.name = name;
        children_ = new ObservableCollection<WorldObject>();
        children_.CollectionChanged += ChilderChanged;
        transform_ = new TransformComponent(this);
        is_dirty = true;
      }

      /**
       * @brief Event handler for when the children collection changes.
       * @param[in] sender (object) The list of children that changed.
       * @param[in] e (NotifyCollectionChangedEventArgs) Armguments containing information about what objects got added / removed from the collection
       */
      private void ChilderChanged(object sender, NotifyCollectionChangedEventArgs e)
      {
        if (e.NewItems != null)
        {
          foreach (WorldObject obj in e.NewItems)
          {
            transform_.AttachChild(obj.transform_);
          }
        }

        if (e.OldItems != null)
        {
          foreach (WorldObject obj in e.OldItems)
          {
            transform_.DetachChild(obj.transform_);
          }
        }
      }

      /**
       *@brief implementation of the INotifyPropertyChanged interface 
       *@param[in] property_name (string) The name of the property that changed
       */
      private void NotifyPropertyChanged(string property_name)
      {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(property_name));
      }

      /**
       * @brief Get the index of this entity in the hierarchy
       * @remarks In the worst case scenario this has to go over all entities in the hierarchy so use this with caution
       * @return (int) The index of this entity in the hierarchy that owns it
       */
      public int GetIndex()
      {
        int result = 0;

        WorldObject prev = previous;
        while (prev != null)
        {
          prev = prev.previous;

          ++result;
        }

        return result;
      }
      /**
       * @brief Get the index of this entity in the direct children of it's parent
       * @remarks In the worst case scenario this has to go over all entities in the hierarchy so use this with caution
       * @remarks If the entity is parented to the root this will return the index in the list of root nodes
       * @return (int) The index of this entity in the direct children of it's parent
       */
      public int GetSiblingIndex()
      {
        int result = 0;

        WorldObject prev = previous;
        while (prev != parent)
        {
          // Only increment index if the node is a sibling
          if (prev.parent == parent)
          {
            ++result;
          }

          prev = prev.previous;
        }

        return result;
      }

      /**
       * @brief Check whether or not this object has any children
       * @return (bool) True if this object has any children
       */
      public bool HasChildren()
      {
        return children_.Count > 0;
      }
      /**
       * @brief Get the entity that is this one's last direct child
       * @remarks If this entity has no children this will be null
       * @return (sulphur.editor.WorldObject) The entity that is the last child or null if not found
       */
      public WorldObject GetLastChild()
      {
        return HasChildren() == true ? children_[children_.Count - 1] : null;
      }

      /**
       * @brief Get the next entity in the hierarchy whose parent is the same as this one's parent.
       * @remarks If this entity is the last in the hierarchy
       * @return (sulphur.editor.WorldObject) The next sibling of this entity or null if not found
       */
      public WorldObject GetNextSibling()
      {
        WorldObject target = next;
        while (target != null)
        {
          target = target.next;
        }
        return target;
      }

      /**
       * @brief Get the entity that is this one's last direct or indirect child
       * @detail An heir is an entity that can be either this one's direct child or any of it's children (and so forth).
       * @remarks If this entity has no children this will be null
       * @return (sulphur.editor.WorldObject) The entity that is the last heir or null if not found
       */
      public WorldObject GetLastHeir()
      {
        if (HasChildren() == false)
        {
          return null;
        }

        WorldObject next_heir = children_[children_.Count - 1];
        while (next_heir.HasChildren() == true)
        {
          next_heir = next_heir.GetLastChild();
        }
        return next_heir;
      }
      /**
       * @brief Get the amount of entities that are direct or indirect children of this one
       * @detail An heir is an entity that can be either this one's direct child or any of it's children (and so forth).
       * @return (int) The amount of heirs of this entity
       */
      public int GetNumHeirs()
      {
        WorldObject last_heir = GetLastHeir();
        if (last_heir != null)
        {
          return last_heir.GetIndex() - GetIndex();
        }
        return 0;
      }



      /**
       * @class WorldObject.HierarchyConverter : JsonConverter
       * @brief Jsonconverter class implementation used to convert a hierary from and to Json format.
       * @author Stan Pepels
       */
      class HierarchyConverter : JsonConverter
      {
        /**
         * @brief Checks whether an object of type 'type'  can be converted by this converter.
         * @param[in] type (Type) The type of the object to be converted.
         */
        public override bool CanConvert(Type type)
        {
          return type == typeof(List<WorldObject>);
        }

        /**
         * @brief Reads Json and returns a list of objects representing the hierarchy.
         * @param[in] reader (JsonReader) Reader used to read the file.
         * @param[in] type (Type) Type of the object currently being deserialized.
         * @param[in] existsing_value (existing_value) The value of the parent object if the object being serialized is a member field.
         * @param[in] serializer (serializer) Serializer used to deserialize an object.
         */
        public override object ReadJson(JsonReader reader, Type type, object existsing_value, JsonSerializer serializer)
        {
          List<WorldObject> hierarchy = new List<WorldObject>();
          Stack<int> child_counts = new Stack<int>(); // each item represents the last parent
          Stack<int> parents = new Stack<int>();
          WorldObject obj = null;
          while (reader.Read())
          {
            switch (reader.TokenType)
            {
              case JsonToken.StartObject:
                continue;
              case JsonToken.PropertyName:
                {
                  obj = new WorldObject(reader.Value as string);
                  reader.Read(); // read startobject
                  reader.Read(); // read property name of child count

                  int child_count = reader.ReadAsInt32().Value;
                  reader.Read(); // read components propertyname
                  reader.Read(); // read startobject
                  serializer.Populate(reader, obj);
                  obj.parent = parents.Count > 0 ? hierarchy[parents.Peek()] : null;
                  hierarchy.Add(obj);
                  if (child_count > 0)
                  {
                    child_counts.Push(child_count);
                    parents.Push(hierarchy.Count - 1);
                  }
                  else if (child_counts.Count > 0 && child_counts.Peek() > 0)
                  {
                    int curr = child_counts.Pop();
                    child_counts.Push(--curr);
                  }
                  while (child_counts.Count > 0 && child_counts.Peek() == 0)
                  {
                    child_counts.Pop();
                    parents.Pop();
                    if (child_counts.Count > 0 && child_counts.Peek() > 0)
                    {
                      int curr = child_counts.Pop();
                      child_counts.Push(--curr);
                    }
                  }
                }
                break;
            }
          }
          return hierarchy;
        }

        /**
         * @brief Writes an object to Json format.
         * @param[in] writer (JsonWriter) Writer used to write the Json file.
         * @param[in] value (object) The value to serialize.
         * @param[in] serializer (JsonSerializer) Serializer used to serialize the object.
         */
        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
          List<WorldObject> hierarchy = (List<WorldObject>)value;
          writer.WriteStartObject();
          for (int i = 0; i < hierarchy.Count; ++i)
          {
            WorldObject obj = hierarchy[i];
            writer.WritePropertyName(obj.name);
            writer.WriteStartObject();
            writer.WritePropertyName("child_count");
            writer.WriteValue(obj.children.Count);
            writer.WritePropertyName("components");
            serializer.Serialize(writer, obj);
            obj.is_dirty = false;
            writer.WriteEndObject();
          }
          writer.WriteEndObject();
        }
      }

      /**
       * @class sulphur.editor.WorldObject.Hierarchy
       * @brief Represents a hierarchy that can manage (add, remove, modify) world objects
       * @details This class manages both the data in sequential order (for synchronization with
       *   the engine) and the views that can be bound to WPF controls.
       * @author Maarten ten Velden
       */
      public class Hierarchy
      {
        public bool is_dirty { get; set; } //!< Value indicating that the hierarchy has changed since it was last svaed to disk.

        /**
         * @brief Event handler for handling object changed events
         * @param[in] sender (object) The world object that changed.
         * @param[in] e (EventArgs) No arguments are send with this event
         * @see sulphur.editor.WorldObject.
         */
        private void HandleObjectChanged(object sender, EventArgs e)
        {
          is_dirty = true;
        }


        private readonly List<WorldObject> hierarchy_; //!< The sequential entity data
        private readonly ObservableCollection<WorldObject> root_; //!< The actual value of the entities whose parent is the root
        /**
         * @brief Getter for the entities whose parent is the root
         */
        public ReadOnlyObservableCollection<WorldObject> root
        {
          get
          {
            return new ReadOnlyObservableCollection<WorldObject>(root_);
          }
        }

        /**
         * @brief Constructor.
         */
        public Hierarchy()
        {
          hierarchy_ = new List<WorldObject>();
          root_ = new ObservableCollection<WorldObject>();
        }

        /**
         * @brief Gets an object at an index in the hieracrhy.
         * @param[in] index (int) The hierarchy index of the object to retrieve.
         * @return (WorldObject) The world object at the specified index.
         */
        public WorldObject GetObjectAtIndex(int index)
        {
          return index >= hierarchy_.Count ? null : hierarchy_[index];
        }

        /**
         * @brief Get the number of objects in the hierarchy.
         * @return (int) The number of objects in the hierarchy.
         */
        public int GetObjectCount()
        {
          return hierarchy_.Count;
        }

        /**
         * @brief Add a new entity to the hierarchy with an optional parent
         * @param[in] name (string) The new entity's display name
         * @param[in] parent (sulphur.editor.WorldObject) The new entity's parent
         * @return (sulphur.editor.WorldObject) The new entity
         */
        public WorldObject Create(string name, WorldObject parent = null)
        {
          WorldObject node = new WorldObject(name);
          node.object_changed_event += HandleObjectChanged;

          if (parent == null)
          {
            InsertNode(node, hierarchy_.Count);
          }
          else
          {
            InsertNode(node, parent.GetIndex() + parent.GetNumHeirs() + 1);
          }

          node.parent_ = parent;
          AddParentViews(node);
          is_dirty = true;
          return node;
        }

        /**
         * @brief Add an entity to the hierarchy with an optional parent
         * @param[in] obj (WorldObject) The Entity to add to the hierarchy.
         * @param[in] parent (sulphur.editor.WorldObject) The new entity's parent
         * @return (sulphur.editor.WorldObject) The added entity
         * @remarks This function is mainly used to construct an entity before adding it to the hierarchy. This way all components can be added to the entity up front.
         */
        public WorldObject Add(WorldObject obj, WorldObject parent = null)
        {
          WorldObject node = obj;
          node.object_changed_event += HandleObjectChanged;

          if (parent == null)
          {
            InsertNode(node, hierarchy_.Count);
          }
          else
          {
            InsertNode(node, parent.GetIndex() + parent.GetNumHeirs() + 1);
          }

          node.parent_ = parent;
          AddParentViews(node);
          is_dirty = true;
          return node;
        }

        /**
         * @brief Remove an entity and all its children from the hierarchy
         * @param[in] target (sulphur.editor.WorldObject) The target entity to remove
         */
        public void Delete(WorldObject target)
        {
          RemoveRange(target.GetIndex(), target.GetNumHeirs() + 1);
          RemoveParentViews(target);
        }

        /**
         * @brief Set the parent of a node in the hierarchy
         * @param[in] new_parent (sulphur.editor.WorldObject) The new parent node of target or null
         *   to indicate the root
         * @remarks Nodes are not allowed to be their own parents.
         * @remarks If the new parent is the same as the target's current one nothing will happen
         */
        public void SetParent(WorldObject target, WorldObject new_parent)
        {
          if (new_parent == target)
          {
            return;
          }

          if (target.parent != new_parent)
          {
            int old_index = target.GetIndex();
            int old_num_heirs = target.GetNumHeirs();

            List<WorldObject> heritage = hierarchy_.GetRange(old_index, old_num_heirs + 1);
            RemoveRange(old_index, old_num_heirs + 1);

            if (new_parent == null)
            {
              InsertRange(heritage, hierarchy_.Count);
            }
            else
            {
              InsertRange(heritage, new_parent.GetIndex() + 1 + new_parent.GetNumHeirs());
            }

            RemoveParentViews(target);
            target.parent = new_parent;
            AddParentViews(target);
          }
        }

        /**
         * @brief Insert a range of nodes into the hierarchy at the specified index
         * @param[in] node (sulphur.editor.WorldObject) The node to insert
         * @param[in] index (int) The index at which to insert the node
         * @remarks This only inserts the nodes, but does not update the neighbouring linked-list
         *   nodes. For the purpose of fixing up the nodes affected by a change in the hierarchy
         *   there is MakeNeighboursSafe.
         * @see sulphur.editor.WorldObject.AddParentViews
         * @see sulphur.editor.WorldObject.RemoveParentViews
         */
        private void InsertRangeDataOnly(List<WorldObject> nodes, int index)
        {
          if (index == hierarchy_.Count)
          {
            hierarchy_.AddRange(nodes);
          }
          else
          {
            hierarchy_.InsertRange(index, nodes);
          }
        }
        /**
         * @brief Insert a node into the hierarchy at the specified index
         * @param[in] node (sulphur.editor.WorldObject) The node to insert
         * @param[in] index (int) The index at which to insert the node
         * @remarks This only inserts the nodes, but does not update the neighbouring linked-list 
         *   nodes. For the purpose of fixing up the nodes affected by a change in the hierarchy 
         *   there is MakeNeighboursSafe.
         * @see sulphur.editor.WorldObject.MakeNeighboursSafe
         */
        private void InsertNodeDataOnly(WorldObject node, int index)
        {
          if (index == hierarchy_.Count)
          {
            hierarchy_.Add(node);
          }
          else
          {
            hierarchy_.Insert(index, node);
          }
        }
        /**
         * @brief Make the two nodes specified by left_index and right_index neighbours of one another.
         * @param[in] left_index (int) The index of the left node whose next property will point to
         *   the node specified by right_index after assignment (if non-null)
         * @param[in] right_index (int) The index of the right node whose previous property will
         *   point to the node specified by right_index after assignment (if non-null)
         * @remarks This function is safe as left_index and right_index may go out of range. They
         *   may not, however, come in the wrong order (i.e. left_index must precede right_index). 
         */
        private void MakeNeighboursSafe(int left_index, int right_index)
        {
          if (left_index > right_index)
          {
            throw new System.ArgumentException("Left index may not be greater than right index");
          }

          WorldObject left = null;
          if (left_index >= 0)
          {
            left = hierarchy_[left_index];
          }

          WorldObject right = null;
          if (right_index < hierarchy_.Count)
          {
            right = hierarchy_[right_index];
          }

          if (left != null)
          {
            left.next = right;
          }
          if (right != null)
          {
            right.previous = left;
          }
        }

        /**
         * @brief Insert a node into the hierarchy at the specified index
         * @param[in] node (sulphur.editor.WorldObject) The node to insert
         * @param[in] index (int) The index at which to insert the node
         * @remarks This only updates the hierarchy data, not the views. For updating the views
         *   you can call AddParentViews or RemoveParentViews.
         * @see sulphur.editor.WorldObject.AddParentViews
         * @see sulphur.editor.WorldObject.RemoveParentViews
         */
        private void InsertNode(
          WorldObject node,
          int index)
        {
          if (hierarchy_.Contains(node) == true)
          {
            Console.Out.Write("Duplicate node found in target list");
            return;
          }

          InsertNodeDataOnly(node, index);

          MakeNeighboursSafe(index - 1, index);
          MakeNeighboursSafe(index, index + 1);
        }
        /**
         * @brief Remove a node from the hierarchy at the specified index
         * @param[in] index (int) The index of the effected node
         * @remarks This only updates the hierarchy data, not the views. For updating the views
         *   you can call AddParentViews or RemoveParentViews.
         * @see sulphur.editor.WorldObject.AddParentViews
         * @see sulphur.editor.WorldObject.RemoveParentViews
         */
        private void RemoveNode(
          int index)
        {
          if ((index > 0 && index < hierarchy_.Count) == false)
          {
            throw new System.IndexOutOfRangeException();
          }

          WorldObject node = hierarchy_[index];

          if (index > 0)
          {
            hierarchy_[index - 1].next = node.next;
            node.next = null;
          }
          if (index < hierarchy_.Count - 1)
          {
            hierarchy_[index + 1].previous = node.previous;
            node.previous = null;
          }
          hierarchy_.RemoveAt(index);
        }

        /**
         * @brief Insert a range of nodes into the hierarchy at the specified index
         * @param[in] nodes (System.Collections.Generic.List <sulphur.editor.WorldObject>) The list 
         *   of nodes to insert.
         * @param[in] index (int) The index at which to insert the nodes.
         * @remarks This only updates the hierarchy data, not the views. For updating the views
         *   you can call AddParentViews or RemoveParentViews.
         * @see sulphur.editor.WorldObject.AddParentViews
         * @see sulphur.editor.WorldObject.RemoveParentViews
         */
        private void InsertRange(List<WorldObject> nodes, int index)
        {
          InsertRangeDataOnly(nodes, index);

          // Patch up both ends in the linked-list
          MakeNeighboursSafe(index - 1, index);
          MakeNeighboursSafe(index + nodes.Count - 1, index + nodes.Count);
        }
        /**
         * @brief Remove a range of nodes from the hierarchy starting from the specified index
         * @param[in] index (int) The index of the first node to remove
         * @param[in] num_nodes (int) The amount of nodes to remove
         * @remarks This only updates the hierarchy data, not the views. For updating the views
         *   you can call AddParentViews or RemoveParentViews.
         * @see sulphur.editor.WorldObject.AddParentViews
         * @see sulphur.editor.WorldObject.RemoveParentViews
         */
        private void RemoveRange(int index, int num_nodes)
        {
          hierarchy_.RemoveRange(index, num_nodes);

          // Patch up the created gap in the linked-list
          MakeNeighboursSafe(index - 1, index);
        }

        /**
         * @brief Register a node to its current parent's view.
         * @param[in] parent (sulphur.editor.WorldObject) The target node who will be added
         *   to its current parent's view.
         */
        private void AddParentViews(WorldObject target)
        {
          if (target.parent == null)
          {
            root_.Add(target);
          }
          else
          {
            target.parent.children_.Add(target);
          }
        }
        /**
         * @brief Move a node to a new index in its current parent's view.
         * @param[in] parent (sulphur.editor.WorldObject) The target node who will be moved.
         * @param[in] new_index (int) The new index in the view of the target node's current parent.
         */
        private void MoveParentViews(WorldObject target, int new_index)
        {
          int old_index = target.GetSiblingIndex();
          if (target.parent == null)
          {
            root_.Move(old_index, new_index);
          }
          else
          {
            target.parent.children_.Move(old_index, new_index);
          }
        }
        /**
         * @brief Remove a node from its current parent's view.
         * @param[in] target (sulphur.editor.WorldObject) The target node who will be removed
         *   from its current parent's view.
         */
        private void RemoveParentViews(WorldObject target)
        {
          if (target.parent == null)
          {
            root_.Remove(target);
          }
          else
          {
            target.parent.children_.Remove(target);
          }
        }

        /**
         * @brief Loads a '*.swo' file from disk and fils the hierarchy with its contents.
         * @param[in] path (string) The path to the *.swo file.
         */
        public void LoadFromDisk(string path)
        {
          JsonSerializer serializer = new JsonSerializer();
          serializer.Converters.Add(new HierarchyConverter());
          serializer.Formatting = Formatting.Indented;
          List<WorldObject> objects = null;
          using (StreamReader sw = new StreamReader(path))
          using (JsonReader reader = new JsonTextReader(sw))
          {
            objects = serializer.Deserialize<List<WorldObject>>(reader);
          }

          if(objects == null)
          {
            is_dirty = false;
            return;
          }

          foreach (WorldObject obj in objects)
          {
            obj.is_dirty = false;
            Add(obj, obj.parent);

            // temp code untill engine side serialization is done
            native.messages.EntityCreatedMessage msg = new native.messages.EntityCreatedMessage();
            msg.entity_index = (UInt64)obj.GetIndex();
            msg.sibling_index = (UInt64)obj.GetSiblingIndex();
            msg.parent_index = obj.parent != null ? (UInt64)obj.parent.GetIndex() : UInt64.MaxValue;
            msg.position = new float[3];
            msg.rotation = new float[4];
            msg.scale = new float[3];

            msg.position[0] = obj.transform_.position_.X;
            msg.position[1] = obj.transform_.position_.Y;
            msg.position[2] = obj.transform_.position_.Z;

            msg.rotation[0] = obj.transform_.rotation_.X;
            msg.rotation[1] = obj.transform_.rotation_.Y;
            msg.rotation[2] = obj.transform_.rotation_.Z;
            msg.rotation[3] = obj.transform_.rotation_.W;

            msg.scale[0] = obj.transform_.scale_.X;
            msg.scale[1] = obj.transform_.scale_.Y;
            msg.scale[2] = obj.transform_.scale_.Z;

            byte[] data = Utils.StructToBytes(msg);
            native.Networking.SNetSendData(
              (uint)native.NetworkMessages.kEntityCreated,
              data,
              (uint)data.Length);
            
            // end temp code
          }
          is_dirty = false;
        }

        /**
         * @brief Saves the hierarchy to a file so it can loaded back in at a later ime.
         * @remark The file written to is the currently loaded world.
         * @see sulphur.editor.Project.current_world_ 
         */
        public void SaveToDisk()
        {
          if (is_dirty == false)
          {
            return;
          }

          JsonSerializer serializer = new JsonSerializer();
          serializer.Converters.Add(new HierarchyConverter());
          serializer.Formatting = Formatting.Indented;

          using (StreamWriter sw = new StreamWriter(Project.directory_path + "\\" + Project.current_world_))
          using (JsonWriter writer = new JsonTextWriter(sw))
          {
            serializer.Serialize(writer, hierarchy_);
          }
          is_dirty = false;
        }
      }
    }
  }
}
