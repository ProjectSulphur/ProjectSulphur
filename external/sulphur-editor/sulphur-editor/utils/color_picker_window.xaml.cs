using System;
using System.Collections.Generic;
using System.Linq;
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

namespace sulphur.editor.utils
{
  /// <summary>
  /// Interaction logic for ColorPickerWindow.xaml
  /// </summary>
  public partial class ColorPickerWindow : Window
  {
    struct HSV
    {
      public double h { get; set; }
      public double s { get; set; }
      public double v { get; set; }
    };

    private HSV hsv = new HSV { s = 1.0, v = 1.0 };
    public Color SelectedColor { set; get; }

    public ColorPickerWindow()
    {
      InitializeComponent();
    }

    private void ColorPicker_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
    {
      SelectedColor = e.NewValue??Colors.Black;
    }

    private void ColorPickerCanvas_MouseDown(object sender, MouseButtonEventArgs e)
    {
      ColorPickerCanvas.CaptureMouse();
    }

    private Color HSVtoRGB(double h, double s, double v)
    {
      int i;
      double f, p, q, t;

      if (s <= 0)
      {
        // achromatic (grey)
        v = Math.Ceiling(v * 255.0);
        return Color.FromRgb((byte)v, (byte)v, (byte)v);
      }

      if (h >= 360.0) h = 0.0;
      h /= 60.0;
      i = (int)Math.Floor(h);
      f = h - i;
      p = v * (1.0 - s);
      q = v * (1.0 - (s * f));
      t = v * (1.0 - (s * (1.0 - f)));

      //Float to Byte
      v = Math.Ceiling(v * 255.0);
      t = Math.Ceiling(t * 255.0);
      p = Math.Ceiling(p * 255.0);
      q = Math.Ceiling(q * 255.0);

      switch (i)
      {
        case 0:
          return Color.FromRgb((byte)v, (byte)t, (byte)p);
        case 1:                
          return Color.FromRgb((byte)q, (byte)v, (byte)p);
        case 2:                
          return Color.FromRgb((byte)p, (byte)v, (byte)t);
        case 3:                
          return Color.FromRgb((byte)p, (byte)q, (byte)v);
        case 4:                
          return Color.FromRgb((byte)t, (byte)p, (byte)v);
        default:              
          return Color.FromRgb((byte)v, (byte)p, (byte)q);
      }
    }

    private void ColorPickerCanvas_MouseMove(object sender, MouseEventArgs e)
    {
      if (ColorPickerCanvas.IsMouseCaptured)
      {
        Point mousePos = e.GetPosition(ColorPickerCanvas);

        mousePos.X = Math.Max(Math.Min(mousePos.X, ColorPickerCanvas.Width), 0);
        mousePos.Y = Math.Max(Math.Min(mousePos.Y, ColorPickerCanvas.Height), 0);

        ColorPickerIconTransform.X = mousePos.X;
        ColorPickerIconTransform.Y = mousePos.Y;
        
        hsv.s = Math.Min(1.0f, Math.Max(0.0f, mousePos.X / ColorPickerCanvas.Width));
        hsv.v = 1.0f - Math.Min(1.0f, Math.Max(0.0f, mousePos.Y / ColorPickerCanvas.Height));

        SelectedColor = HSVtoRGB(hsv.h, hsv.s, hsv.v);
        ColorPreviewCanvas.Background = new SolidColorBrush(SelectedColor);
      }
    }

    private void ColorPickerCanvas_MouseUp(object sender, MouseButtonEventArgs e)
    {
      ColorPickerCanvas.ReleaseMouseCapture();
    }

    private void HueCanvas_MouseDown(object sender, MouseButtonEventArgs e)
    {
      HueCanvas.CaptureMouse();
    }

    private void HueCanvas_MouseMove(object sender, MouseEventArgs e)
    {
      if (HueCanvas.IsMouseCaptured)
      {
        Point mousePos = e.GetPosition(HueCanvas);

        mousePos.Y = Math.Max(Math.Min(mousePos.Y, HueCanvas.Height), 0);

        HuePickerIconTransform.Y = mousePos.Y;

        hsv.h = 360.0 - Math.Min(360.0, Math.Max(0.0f, 360.0 / HueCanvas.Height * mousePos.Y));

        SelectedColor = HSVtoRGB(hsv.h, hsv.s, hsv.v);

        ColorPreviewCanvas.Background = ColorPickerCanvasBackground.Fill = new SolidColorBrush(SelectedColor);
      }
    }

    private void HueCanvas_MouseUp(object sender, MouseButtonEventArgs e)
    {
      HueCanvas.ReleaseMouseCapture();
    }
  }
}
