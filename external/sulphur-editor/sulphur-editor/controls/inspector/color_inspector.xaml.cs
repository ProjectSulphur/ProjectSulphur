using sulphur.editor.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
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
  /// Interaction logic for ColorInspector.xaml
  /// </summary>
  public partial class ColorInspector : UserControl
  {
    private VarRef var_;

    public ColorInspector()
    {
      InitializeComponent();
    }

    public ColorInspector(VarRef var)
    {
      InitializeComponent();
      var_ = var;
      FromVariable();
    }

    private void ColorPickerButton_Click(object sender, RoutedEventArgs e)
    {
      
    }

    private void Canvas_MouseUp(object sender, MouseButtonEventArgs e)
    {
      ColorPickerWindow colorPicker = new ColorPickerWindow
      {
        SelectedColor = SelectedColor.Color
      };

      colorPicker.ShowDialog();

      SelectedColor.Color = colorPicker.SelectedColor;
      ColorStop2.Color = SelectedColor.Color;
    }

    private void FromVariable()
    {
      if (var_ == null || var_.Value == null)
      {
        return;
      }

      Vector4 color = (Vector4)var_.Value;
      Vector4.Normalize(color);
      color *= 255;

      SelectedColor.Color = Color.FromArgb((byte)color.W, (byte)color.X, (byte)color.Y, (byte)color.Z);
    }

    private void UpdateVariable()
    {
      if (var_ == null)
      {
        return;
      }

      Vector4 color = new Vector4(
        SelectedColor.Color.R,
        SelectedColor.Color.G,
        SelectedColor.Color.B,
        SelectedColor.Color.A);

      color *= 0.00392156862f;

      var_.Value = color;
    }
  }
}
