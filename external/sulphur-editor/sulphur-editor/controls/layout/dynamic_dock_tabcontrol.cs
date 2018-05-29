using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Input;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Windows.Controls.Primitives;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.DynamicDockTabControl : TabControl
   * @brief Class used by the sulphur.editor.controls.DynamicDock to group together controls.
   * @see sulphur.editor.controls.DynamicDock
   * @author Stan Pepels
   */
  public class DynamicDockTabControl : TabControl
  {
    private Point start_position_; //!< Starting point of the mouse when a drag is starting.
    public bool allow_empty_; //!< Bool indicating if the control is allowd to be empty. If this is set to false remobing the last child is not allowed.
    private bool mouse_clicked_ = false; //!< Bool indicating if the mouse was pressed.
    private TabPanel tab_panel_; //!< The tabpanel used to display the available tabs.

    public event EventHandler items_changed; //!< Event fired when a tab is added or removed.

    /**
     * @brief Default constructor
     */
    public DynamicDockTabControl() { }

    /**
     * @brief Contructor that sets up the control to be used by a dynamic dock or tab window.
     * @param[in] allow_empty (bool) Bool indicating if the control is allowed to be empty.
     */
    public DynamicDockTabControl(bool allow_empty)
    {
      PreviewMouseLeftButtonDown += HandleMouseLeftButtonDown;
      MouseMove += HandleMouseMove;
      allow_empty_ = allow_empty;
      AllowDrop = true;
    }

    /**
     * @brief OnApplyTemplate callback.
     * @see https://msdn.microsoft.com/nl-nl/library/system.windows.frameworkelement.onapplytemplate(v=vs.110).aspx
     */
    public override void OnApplyTemplate()
    {
      base.OnApplyTemplate();
      tab_panel_ = Template.FindName("HeaderPanel", this) as TabPanel;
    }

    /**
     * @brief OnItemsChanged callback. This callback sets the Width and Height properties to be the same as the control width and height properties.
     * @param[in] e (NotifyCollectionChangedEventArgs) Arguments passed when the event was fired.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.controls.itemscontrol.onitemschanged(v=vs.110).aspx
     */
    protected override void OnItemsChanged(NotifyCollectionChangedEventArgs e)
    {
      EventHandler handler = items_changed;
      handler?.Invoke(this, new EventArgs());

      if(e.NewItems == null)
      {
        return;
      }
      foreach(TabItem item in e.NewItems)
      {
        FrameworkElement control = item.Content as FrameworkElement;
        control.HorizontalAlignment = HorizontalAlignment.Stretch;
        control.VerticalAlignment = VerticalAlignment.Stretch;
        if(double.IsNaN(Width) || Width < control.Width)
        {
          Width = control.Width;
          control.Width = double.NaN;
        }
        if (double.IsNaN(Height) || Height < control.Height)
        {
          Height = control.Height;
          control.Height = double.NaN;
        }
      }
    }

    /**
     * @brief OnMouseLeftButtonUp callback.
     * @brief e (MouseButtonEventArgs)  Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.uielement.onmouseleftbuttonup(v=vs.110).aspx
     */
    protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
    {
      base.OnMouseLeftButtonUp(e);
      mouse_clicked_ = false;
    }

    /**
     * @brief Called when the conditions for a drag are met. 
     * This removes the currently selected tab from the control and puts it into a seperate window.
     * @param[in] item (DynamicDockTab) Item to be removed.
     * @see sulphur.editor.controls.TabWindow.
     */
    private void StartDrag(DynamicDockTab item)
    {
      if(Items.Count == 1 && allow_empty_ == false)
      {
        return;
      }

      Items.Remove(item);
      if(Items.Count != 0)
      {
        SelectedIndex = Items.Count - 1;
      }
      TabWindow window = new TabWindow(item);
      SubscriptionHandler.Register(window);
      window.Show();
      window.Focus();
      window.DragMove();
    }

    /**
     * @brief Handles the MouseLeftButtonDown event. Sets the mouse start position.
     * @param[in] sender (object) sender of the event.
     * @param[in] e (MouseButtonEventArgs)Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.uielement.mouseleftbuttondown(v=vs.110).aspx
     */
    private void HandleMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      start_position_ = e.GetPosition(null);
      mouse_clicked_ = true;
    }

    /**
      * @brief Handles the MouseMove event. If the mouse is moved far enough form its starting position. A drag operation will be started.
      * @param[in] sender (object) sender of the event.
      * @param[in] e (MouseButtonEventArgs)Arguments passed with the event that contains information
      *  of the mouse from the time the event was fired.
      * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.mousemove(v=vs.110).aspx
      */
    private void HandleMouseMove(object sender, MouseEventArgs e)
    {
      if (e.LeftButton == MouseButtonState.Pressed && mouse_clicked_ == true)
      {
        Point position = e.GetPosition(null);
        if (Math.Abs(position.X - start_position_.X) > SystemParameters.MinimumHorizontalDragDistance ||
          Math.Abs(position.Y - start_position_.Y) > SystemParameters.MinimumVerticalDragDistance)
        {
          var tabItem = e.Source as DynamicDockTab;
          if (tabItem == null) return;
          StartDrag(tabItem);

        }
      }
      e.Handled = true;
    }

    /**
     * @brief Tries to add a sulphur.editor.controls.TabWindow content to this control.
     * @param[in] window (TabWindow) Window whose content has to be copied over to this control.
     * @return True if window was added, false otherwise.
     * @see sulphur.editor.controls.TabWindow.
     * @remark If succesfull the window added will be closed. 
     */
    public bool AddWindow(TabWindow window)
    {
      Point top_left = tab_panel_.TranslatePoint(new Point(0, 0), this);
      top_left = this.PointToScreen(top_left);
      Point bottom_right = new Point(top_left.X + tab_panel_.ActualWidth, top_left.Y + tab_panel_.ActualHeight);

      Point mouse = native.Win32.GetMousePosition();

      bool result = mouse.X > top_left.X && mouse.X < bottom_right.X && mouse.Y > top_left.Y && mouse.Y < bottom_right.Y;
      if (result == false)
      {
        return result;
      }

      List<object> collection = window.GetTabs();
      window.UnparentTabs();
      foreach (object item in collection)
      {
        Items.Add(item);
        SelectedItem = item;
      }
      window.Close();
      return true;
    }
  }
}
