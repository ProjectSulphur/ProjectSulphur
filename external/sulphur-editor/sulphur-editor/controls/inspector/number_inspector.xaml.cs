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
  /// Interaction logic for NumberInspector.xaml
  /// </summary>
  public partial class NumberInspector : UserControl
  {
    private VarRef var_;

    public NumberInspector()
    {
      InitializeComponent();
    }

    public NumberInspector(VarRef var)
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

    private void TextBox1_PreviewTextInput(object sender, TextCompositionEventArgs e)
    {
      e.Handled = !IsTextAllowed(e.Text);
    }

    private void TextBox1_Pasting(object sender, DataObjectPastingEventArgs e)
    {
      if (!e.DataObject.GetDataPresent(typeof(String)))
        e.CancelCommand();

      String text = (String)e.DataObject.GetData(typeof(String));
      if (!IsTextAllowed(text))
        e.CancelCommand();
    }

    private void TextBox1_LostFocus(object sender, RoutedEventArgs e)
    {
      TextBox1.Text = StringToFormula.Eval(TextBox1.Text).ToString("0.00");
      UpdateVariable();
    }

    private void FromVariable()
    {
      if (var_ == null || var_.Value == null)
      {
        return;
      }

      TextBox1.Text = ((float)var_.Value).ToString("0.00");
    }

    private void UpdateVariable()
    {
      if (var_ == null)
      {
        return;
      }

      var_.Value = TextBox1.Text;
    }

    private void TextBox1_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
        TextBox1_LostFocus(sender, null);
    }
  }
}
