using sulphur.editor.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace sulphur.editor.controls.inspector
{
  /// <summary>
  /// Interaction logic for SliderInspector.xaml
  /// </summary>
  public partial class SliderInspector : UserControl
  {
    private VarRef var_;

    public SliderInspector()
    {
      InitializeComponent();
    }

    public SliderInspector(VarRef var)
    {
      InitializeComponent();
      var_ = var;
      FromVariable();
    }

    private bool IsTextAllowed(string text)
    {
      Regex regex = new Regex(@"[^\d.\-\*\^\+\/\(\)]+"); //regex that matches disallowed text
      return !regex.IsMatch(text);
    }
    
    private void SliderText_PreviewTextInput(object sender, TextCompositionEventArgs e)
    {
      e.Handled = !IsTextAllowed(e.Text);
    }

    private void SliderText_Pasting(object sender, DataObjectPastingEventArgs e)
    {
      if (!e.DataObject.GetDataPresent(typeof(String)))
        e.CancelCommand();
      
      String text = (String)e.DataObject.GetData(typeof(String));
      if (!IsTextAllowed(text))
        e.CancelCommand();
    }

    private void SliderText_LostFocus(object sender, RoutedEventArgs e)
    {
      SliderValue.Value = (float)StringToFormula.Eval(SliderText.Text);
      SliderText.Text = SliderValue.Value.ToString("0.00");
      UpdateVariable();
    }

    private void SliderText_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
        SliderText_LostFocus(sender, null);
    }

    private void SliderValue_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
    {
      if(SliderText == null)
      {
        return;
      }

      SliderText.Text = e.NewValue.ToString("0.00");
      UpdateVariable();
    }

    private void FromVariable()
    {
      if (var_ == null || var_.Value == null)
      {
        return;
      }

      SliderValue.Value = (float)var_.Value;
      SliderText.Text = SliderValue.Value.ToString("0.00");
    }

    private void UpdateVariable()
    {
      if (var_ == null)
      {
        return;
      }

      var_.Value = SliderValue.Value;
    }
  }
}
