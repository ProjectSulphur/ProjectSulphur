using FontAwesome.WPF;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

namespace sulphur.editor.controls
{
  /**
   * @class sulphur.editor.controls.ToolbarControl : UserControl
   * @brief User control that displays the toolbar options.
   * @author Stan pepels, Jelle de Haan
   */
  public partial class ToolbarControl : UserControl
  {
    /**
     * @brief Initializes the WPF components.
     */
    public ToolbarControl()
    {
      InitializeComponent();
    }

    /**
      * @brief Event that occurs on clicking the fast backward button
      * @remarks Sends a message to the engine to start rewinding
      * @param[in] sender (object) The sender of this event
      * @param[in] e (RoutedEventArgs) The event arguments
      */
    private void FastBackwardButtonClick(object sender, RoutedEventArgs e)
    {
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kFastBackward, null, 0);

    }

    /**
    * @brief Event that occurs on clicking the backward button
    * @remarks Sends a message to the engine to rewind one frame
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void BackwardButtonClick(object sender, RoutedEventArgs e)
    {
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kPreviousFrame, null, 0);

    }

    /**
    * @brief Event that occurs on clicking the play button
    * @remarks Sends a message to the engine to start or pause the simulation
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void PlayButtonClick(object sender, RoutedEventArgs e)
    {
      ToggleButton button = ((ToggleButton)sender);

      if (button.IsChecked == true)
      {
        Awesome.SetContent(button, FontAwesomeIcon.Stop);

        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kStartedPlaying, null, 0);
      }
      else
      {
        Awesome.SetContent(button, FontAwesomeIcon.Play);

        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kStoppedPlaying, null, 0);
      }


    }

    /**
    * @brief Event that occurs on clicking the forward button
    * @remarks Sends a message to the engine to forward one frame
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void ForwardButtonClick(object sender, RoutedEventArgs e)
    {
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kNextFrame, null, 0);

    }

    /**
    * @brief Event that occurs on clicking the fast forward button
    * @remarks Sends a message to the engine to fast forward
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void FastForwardButtonClick(object sender, RoutedEventArgs e)
    {
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kFastForward, null, 0);

    }

    /**
    * @brief Event that occurs on clicking the translate gizmo button
    * @remarks Sends a message to the engine to fast forward
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void TranslateGizmoSelectedButtonClick(object sender, RoutedEventArgs e)
    {
      rotate_gizmo_button.IsChecked = false;
      scale_gizmo_button.IsChecked = false;
      translate_gizmo_button.IsChecked = true;

      native.messages.SetTransformGizmoMessage msg = new native.messages.SetTransformGizmoMessage();
      msg.type = (Int32)native.messages.SetTransformGizmoMessage.Type.kTranslate;

      byte[] buffer = Utils.StructToBytes(msg);
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kSetTransformGizmo, buffer, (uint)buffer.Length);

    }

    /**
    * @brief Event that occurs on clicking the rotate gizmo button
    * @remarks Sends a message to the engine to fast forward
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void RotateGizmoSelectedButtonClick(object sender, RoutedEventArgs e)
    {
      translate_gizmo_button.IsChecked = false;
      scale_gizmo_button.IsChecked = false;
      rotate_gizmo_button.IsChecked = true;

      native.messages.SetTransformGizmoMessage msg = new native.messages.SetTransformGizmoMessage();
      msg.type = (Int32)native.messages.SetTransformGizmoMessage.Type.kRotate;

      byte[] buffer = Utils.StructToBytes(msg);
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kSetTransformGizmo, buffer, (uint)buffer.Length);

    }

    /**
    * @brief Event that occurs on clicking the scale gizmo button
    * @remarks Sends a message to the engine to fast forward
    * @param[in] sender (object) The sender of this event
    * @param[in] e (RoutedEventArgs) The event arguments
    */
    private void ScaleGizmoSelectedButtonClick(object sender, RoutedEventArgs e)
    {
      translate_gizmo_button.IsChecked = false;
      rotate_gizmo_button.IsChecked = false;
      scale_gizmo_button.IsChecked = true;

      native.messages.SetTransformGizmoMessage msg = new native.messages.SetTransformGizmoMessage();
      msg.type = (Int32)native.messages.SetTransformGizmoMessage.Type.kScale;

      byte[] buffer = Utils.StructToBytes(msg);
      native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kSetTransformGizmo, buffer, (uint)buffer.Length);

    }

    private void PauseButtonClick(object sender, RoutedEventArgs e)
    {
      if ((sender as ToggleButton).IsChecked == false)
      {
        native.Networking.SNetSendData(
         (uint)native.NetworkMessages.kContinuePlaying, null, 0);
      }
      else
      {
        native.Networking.SNetSendData(
          (uint)native.NetworkMessages.kPause, null, 0);
      }
    }
  }
}
