﻿using sulphur.editor.utils;
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
  /// Interaction logic for Vector3Inspector.xaml
  /// </summary>
  public partial class Vector3Inspector : UserControl
  {
    private VarRef var_;

    public Vector3Inspector()
    {
      InitializeComponent();
    }

    public Vector3Inspector(VarRef var)
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

      Vector3 vec3 = (Vector3)var_.Value;

      X.Text = vec3.X.ToString("0.00");
      Y.Text = vec3.Y.ToString("0.00");
      Z.Text = vec3.Z.ToString("0.00");
    }

    private void UpdateVariable()
    {
      if (var_ == null)
      {
        return;
      }

      var_.Value = new Vector3(
        float.Parse(X.Text),
        float.Parse(Y.Text),
        float.Parse(Z.Text));
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
