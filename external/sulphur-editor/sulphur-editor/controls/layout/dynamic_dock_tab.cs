using System.Windows.Controls;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.DynamicDockTab : TabItem
   * @brief Tab in a sulphur.editor.controls.DynamicDockTabControl.
   * @author Stan Pepels
   */
  public class DynamicDockTab : TabItem
  {
    /**
     * @brief Constructor that sets AllowDrop to true.
     */
    public DynamicDockTab()
    {
      AllowDrop = true;
    }
  }
}
