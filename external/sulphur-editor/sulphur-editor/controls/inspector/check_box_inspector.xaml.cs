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

namespace sulphur.editor.controls.inspector
{
  /// <summary>
  /// Interaction logic for CheckboxInspector.xaml
  /// </summary>
  public partial class CheckBoxInspector : UserControl
  {
    private VarRef var_;

    public CheckBoxInspector()
    {
      InitializeComponent();
    }

    public CheckBoxInspector(VarRef var)
    {
      InitializeComponent();
      var_ = var;
      FromVariable();
    }

    private void CheckBox_Checked(object sender, RoutedEventArgs e)
    {
      UpdateVariable(true);
    }

    private void CheckBox_Unchecked(object sender, RoutedEventArgs e)
    {
      UpdateVariable(false);
    }

    private void FromVariable()
    {
      if (var_ == null || var_.Value == null)
      {
        return;
      }

      checkbox1.IsChecked = (bool)var_.Value;
    }

    private void UpdateVariable(bool value)
    {
      if (var_ == null)
      {
        return;
      }

      var_.Value = value;
    }
  }
}
