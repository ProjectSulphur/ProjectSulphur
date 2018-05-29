using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.DynamicDockSplitter : Control
   * @brief Class that controls the size of an attached element. This control is used to split to controls in the sulphur.editor.controls.DynamicDock
   * and allows them to be resized dynamically.
   * @see sulphur.editor.controls.DynamicDock
   * @author Stan Pepels
   */
  public class DynamicDockSplitter : Control
  {
    Point start_pos_; //!< Position of the mouse when the user started dragging this control.
    FrameworkElement root_; //!< Element to be resized when the control is dragged around.
    double root_size_; //!< Size of the element that is controlled by the spliiter.
    bool horizontal_; //!< Bool that dictates if the dragging is vertical or horizontal
    public double thickness = 5; //!< Thickness of the splitter

    /**
     * @brief Static contructor that sets up the WPF metadata for this control type.
     */
    static DynamicDockSplitter()
    {
      DefaultStyleKeyProperty.OverrideMetadata(typeof(DynamicDockSplitter),
              new FrameworkPropertyMetadata(typeof(DynamicDockSplitter)));

      DockPanel.DockProperty.OverrideMetadata(typeof(DynamicDockSplitter),
          new FrameworkPropertyMetadata(Dock.Left, new PropertyChangedCallback(DockChanged)));
    }

    /**
     * @brief Constructor that intialize the root control.
     * @param[in] root (FrameworkElement) Element to be resized by this splitter.
     */
    public DynamicDockSplitter(FrameworkElement root)
    {
      root_ = root;
      UpdateThickness();
    }

    /**
     * @brief Callback to the DockChanged event 
     * @param[in] o (DependencyObject) Object whose dock property changed.
     * @param[in] e (DependencyPropertyChangedEventArgs) Arguments passed with the event
     * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.dockchanged(v=vs.110).aspx
     * @see https://msdn.microsoft.com/en-us/library/system.windows.dependencypropertychangedeventargs(v=vs.110).aspx
     * @see https://msdn.microsoft.com/en-us/library/system.windows.dependencyobject(v=vs.110).aspx
     * @remark This also updates the way the thickness is applied. (horizontal or vertical)
     */
    static void DockChanged(DependencyObject o, DependencyPropertyChangedEventArgs e)
    {
      ((DynamicDockSplitter)o).UpdateThickness();
    }

    /**
     * @brief Updates the way the thickness in applied. (horizontal or vertical)
     */
    void UpdateThickness()
    {
      Dock dock = DockPanel.GetDock(this);
      horizontal_ = dock == Dock.Top || dock == Dock.Bottom;

      if (horizontal_ == true)
      {
        Height = thickness;
        Width = double.NaN;
      }
      else
      {
        Height = double.NaN;
        Width = thickness;
      }
    }

    /**
     * @brief OnMouseDown event callback.
     * @param[in] e (MouseButtonEventArgs) Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.input.mousebuttoneventargs(v=vs.110).aspx
     * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.onmousedown(v=vs.110).aspx
     * @remark This function also captures the mouse on this control.
     */
    protected override void OnMouseDown(MouseButtonEventArgs e)
    {
      base.OnMouseDown(e);
      start_pos_ = e.GetPosition(Parent as IInputElement);
      if (horizontal_ == true)
      {
        root_size_ = root_.ActualHeight;
      }
      else
      {
        root_size_ = root_.ActualWidth;
      }
      CaptureMouse();
    }

    /**
     * @brief OnMouseEnter callback.
     * @param[in] e (MouseButtonEventArgs) Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.     
     * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.onmouseenter(v=vs.110).aspx
     */
    protected override void OnMouseEnter(MouseEventArgs e)
    {
      base.OnMouseEnter(e);
      if (horizontal_ == true)
      {
        Cursor = Cursors.SizeNS;
      }
      else
      {
        Cursor = Cursors.SizeWE;
      }
    }

    /**
     * @brief Allows hittests to occur on this control.
     * @param[in] parameters (PointHitTestParameters) Hit test parameters used to detect a hit on the control.
     * @return (PointHitTestResult) Result of the hit test.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.media.pointhittestparameters.aspx?PHPSESSID=tn8k5...
     */
    protected override HitTestResult HitTestCore(PointHitTestParameters parameters)
    {
      return new PointHitTestResult(this, parameters.HitPoint);
    }

    /**
     * @brief OnMouseMove callback.
     * @param[in] e (MouseButtonEventArgs) Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.
     * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.onmousemove(v=vs.110).aspx 
     */
    protected override void OnMouseMove(MouseEventArgs e)
    {
      base.OnMouseMove(e);
      if (IsMouseCaptured == false)
      {
        return;
      }
      Point curr_pos_ = e.GetPosition(Parent as IInputElement);
      Point diff = new Point(curr_pos_.X - start_pos_.X, curr_pos_.Y - start_pos_.Y);


      if (horizontal_ == true)
      {
        if (DockPanel.GetDock(this) == Dock.Bottom)
        {
          diff.Y *= -1;
        }
        root_size_ += diff.Y;
      }
      else
      {
        if (DockPanel.GetDock(this) == Dock.Right)
        {
          diff.X *= -1;
        }
        root_size_ += diff.X;
      }

      if (root_size_ <= 0.0d)
      {
        return;
      }


      if (horizontal_ == true)
      {
        root_.Height = root_size_;
        start_pos_ = new Point(start_pos_.X, curr_pos_.Y);
      }
      else
      {
        root_.Width = root_size_;
        start_pos_ = new Point(curr_pos_.X, start_pos_.Y);
      }
    }

    /**
     * @brief OnMouseUp callback.
     * @param[in] e (MouseButtonEventArgs) Arguments passed with the event that contains information
     *  of the mouse from the time the event was fired.
     *  @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.onmouseup(v=vs.110).aspx
     */
    protected override void OnMouseUp(MouseButtonEventArgs e)
    {
      base.OnMouseUp(e);
      ReleaseMouseCapture();
    }
  }
}
