using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.IO;

namespace sulphur.editor
{
  /**
   * @class sulphur.editor.EditBoxRenamedEventArgs : EventArgs
   * @brief Event arguments passed with an EditboxRenamed event.
   * @author Stan Pepels.
   */
  public class EditBoxRenamedEventArgs : EventArgs
  {
    /**
     * @brief Constructor used to intialzie the read only fields.
     * @param[in] old_text (string) Text before the editbox text was changed.
     * @param[in] new_text (string) New Text value of the editbox.
     */
    public EditBoxRenamedEventArgs(string old_text, string new_text)
    {
      old_value = old_text;
      new_value = new_text;
    }
    public readonly string old_value; //!< Text before the editbox text was changed.
    public readonly string new_value; //!< New Text value of the editbox.
  }

  public delegate void EditBoxCallback(object sender, EditBoxRenamedEventArgs e); //!< Callback / event declaration for a renamed event of an editbox.

  namespace controls
  {
    /**
     * @class sulphur.editor.controls. EditBox : System.Windows.Controls.Control
     * @brief Custom control allowing textblocks content to be edited via textbox. This control can come in handy if renaming of an object is needed.
     * @remark For examples see asset_browser_control.xaml.
     * @author Stan Pepels
     */
    public class EditBox : Control
    {
      public static readonly DependencyProperty TextProperty =
        DependencyProperty.Register("Text",
          typeof(string),
          typeof(EditBox),
          new PropertyMetadata("")); //!< Property backed by the WPF property system. This allows for binding expressions to be used in this property. This property holds the text value of the textbox.

      public static readonly DependencyProperty AllowEditProperty =
        DependencyProperty.Register("AllowEdit",
          typeof(bool),
          typeof(EditBox),
          new PropertyMetadata(true)); //!< Property backed by the WPF property system. This allows for binding expressions to be used in this property. This property can be used to turn the edit features on and of.

      public static readonly DependencyProperty IsEditingProperty =
        DependencyProperty.Register("IsEditing",
          typeof(bool),
          typeof(EditBox),
          new PropertyMetadata(false)); //!< Property backed by the WPF property system. This allows for binding expressions to be used in this property. This property can be used to poll the current state of the control.

      public static readonly DependencyProperty RenamedCallbackProperty =
        DependencyProperty.Register("RenamedCallback",
          typeof(utils.CommandHandler),
          typeof(EditBox),
          new FrameworkPropertyMetadata(null)); //!< Property backed by the WPF property system. This allows for binding expressions to be used in this property. This property can be used to perform an action when the text in the editbox has changed.
      
      /**
      * @brief Value of Renamed callback. This property can be bound to since it is backed by the WPF property system.
      * @see sulphur.editor.controls.EditBox.TextProperty
      * @see https://docs.microsoft.com/en-us/dotnet/framework/wpf/advanced/dependency-properties-overview
      */
      public utils.CommandHandler RenamedCallback
      {
        get { return (utils.CommandHandler)GetValue(RenamedCallbackProperty); }
        set { SetValue(RenamedCallbackProperty, value); }
      }

      /**
       * @brief Value of the textbox as currently displayed. This property can be bound to since it is backed by the WPF property system.
       * @see sulphur.editor.controls.EditBox.TextProperty
       * @see https://docs.microsoft.com/en-us/dotnet/framework/wpf/advanced/dependency-properties-overview
       */
      public string Text
      {
        get { return (string)GetValue(TextProperty); }
        set { SetValue(TextProperty, value); }
      }

      /**
       * @brief Used to enable or disable the textblock editing features. If disabled tis conrol will work like a regular TextBlock. This property can be bound to since it is backed by the WPF property system.
       * @see sulphur.editor.controls.EditBox.allow_edit_property
       * @see https://docs.microsoft.com/en-us/dotnet/framework/wpf/advanced/dependency-properties-overview
       */
      public bool AllowEdit
      {
        get { return (bool)GetValue(AllowEditProperty); }
        set { SetValue(AllowEditProperty, value); }
      }

      /**
      * @brief Used to poll the current state of the control. This property can be bound to since it is backed by the WPF property system.
      * @see sulphur.editor.controls.EditBox.allow_edit_property
      * @see https://docs.microsoft.com/en-us/dotnet/framework/wpf/advanced/dependency-properties-overview
      */
      public bool IsEditing
      {
        get { return (bool)GetValue(IsEditingProperty); }
        set { SetValue(IsEditingProperty, value); }
      }

      private EditBoxAdorner adorner_; //!< Adorner used to edit the text on the textblock. This adorner is displayed over the textblock.
      private TextBlock textblock_; //!< Textblock that displays the value in the Text property. This control servers as a binding target.
      private TextBox textbox_; //!< Textbox uses by the adorner to capture input so the text in the textblock can be changed.
      private AdornerLayer layer_; //!< Layer the adorner should live on when enables. Each EditBox control has its own layer created by WPF.
      private Control parent_; //!< Parent container of this control. This is only used when the EditBox is part of another collection such as a Treeview or ListBox. This Control will point to the Container of the EditBox. For example in case of the treeview the underlying type will be type of TreeviewItem
      private string old_value_; //!< Value of the Textblock before the editing begins.

      /**
       * @brief Static constructor used to setup the Metadata for this control used by the WPF system.
       */
      static EditBox()
      {
        DefaultStyleKeyProperty.OverrideMetadata(typeof(EditBox), new FrameworkPropertyMetadata(typeof(EditBox)));
      }

      /**
       * @brief Empty Constructor required to instantiate the control using xaml.
       */
      public EditBox()
      { }

      /**
       * @brief Performs an upwards search on the visual tree to find an item of a specified type.
       * @param[in] start (System.Windows.DependencyObject) Starting point of the search.
       * @param[in] type (System.Type) Type of the object to find.
       * @return (System.Windows.DependencyObject) First object found that is of the type specified by the "type" input parameter.
       */
      private DependencyObject GetDependencyObjectFromVisualTree(DependencyObject start, Type type)
      {
        DependencyObject parent = start;
        while (parent != null)
        {
          if (type.IsInstanceOfType(parent))
          {
            break;
          }
          else
          {
            parent = VisualTreeHelper.GetParent(parent);
          }
        }
        return parent;
      }

      /**
       * @brief OnApplyTemplate override. This function sets up the bindings between the textblock_, textbox_ controls and the Text property.
       * @see https://msdn.microsoft.com/nl-nl/library/system.windows.frameworkelement.onapplytemplate(v=vs.110).aspx
       * @remark If no adornerlayer can be found an error will be thrown which can be viewed in the logger. This will also stop the adorner and TextBox from being created meaning this control will be used like a normal textblcok control.
       */
      public override void OnApplyTemplate()
      {
        textblock_ = Template.FindName("textblock", this) as TextBlock;
        layer_ = AdornerLayer.GetAdornerLayer(textblock_);

        Binding widthbinding = new Binding();
        widthbinding.Source = this;
        widthbinding.Path = new PropertyPath("Width");
        textblock_.SetBinding(TextBlock.WidthProperty, widthbinding);

        Binding textbinding = new Binding();
        textbinding.Source = this;
        textbinding.Path = new PropertyPath("Text");
        textblock_.SetBinding(TextBlock.TextProperty, textbinding);

        if (layer_ == null)
        {
          Log.Write(Log.Verbosity.kFatal, "EditBox: AdornerLayer not found");
        }
        else
        {
          textbox_ = new TextBox();
          textbox_.LostKeyboardFocus += HandleLostKeyboardFocus;
          textbox_.KeyDown += HandleKeyDown;
          adorner_ = new EditBoxAdorner(textblock_, textbox_);
        }

        ItemsControl container = GetDependencyObjectFromVisualTree(this, typeof(ItemsControl)) as ItemsControl;
        Control item = container.ItemContainerGenerator.ContainerFromItem(this) as EditBox;
        parent_ = item == null ? container : item;

        if (parent_ != null)
        {
          parent_.LostKeyboardFocus += HandleLostKeyboardFocus;
          parent_.MouseDoubleClick += Control_MouseDoubleClick;
          parent_.KeyDown += Control_KeyDown;
        }
      }

      /**
       * @brief OnMouseLeftButtonDown override. Enables the adorner to allow the textblock to be edited.
       * @see https://msdn.microsoft.com/en-us/library/system.windows.uielement.onmouseleftbuttondown(v=vs.110).aspx
       * @remark If editing is disabled this function does nothing.
       */
      protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
      {
        if (e.ClickCount != 2 || AllowEdit == false)
        {
          return;
        }

        if (AllowEdit == true)
        {
          EnableAdorner();
        }
        e.Handled = true;
      }

      /**
      * @brief OnKeyDown overide. Handles the F2 key press by switching to edit mode if enabled.
      * @param[in] e (KeyEventArgs) Argument containing informations about the key pressed.
      * @see https://msdn.microsoft.com/en-us/library/system.windows.forms.control.onkeydown(v=vs.110).aspx
      */
      protected override void OnKeyDown(KeyEventArgs e)
      {
        if (AllowEdit == false)
        {
          return;
        }

        if (e.Key == Key.F2)
        {
          EnableAdorner();
        }
      }

      /**
      * @brief Focuses the adorner if in edit mode. This event handler is required because if the editbox is part of a collection the F2 key will focus on the item container immidiatly disabling the edit mode.
      * @param[in] sender (object) Original sender of the event.
      * @param[in] e (KeyEventArgs) Argument containing informations about the key pressed.
      * @remark If the control is not part of a control that contains a collection of items this fuction will not be called.
      */
      private void Control_KeyDown(object sender, KeyEventArgs e)
      {
        if (AllowEdit == false)
        {
          return;
        }
        if (e.Key == Key.F2)
        {
          EnableAdorner();
          FocusAdorner();
        }

        e.Handled = true;
      }

      /**
       * @brief Focuses the adorner if in edit mode. This event handler is required because if the editbox is part of a collection the double click event will focus on the item container immidiatly disabling the edit mode.
       * @param[in] sender (object) Original sender of the event.
       * @param[in] e (MouseButtonEventArgs) Argument containing informations about the state of the mouse when the event was fired.
       * @remark If the control is not part of a control that contains a collection of items this fuction will not be called.
       */
      private void Control_MouseDoubleClick(object sender, MouseButtonEventArgs e)
      {
        FocusAdorner();
        e.Handled = true;
      }

      /**
       * @brief Handles LostKeyboardFocus event for both the textblock on the adorner and the item container if any.
       * @param[in] sender (object) Original sender of the event.
       * @param[in] e (KeyboardFocusChangedEventArgs) Argument passed with the event.
       */
      private void HandleLostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
      {
        if (e.NewFocus == parent_ || e.NewFocus == textbox_)
        {
          return;
        }

        DisableAdorner();
      }

      /**
       * @brief Handles the Enter key press by disabling the edit mode.
       * @param[in] sender (object) Original sender of the event.
       * @param[in] e (KeyEventArgs) Argument containing informations about the key pressed.
       */
      private void HandleKeyDown(object sender, KeyEventArgs e)
      {
        if (IsEditing == false)
        {
          return;
        }
        else if (e.Key == Key.Enter)
        {
          DisableAdorner();
          e.Handled = true;
        }
      }

      /**
       * @bried Puts the current focus on the adorner textbox.
       * @remark This function is called after the control has switched into edit mode.
       */
      void FocusAdorner()
      {
        if (IsEditing == true)
        {
          textbox_.CaptureMouse();
          textbox_.SelectAll();
          textbox_.Focus();
        }
      }

      /**
       * @brief Enables the adorner to allow for editing of the textblock.
       */
      void EnableAdorner()
      {
        if (IsEditing == false)
        {
          IsEditing = true;
          layer_.Add(adorner_);
          old_value_ = textbox_.Text;
        }
      }

      /**
       * @brief Disables the adorner.
       * @remark If at the time of this function call the text of the textbox is empty. The text value will be restored to the value from before the edit started.
       */
      void DisableAdorner()
      {
        if (IsEditing == true)
        {
          if (textbox_.Text.Length == 0)
          {
            textbox_.Text = old_value_;
          }
          textbox_.ReleaseMouseCapture();
          layer_.Remove(adorner_);
          EditBoxRenamedEventArgs args = new EditBoxRenamedEventArgs(old_value_, textbox_.Text);

          if (args.old_value != args.new_value)
          {
            RenamedCallback?.Execute(args);
          }
          IsEditing = false;
        }
      }
    }

    /**
     * @class sulphur.editor.controls.EditBoxAdorner : Adorner
     * @brief Displays a TextBox over an EditBox. This textbox is used to edit the editbox value.
     * @author Stan Pepels
     */
    public class EditBoxAdorner : Adorner
    {
      private FrameworkElement child_; //!< The textbox to be displayed by the adorner.
      public FrameworkElement child //!< Child property used for getting and setting the child value.
      {
        get { return child_; }
        set
        {
          if (child_ != null)
          {
            RemoveVisualChild(child_);
          }
          child_ = value;
          if (child_ != null)
          {
            AddVisualChild(child_);
          }
        }
      }

      /**
       * @brief Gets the number of visual children on this adorner.
       */
      protected override int VisualChildrenCount
      {
        get
        {
          return 1;
        }
      }

      /**
       * @brief Gets the visual child at an index.
       * @param[in] index (int) Index of the child to be retrieved.
       * @return (Visual) The visual child at the index passed.
       */
      protected override Visual GetVisualChild(int index)
      {
        if (index != 0)
        {
          throw new ArgumentOutOfRangeException();
        }
        return child_;
      }

      /**
     * @brief Calculates the final size of the adorner.
     * @param[in] constraints (Size) The constraints the adorner must to adhere to.
     * @return (Size) Final size of the adorner.
     */
      protected override Size MeasureOverride(Size constraint)
      {
        AdornedElement.Measure(constraint);
        textbox_.Measure(constraint);
        return new Size(AdornedElement.DesiredSize.Width + 15, textbox_.DesiredSize.Height);
      }

      /**
       * @brief calculates the positions and sizes of the child elements using the final size.
       * @param[in] final_size (Size) max size of the adorner.
       * @return (Size) actually required size of the adorner as caclulated from its visual children positions and sizez.
       */
      protected override Size ArrangeOverride(Size final_size)
      {
        textbox_.Arrange(new Rect(0, 0, final_size.Width, final_size.Height));
        return final_size;
      }

      /**
       * @brief Initializes the adorner with a textbox.
       * @param[in] element (UIElement) Element this adorner belongs to.
       * @param[in] textbox (TextBox) The textbox to be displayed by the adorner.
       */
      public EditBoxAdorner(UIElement element, TextBox textbox) :
        base(element)
      {
        textbox_ = textbox;
        Binding binding = new Binding("Text");
        binding.Mode = BindingMode.TwoWay;
        binding.Source = AdornedElement;
        binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
        textbox_.SetBinding(TextBox.TextProperty, binding);

        canvas = new Canvas();
        canvas.Children.Add(textbox_);
        child = canvas;
      }

      TextBox textbox_; //!< The textbox displayed over the editbox. Used for changing the editbox text value.
      Canvas canvas; //!< Canvas to display the textbox on.
    }
  }
}
