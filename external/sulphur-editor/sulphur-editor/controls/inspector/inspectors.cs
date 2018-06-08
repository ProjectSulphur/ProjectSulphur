using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace sulphur.editor.controls.inspector
{
  /**
  *@enum sulphur.controls.inspector.InspectorTypes
  *@brief The type of inspector
  *@author Jelle de Haan
  */
  public enum InspectorTypes
  {
    kCheckbox,
    kColor,
    kCombobox,
    kImage,
    kNumber,
    kSlider,
    kVector2,
    kVector3,
    kVector4,
    kMatrix3x3,
    kMatrix4x4
  }

  /**
  *@class sulphur.controls.inspector.ComboBoxData
  *@brief Helper class for wrapping a variable so we can edit it using a ComboBoxInspector
  *@author Jelle de Haan
  */
  public class ComboBoxData
  {
    public List<string> Items { get; set; }
    public int SelectedItem { get; set; }
  }

  /**
  *@class sulphur.controls.inspector.VarRef
  *@brief Helper class for wrapping a variable so we can edit it
  *@author Jelle de Haan
  */
  public class VarRef
  {
    private Func<object> get_;
    private Action<object> set_;

    public VarRef(Func<object> @get, Action<object> @set)
    {
      get_ = @get;
      set_ = @set;
    }

    public object Value
    {
      get { return get_(); }
      set { set_(value); }
    }
  }

  public class Inspectors
  {
    /**
     *@class sulphur.controls.inspector.Inspectors
     *@brief Creates an inspector user control based on an InspectorTypes type
     *@param[in] type (InspectorTypes) The type of inspector to create
     *@author Jelle de Haan
     */
    public static UserControl CreateInspector(InspectorTypes type, VarRef var)
    {
      switch (type)
      {
        case InspectorTypes.kCheckbox:
          return new CheckBoxInspector(var);
        case InspectorTypes.kColor:
          return new ColorInspector(var);
        case InspectorTypes.kCombobox:
          return new ComboBoxInspector(var);
        case InspectorTypes.kImage:
          return new ImageInspector(var);
        case InspectorTypes.kNumber:
          return new NumberInspector(var);
        case InspectorTypes.kSlider:
          return new SliderInspector(var);
        case InspectorTypes.kVector2:
          return new Vector2Inspector(var);
        case InspectorTypes.kVector3:
          return new Vector3Inspector(var);
        case InspectorTypes.kVector4:
          return new Vector4Inspector(var);
        case InspectorTypes.kMatrix3x3:
          return new Matrix3x3Inspector(var);
        case InspectorTypes.kMatrix4x4:
          return new Matrix4x4Inspector(var);
        default:
          return new NumberInspector(var);
      }
    }
  }
}
