using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.DynamicDock : DockPanel, ISubscribeable
   * @brief Class that extends the WPF dockpanel. This panel allows for drag n drop docking and allows controls to be grouped together with tabs.
   * @see sulphur.editor.controls.DynamicDockSplitter
   * @see sulphur.editor.controls.DynamicDockTab
   * @see sulphur.editor.controls.DynamicDockTabControl
   * @see sulphur.editor.controls.TabWindow
   * @see sulphur.editor.ISubscribeable
   * @see https://msdn.microsoft.com/en-us/library/system.windows.controls.dockpanel(v=vs.110).aspx
   * @author Stan Pepels
   */
  public class DynamicDock : DockPanel, ISubscribeable
  {
 
    public static readonly DependencyProperty group_property = DependencyProperty.RegisterAttached(
  "Group",
  typeof(uint),
  typeof(DynamicDock),
  new FrameworkPropertyMetadata(uint.MaxValue, FrameworkPropertyMetadataOptions.Inherits)
  ); //!< Group Attached property that can be used to group controls together in the same tab control.

    /**
     * @brief Sets the group for a UIElement
     * @param[in] element (UIElement) lement to set the property on.
     * @param[in] value (uint) Value to set the property on the UIElement to.
     */
    public static void SetGroup(UIElement element, uint value)
    {
      element.SetValue(group_property, value);
    }

    /**
     * @brief Gets the group a UIElement belongs to.
     * @param[in] element (UIElement) Element to get the property from.
     * @return (uint) Value of the group the UIElement belongs to.
     */
    public static uint GetGroup(UIElement element)
    {
      return (uint)element.GetValue(group_property);
    }

    /**
     * @class sulphur.editor.controls.DynamicDock.Pair
     * @brief Class representing an element - splitter pair. In the dockpanel each new element is divided with a splitter that allows the elements to be resized within the dock panel.
     * This class allows for easy access of the splitter that was added with a new element.
     * @author Stan Pepels
     */
    private class Pair
    {
      public DynamicDockSplitter splitter; //!< The splitter added in front of the added element.
      public DynamicDockTabControl element; //!< The element added to the dockpanel. This element is the main target of the splitter.
      public DynamicDockAdorner adorner; //!< Adorner used to dock to this pair.
      public Dock dockmode; //!< The docking mode that is used to arrange the pair.

      /**
       * @brief Creates the splitter element used for dynamic resizing of an element.
       */
      public void CreateSplitter()
      {
        splitter = new DynamicDockSplitter(element);
        SetDock(splitter, dockmode);
      }
    }

    List<Pair> paired_children_; //!< All Children of this dockpanel paired up with a splitter.

    /**
     * @brief Runs after WPF initialization of the control is complete. This functions puts all childs in their respective tab controls and dockpanels.
     */
    public override void EndInit()
    {
      base.EndInit();
      for (int i = 0; i < Children.Count; ++i)
      {
        UIElement child = Children[i];
        uint group = GetGroup(child);

        DynamicDockTab item = new DynamicDockTab();
        item.Header = ((FrameworkElement)child).Name;
        item.Content = new DynamicDock();
        item.Content = child;

        if (group != uint.MaxValue)
        {
          bool group_found = false;
          foreach (Pair entry in paired_children_)
          {
            if (group == GetGroup(entry.element))
            {
              entry.element.Items.Add(item);
              group_found = true;
              break;
            }
          }

          if (group_found == true)
          {
            continue;
          }
        }

        Dock dockmode = GetDock(child);
        DynamicDockTabControl tabs = new DynamicDockTabControl(true);
        tabs.items_changed += TabItemsChanged;
        tabs.Items.Add(item);
        SetGroup(tabs, group);

        Pair pair = new Pair();
        pair.splitter = null;
        pair.element = tabs;
        pair.dockmode = dockmode;
        pair.adorner = new DynamicDockAdorner(pair.element);
        SetDock(pair.element, pair.dockmode);
        if (paired_children_.Count != 0)
        {
          paired_children_[paired_children_.Count - 1].CreateSplitter();
        }
        paired_children_.Add(pair);
      }

      Children.Clear();
      foreach (Pair pair in paired_children_)
      {
        Children.Add(pair.element);

        if (pair.splitter != null)
        {
          Children.Add(pair.splitter);
        }
      }
    }

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
      *@remark this system is subscribed to the sulphur.editor.controls.TabWindow
      *@see sulphur.editor.controls.TabWindow.Notifications
      */
    public Subscription[] GetSubscriptions()
    {
      Subscription[] subscription = new Subscription[1];
      subscription[0] = new Subscription(id<TabWindow>.type_id_, OnNotification);
      return subscription;
    }

    /**
     *@brief Event fired when subscribers need to be notified.
     */
    private event OnNotification notify_subscribers_;
    #endregion

    /**
     * @brief Handles notifications recieved from tabwindows.
     * @param[in] sender (object) Sender of the event. 
     * @param[in] e (NotificationEventArgs) Arguments send when the event was fired. 
     * This contains the id of the event and some optional additional data.
     * @see sulphur.editor.NotificationEventArgs
     */
    private void OnNotification(object sender, NotificationEventArgs e)
    {
      TabWindow.Notifications id = (TabWindow.Notifications)e.notification_id;
      switch (id)
      {
        case TabWindow.Notifications.kDragEnd:
          foreach (Pair pair in paired_children_)
          {
            AdornerLayer layer = AdornerLayer.GetAdornerLayer(pair.element);
            layer.Remove(pair.adorner);
          }
          break;
        case TabWindow.Notifications.kDragStart:
          foreach (Pair pair in paired_children_)
          {
            AdornerLayer layer = AdornerLayer.GetAdornerLayer(pair.element);
            layer.Add(pair.adorner);
          }

          break;
        case TabWindow.Notifications.kDrop:
          foreach (Pair pair in paired_children_)
          {
            if (pair.adorner.DockWindow(sender as TabWindow))
            {
              Log.Write(Log.Verbosity.kInfo, "docked");
              break;
            }
            else if (pair.element.AddWindow(sender as TabWindow))
            {
              Log.Write(Log.Verbosity.kInfo, "tabs added");
              break;
            }
          }
          break;
        case TabWindow.Notifications.kDragMove:
          foreach (Pair pair in paired_children_)
          {

          }
          break;
      }
    }

    /**
     * @brief Removes a tab control when it has no more children and ensures that the last child of the dockpanel always takes the remaining space.
     * @param[in] sender (object) The sulphur.editor.controls.DynamicDockTabControl whose item collection has changed.
     * @param[in] e (EventArgs) Arguments passes with the event.
     */
    private void TabItemsChanged(object sender, EventArgs e)
    {
      DynamicDockTabControl control = sender as DynamicDockTabControl;
      if (control.Items.Count == 0)
      {
        foreach (Pair p in paired_children_)
        {
          if (p.element == control)
          {
            RemovePair(p);
            ((FrameworkElement)Children[Children.Count - 1]).Width = double.NaN;
            ((FrameworkElement)Children[Children.Count - 1]).Height = double.NaN;
            UpdateSplitters();
            break;
          }
        }
      }
    }

    /**
     * @brief Static constructor used to setup the WPF metadata of the control.
     */
    static DynamicDock()
    {
      DefaultStyleKeyProperty.OverrideMetadata(typeof(DynamicDock), new FrameworkPropertyMetadata(typeof(DynamicDock)));
    }

    /**
     * @brief Constructor that initializes all private variables.
     */
    public DynamicDock()
    {
      paired_children_ = new List<Pair>();
    }

    /**
     * @brief Removes a pair from the item collection.
     * @param[in] pair (sulphur.editor.controls.DynamicDock.Pair) Pair to remove from the item collection.
     */
    void RemovePair(Pair pair)
    {
      if (pair.splitter == null)
      {
        Children.Remove(pair.element);
      }
      else
      {
        Children.Remove(pair.splitter);
        Children.Remove(pair.element);
      }
      paired_children_.Remove(pair);
    }

    /**
     * @brief Updates the splitters in the pairs. If the pair contains the last element in the items collection from the panel the splitter will be removed.
     * @see sulphur.editor.controls.DynamicDockSplitter.
     */
    void UpdateSplitters()
    {
      if (paired_children_.Count == 1)
      {
        Children.Remove(paired_children_[0].splitter);
        paired_children_[0].splitter = null;
      }

      foreach (Pair pair in paired_children_)
      {
        if (pair.splitter == Children[Children.Count - 1])
        {
          Children.RemoveAt(Children.Count - 1);
          pair.splitter = null;
        }
      }
    }
  }

  /**
   * @class sulphur.editor.control.DynamicDockAdorner : Adorner
   * @brief Adorner that is displayed over the dock areas to indicate the possible docking options.
   * @author Stan Pepels
   */
  public class DynamicDockAdorner : Adorner
  {
    private UIElementCollection children; //!< Visual children of the adorner. These children represent the different docking options.

    /**
     * @brief Calculates the final size of the adorner.
     * @param[in] constraints (Size) The constraints the adorner must to adhere to.
     * @return (Size) Final size of the adorner.
     */
    protected override Size MeasureOverride(Size constraint)
    {
      AdornedElement.Measure(constraint);
      return AdornedElement.RenderSize;
    }

    /**
     * @brief calculates the positions and sizes of the child elements using the final size.
     * @param[in] final_size (Size) max size of the adorner.
     * @return (Size) actually required size of the adorner as caclulated from its visual children positions and sizez.
     */
    protected override Size ArrangeOverride(Size final_size)
    {
      DynamicDockTab c = AdornedElement as DynamicDockTab;

      children[0].Arrange(new Rect(final_size.Width * 0.5f - 25, final_size.Height * 0.5f - 25, 50, 50));
      return final_size;
    }

    /**
     * @brief Current amount of visual children of this adorner.
     */
    protected override int VisualChildrenCount
    {
      get
      {
        return children.Count;
      }
    }

    /**
     * @brief Get a visual child by index
     * @param[in] index (int) Index of the child to retrieve
     * @return (Visual) The visual at the given index in the visual collection
     * @see sulphur.editor.controls.DynamicDockAdorner.children
     */
    protected override Visual GetVisualChild(int index)
    {
      if (index != 0)
      {
        throw new ArgumentOutOfRangeException();
      }
      return children[index];
    }

    /**
     * @brief Constructor that initializes the all children of the adorner.
     * @param[in] element (UIElement) element to be adorned by this adorner.
     */
    public DynamicDockAdorner(UIElement element) :
      base(element)
    {
      Button t = new Button();
      children = new UIElementCollection(this, this);
      t.Width = 50;
      t.Height = 50;
      t.Content = "1";
      children.Add(t);
    }

    /**
     * @brief Tries to dock a tabwindow to the control that is adorned by this adorner.
     * @param[in] window (sulphur.editor.controls.TabWindow) window to be docked.
     * @return (bool) True if window was docked, false otherwise.
     */
    public bool DockWindow(TabWindow window)
    {

      Point top_left = children[0].TranslatePoint(new Point(0, 0), this);
      top_left = PointToScreen(top_left);
      Point bottom_right = new Point(top_left.X + children[0].RenderSize.Width, top_left.Y + children[0].RenderSize.Height);

      Point mouse = native.Win32.GetMousePosition();

      bool result = mouse.X > top_left.X && mouse.X < bottom_right.X && mouse.Y > top_left.Y && mouse.Y < bottom_right.Y;
      if (result == false)
      {
        return result;
      }

      List<object> collection = window.GetTabs();
      (window.Content as DynamicDockTabControl).Items.Clear();
      foreach (object item in collection)
      {
        (AdornedElement as DynamicDockTabControl).Items.Add(item);
        (AdornedElement as DynamicDockTabControl).SelectedItem = item;
      }
      window.Close();

      return result;
    }
  }
}
