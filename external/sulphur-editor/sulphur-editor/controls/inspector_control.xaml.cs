using sulphur.editor.controls.inspector;
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

namespace sulphur.editor.controls
{
  /// <summary>
  /// Interaction logic for inspector_control.xaml
  /// </summary>
  public partial class InspectorControl : UserControl
  {
    public InspectorControl()
    {
      InitializeComponent();

      ClearInspectors();
      AddInspector("Image:", InspectorTypes.kImage, new VarRef(() => null, val => { }));
      AddInspector("Checkbox:", InspectorTypes.kCheckbox, new VarRef(() => null, val => { }));
      AddInspector("Float:", InspectorTypes.kSlider, new VarRef(() => null, val => { }));
    }

    public void ClearInspectors()
    {
      grid1.RowDefinitions.Clear();
      DefineRows(1);
      grid1.Children.Clear();
    }

    public void AddInspector(string label, InspectorTypes type, VarRef var)
    {
      int row = grid1.RowDefinitions.Count - 1; // There is always one row in the designer to fill the item with

      switch (type) {
        case InspectorTypes.kMatrix3x3:
          DefineRows(3);
          break;
        case InspectorTypes.kMatrix4x4:
          DefineRows(4);
          break;
        default:
          DefineRows(1);
          break;
      }
      
      CreateLabelForRow(label, row);
      CreateInspectorForRow(type, var, row);
    }
    
    private void DefineRows(int amount)
    {
      for (int i = 0; i < amount; ++i)
      {
        RowDefinition r = new RowDefinition
        {
          Height = new GridLength(30)
        };

        grid1.RowDefinitions.Add(r);
      }
    }

    private void CreateLabelForRow(string value, int row)
    {
      Label label = new Label
      {
        Content = value,
        Margin = new Thickness(5, 7, 5, 7),
        Padding = new Thickness(5, 0, 5, 0),
        VerticalAlignment = VerticalAlignment.Center,
        Height = 16
      };
      Grid.SetRow(label, row);
      grid1.Children.Add(label);
    }

    private void CreateInspectorForRow(InspectorTypes type, VarRef var, int row)
    {
      UserControl control = Inspectors.CreateInspector(type, var);

      switch (type)
      {
        case InspectorTypes.kMatrix4x4:
          Grid.SetRowSpan(control, 4);
          break;
        case InspectorTypes.kMatrix3x3:
          Grid.SetRowSpan(control, 3);
          break;
      }

      Grid.SetRow(control, row);
      Grid.SetColumn(control, 1);
      grid1.Children.Add(control);
    }
  }
}
