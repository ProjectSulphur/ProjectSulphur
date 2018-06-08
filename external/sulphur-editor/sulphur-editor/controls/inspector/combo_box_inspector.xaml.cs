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
  /// Interaction logic for ComboBoxInspector.xaml
  /// </summary>
  public partial class ComboBoxInspector : UserControl
  {
    private VarRef var_;

    public ComboBoxInspector()
    {
      InitializeComponent();
    }

    public ComboBoxInspector(VarRef var)
    {
      InitializeComponent();
      var_ = var;
    }
  }
}
