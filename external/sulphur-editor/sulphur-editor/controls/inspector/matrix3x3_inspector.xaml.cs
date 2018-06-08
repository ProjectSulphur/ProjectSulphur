using sulphur.editor.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
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
  /// Interaction logic for Matrix3x3Inspector.xaml
  /// </summary>
  public partial class Matrix3x3Inspector : UserControl
  {
    private VarRef var_;

    public Matrix3x3Inspector()
    {
      InitializeComponent();
    }

    public Matrix3x3Inspector(VarRef var)
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

    private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
    {
      e.Handled = !IsTextAllowed(e.Text);
    }

    private void TextBox_Pasting(object sender, DataObjectPastingEventArgs e)
    {
      if (!e.DataObject.GetDataPresent(typeof(String)))
      {
        e.CancelCommand();
      }

      String text = (String)e.DataObject.GetData(typeof(String));
      if (!IsTextAllowed(text))
      {
        e.CancelCommand();
      }
    }

    private void TextBox_LostFocus(object sender, RoutedEventArgs e)
    {
      TextBox t = (TextBox)sender;
      t.Text = StringToFormula.Eval(t.Text).ToString("0.00");
      UpdateVariable();
    }

    private void FromVariable()
    {
      if (var_ == null || var_.Value == null)
      {
        return;
      }

      /*Matrix3x3 mat = (Matrix3x3)var_.Value;

      M00.Text = mat.M11.ToString("0.00"); M01.Text = mat.M12.ToString("0.00"); M02.Text = mat.M13.ToString("0.00");
      M10.Text = mat.M21.ToString("0.00"); M11.Text = mat.M22.ToString("0.00"); M12.Text = mat.M23.ToString("0.00");
      M20.Text = mat.M31.ToString("0.00"); M21.Text = mat.M32.ToString("0.00"); M22.Text = mat.M33.ToString("0.00");*/
    }

    private void UpdateVariable()
    {
      if (var_ == null)
      {
        return;
      }

      /*var_.Value = new Matrix3x3(
        float.Parse(M00.Text), float.Parse(M01.Text), float.Parse(M02.Text),
        float.Parse(M10.Text), float.Parse(M11.Text), float.Parse(M12.Text),
        float.Parse(M20.Text), float.Parse(M21.Text), float.Parse(M22.Text));*/
    }

    private void TextBox_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.Key == Key.Enter)
      {
        TextBox_LostFocus(sender, null);
      }
    }
  }
}
